#include "stdafx.h"
#include "PDisplayD3D.h"

//#include "../video/decoderinterface.h"

//#include "D3DFont.h"

#include <initguid.h>
#include <d3d9.h>

#ifdef USE_DXVA2
#include <dxva2api.h>
#endif

#include <Gdiplus.h>

#include <vector>

#define CONVERT_FROM_YUV420P

namespace {
	const D3DFORMAT VIDEO_RENDER_TARGET_FORMAT = D3DFMT_X8R8G8B8;
	const D3DFORMAT VIDEO_MAIN_FORMAT = D3DFMT_YUY2;
	//const D3DFORMAT VIDEO_MAIN_FORMAT = (D3DFORMAT)MAKEFOURCC('I', 'M', 'C', '3');

	const UINT BACK_BUFFER_COUNT = 1;
	const UINT SUB_STREAM_COUNT = 0;
	const UINT DWM_BUFFER_COUNT = 4;
	const UINT VIDEO_FPS = 60;

	HMODULE g_hRgb9rastDLL = NULL;

	PVOID g_pfnD3D9GetSWInfo = NULL;

	//////////////////////////////////////////////////////////////////////////////

	DWORD RGBtoYUV(const D3DCOLOR rgb)
	{
		const INT A = HIBYTE(HIWORD(rgb));
		const INT R = LOBYTE(HIWORD(rgb)) - 16;
		const INT G = HIBYTE(LOWORD(rgb)) - 16;
		const INT B = LOBYTE(LOWORD(rgb)) - 16;

		//
		// studio RGB [16...235] to SDTV ITU-R BT.601 YCbCr
		//
		INT Y = (77 * R + 150 * G + 29 * B + 128) / 256 + 16;
		INT U = (-44 * R - 87 * G + 131 * B + 128) / 256 + 128;
		INT V = (131 * R - 110 * G - 21 * B + 128) / 256 + 128;

		return D3DCOLOR_AYUV(A, Y, U, V);
	}

	BOOL RegisterSoftwareRasterizer(IDirect3D9* g_pD3D9)
	{
		if (!g_hRgb9rastDLL)
		{
			return FALSE;
		}

		HRESULT hr = g_pD3D9->RegisterSoftwareDevice(g_pfnD3D9GetSWInfo);

		if (FAILED(hr))
		{
			TRACE("RegisterSoftwareDevice failed with error 0x%x.\n", hr);
			return FALSE;
		}

		return TRUE;
	}


	BOOL InitializeModule()
	{
		//
		// Load these DLLs dynamically because these may not be available prior to Vista.
		//
		g_hRgb9rastDLL = LoadLibrary(TEXT("rgb9rast.dll"));

		if (!g_hRgb9rastDLL)
		{
			TRACE("LoadLibrary(rgb9rast.dll) failed with error %d.\n", GetLastError());
		}
		else
		{
			g_pfnD3D9GetSWInfo = GetProcAddress(g_hRgb9rastDLL, "D3D9GetSWInfo");

			if (!g_pfnD3D9GetSWInfo)
			{
				TRACE("GetProcAddress(D3D9GetSWInfo) failed with error %d.\n", GetLastError());
				return FALSE;
			}
		}

		return TRUE;
	}


	D3DPRESENT_PARAMETERS GetD3dPresentParams(HWND hWnd)
	{
		D3DPRESENT_PARAMETERS D3DPP = { 0 };

		D3DPP.BackBufferWidth = GetSystemMetrics(SM_CXSCREEN);
		D3DPP.BackBufferHeight = GetSystemMetrics(SM_CYSCREEN);

		D3DPP.BackBufferFormat = VIDEO_RENDER_TARGET_FORMAT;
		D3DPP.BackBufferCount = BACK_BUFFER_COUNT;
		D3DPP.SwapEffect = D3DSWAPEFFECT_DISCARD;
		D3DPP.hDeviceWindow = hWnd;
		D3DPP.Windowed = TRUE;//g_bWindowed;
		D3DPP.Flags = D3DPRESENTFLAG_VIDEO;
		D3DPP.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
		D3DPP.PresentationInterval = D3DPRESENT_INTERVAL_ONE;

		return D3DPP;
	}

#ifdef _WIN64
#define _mm_movpi64_epi64(a) _mm_set_epi64x(0, (a))
#define __m64 __int64
#define _mm_empty() ((void)0)
#endif

	void SimdCopyAndConvert(
		__m128i* const __restrict origin0,
		__m128i* const __restrict origin1,
		const __m128i* const __restrict src00,
		const __m128i* const __restrict src01,
		const __m64* const __restrict src0,
		const __m64* const __restrict src1,
		size_t count)
	{
		for (size_t i = 0; i < count; ++i)
		{
			__m128i uv = _mm_unpacklo_epi8(
				_mm_movpi64_epi64(src0[i]),
				_mm_movpi64_epi64(src1[i]));
			_mm_stream_si128(origin0 + i * 2, _mm_unpacklo_epi8(src00[i], uv));
			_mm_stream_si128(origin0 + i * 2 + 1, _mm_unpackhi_epi8(src00[i], uv));
			_mm_stream_si128(origin1 + i * 2, _mm_unpacklo_epi8(src01[i], uv));
			_mm_stream_si128(origin1 + i * 2 + 1, _mm_unpackhi_epi8(src01[i], uv));
		}
		_mm_empty();
	}

	void CopyAndConvert(
		uint32_t* __restrict origin0,
		uint32_t* __restrict origin1,
		const uint8_t* __restrict src00,
		const uint8_t* __restrict src01,
		const uint8_t* __restrict src0,
		const uint8_t* __restrict src1,
		size_t count)
	{
		if (!((intptr_t(origin0) & 15) || (intptr_t(origin1) & 15)
			|| (intptr_t(src00) & 15) || (intptr_t(src01) & 15)
			|| (intptr_t(src0) & 7) || (intptr_t(src1) & 7)))
		{
			const auto simdCount = count / 8;

			SimdCopyAndConvert(
				(__m128i*) origin0,
				(__m128i*) origin1,
				(const __m128i*) src00,
				(const __m128i*) src01,
				(const __m64*) src0,
				(const __m64*) src1,
				simdCount);

			origin0 += simdCount * 8;
			origin1 += simdCount * 8;
			src00 += simdCount * 16;
			src01 += simdCount * 16;
			src0 += simdCount * 8;
			src1 += simdCount * 8;

			count -= simdCount * 8;
		}

		for (unsigned int j = 0; j < count; ++j)
		{
			const uint32_t uv = (src0[j] << 8) | (src1[j] << 24);
			origin0[j] = uv | src00[j * 2] | (src00[j * 2 + 1] << 16);
			origin1[j] = uv | src01[j * 2] | (src01[j * 2 + 1] << 16);
		}
	}

	CComPtr<IDirect3DStateBlock9> InitStateBlock(LPDIRECT3DDEVICE9 pd3dDevice,
		IDirect3DTexture9* pTexture)
	{
		pd3dDevice->BeginStateBlock();
		pd3dDevice->SetTexture(0, pTexture);

		//if (D3DFONT_ZENABLE & m_dwFontFlags)
		//    pd3dDevice->SetRenderState(D3DRS_ZENABLE, TRUE);
		//else
		pd3dDevice->SetRenderState(D3DRS_ZENABLE, FALSE);
		pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
		pd3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		pd3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
		pd3dDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
		pd3dDevice->SetRenderState(D3DRS_ALPHAREF, 0x08);
		pd3dDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL);
		pd3dDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
		pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
		pd3dDevice->SetRenderState(D3DRS_STENCILENABLE, FALSE);
		pd3dDevice->SetRenderState(D3DRS_CLIPPING, TRUE);
		pd3dDevice->SetRenderState(D3DRS_CLIPPLANEENABLE, FALSE);
		pd3dDevice->SetRenderState(D3DRS_VERTEXBLEND, D3DVBF_DISABLE);
		pd3dDevice->SetRenderState(D3DRS_INDEXEDVERTEXBLENDENABLE, FALSE);
		pd3dDevice->SetRenderState(D3DRS_FOGENABLE, FALSE);
		pd3dDevice->SetRenderState(D3DRS_COLORWRITEENABLE,
			D3DCOLORWRITEENABLE_RED | D3DCOLORWRITEENABLE_GREEN |
			D3DCOLORWRITEENABLE_BLUE | D3DCOLORWRITEENABLE_ALPHA);
		pd3dDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
		pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
		pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
		pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
		pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
		pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
		pd3dDevice->SetTextureStageState(0, D3DTSS_TEXCOORDINDEX, 0);
		pd3dDevice->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE);
		pd3dDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
		pd3dDevice->SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
		pd3dDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
		pd3dDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
		pd3dDevice->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_NONE);
		CComPtr<IDirect3DStateBlock9> result;
		pd3dDevice->EndStateBlock(&result);
		return result;
	}
}


class FrameListener : public IFrameListener
{
public:
	explicit FrameListener(CPDisplayD3D* playerD3D) : m_playerD3D(playerD3D) {}

private:
	void updateFrame() override
	{
		m_playerD3D->updateFrame();
	}
	void drawFrame(IFrameDecoder* decoder, unsigned int generation) override
	{
		m_playerD3D->ProcessVideo();
		decoder->finishedDisplayingFrame(generation);
	}

private:
	CPDisplayD3D* m_playerD3D;
};

// CPDisplayD3D::CPDisplayD3D(HWND hWnd) : m_hWnd(hWnd), 
// m_frameListener( new FrameListener(this)),
// m_aspectRatio(1, 1)
// {
// 	InitializeCriticalSection(&m_csLock);
// }

CPDisplayD3D::CPDisplayD3D() :
m_hWnd(NULL),
m_frameListener(new FrameListener(this)),
m_aspectRatio(1, 1),
m_frameDecoder(GetFrameDecoder())
{
	InitializeCriticalSection(&m_csLock);
}

CPDisplayD3D::~CPDisplayD3D()
{
	DeleteCriticalSection(&m_csLock);
}

void CPDisplayD3D::OpenVideoUrl(HWND hwnd, CString strUrl, bool bUseD3D)
{
	m_hWnd = hwnd;
	m_frameDecoder->setFrameListener(m_frameListener.get());
	m_frameDecoder->SetFrameFormat(IFrameDecoder::
#ifdef CONVERT_FROM_YUV420P
		PIX_FMT_YUV420P
#else
		PIX_FMT_YUYV422
#endif
		, bUseD3D);
	CloseVideo();
	//if (strUrl.GetAt(0) != 'r');
	bool	bIsplay = false;
	if (strUrl.Left(4) != "rtsp")
	{
		bIsplay = m_frameDecoder->openFile(strUrl.GetBuffer(0));
	}
	else
	{		
		bIsplay = m_frameDecoder->openUrl(strUrl.GetBuffer(0));
	}
	if (bIsplay)
		m_frameDecoder->play();
	else
		AfxMessageBox("视频路径打开失败");
	
}

void CPDisplayD3D::CloseVideo()
{
	m_frameDecoder->close();
}

void CPDisplayD3D::updateFrame()
{
	EnterCriticalSection(&m_csLock);

	FrameRenderingData data;

	//更新数据
// 	if (!GetDocument()->getFrameDecoder()->getFrameRenderingData(&data))
// 	{
// 		return;
// 	}
	if (!m_frameDecoder->getFrameRenderingData(&data))
		return;
	data.width &= -2; // must be even

	m_aspectRatio.cx = data.aspectNum;
	m_aspectRatio.cy = data.aspectDen;

	if (data.d3d9device)
	{
		if (data.d3d9device != m_pD3DD9)
		{
			m_sourceSize.cx = data.width;
			m_sourceSize.cy = data.height;

			DestroyExtra();
			DestroyD3D9();

			m_pD3DD9 = data.d3d9device;
			m_pD3D9.Release();

			InitializeExtra(false);
		}
	}
	else if (!m_pD3D9 || data.width != m_sourceSize.cx || data.height != m_sourceSize.cy)
	{
		m_sourceSize.cx = data.width;
		m_sourceSize.cy = data.height;
		ResetDevice();
	}

	if (data.surface)
	{
		std::swap(m_pMainStream.p, *data.surface);
	}
	else
	{
		D3DLOCKED_RECT lr;
		HRESULT hr = m_pMainStream->LockRect(&lr, NULL, D3DLOCK_NOSYSLOCK);
		if (FAILED(hr))
		{
			TRACE("LockRect failed with error 0x%x.\n", hr);
			return;
		}

#ifdef CONVERT_FROM_YUV420P
		for (int i = 0; i < data.height / 2; ++i)
		{
			CopyAndConvert(
				(uint32_t*)((char*)lr.pBits + lr.Pitch * 2 * i),
				(uint32_t*)((char*)lr.pBits + lr.Pitch * (2 * i + 1)),
				data.image[0] + data.pitch[0] * 2 * i,
				data.image[0] + data.pitch[0] * (2 * i + 1),
				data.image[1] + data.pitch[1] * i,
				data.image[2] + data.pitch[2] * i,
				data.width / 2);
		}
#else
		const size_t lineSize = (size_t)min(lr.Pitch, data.width * 2);
		for (int i = 0; i < data.height; ++i)
		{
			memcpy((BYTE*)lr.pBits + lr.Pitch * i, data.image[0] + data.width * 2 * i, lineSize);
		}
#endif

		hr = m_pMainStream->UnlockRect();
		if (FAILED(hr))
		{
			TRACE("UnlockRect failed with error 0x%x.\n", hr);
		}
	}
	LeaveCriticalSection(&m_csLock);
}

bool CPDisplayD3D::ProcessVideo()
{
	if (!m_pD3DD9)
	{
		return false;
	}
	EnterCriticalSection(&m_csLock);

	// Check the current status of D3D9 device.
	HRESULT hr = m_pD3DD9->TestCooperativeLevel();
	switch (hr)
	{
	case D3D_OK:
		break;

	case D3DERR_DEVICELOST:
		TRACE("TestCooperativeLevel returned D3DERR_DEVICELOST.\n");
		return true;

	case D3DERR_DEVICENOTRESET:
		TRACE("TestCooperativeLevel returned D3DERR_DEVICENOTRESET.\n");

		if (!m_pD3D9)
		{
			DestroyExtra();
			DestroyD3D9();
			//重新打开视频
			//GetDocument()->getFrameDecoder()->videoReset();
			return false;
		}
		else if (!ResetDevice())
		{
			return false;
		}
		break;

	default:
		TRACE("TestCooperativeLevel failed with error 0x%x.\n", hr);
		return false;
	}

	RECT srcRect = { 0, 0, m_sourceSize.cx, m_sourceSize.cy };
	CRect screenPosition = GetScreenPosition();
	CRect target(POINT{}, screenPosition.Size());
	
	m_pD3DD9->Clear(
		0,
		NULL,
		D3DCLEAR_TARGET,
		D3DCOLOR_XRGB(0, 0, 0),
		1.0f,
		0);

	hr = m_pD3DD9->StretchRect(
		m_pMainStream,
		&srcRect,
		m_pD3DRT,
		&target,
		D3DTEXF_NONE);
	if (FAILED(hr))
	{
		TRACE("StretchRect failed with error 0x%x.\n", hr);
	}
	hr = m_pD3DD9->Present(&target, &screenPosition, m_hWnd, NULL);
	if (FAILED(hr))
	{
		TRACE("Present failed with error 0x%x.\n", hr);
	}
	LeaveCriticalSection(&m_csLock);

	return true;
}

bool CPDisplayD3D::InitializeD3D9()
{
	m_pD3D9.Attach(Direct3DCreate9(D3D_SDK_VERSION));

	if (!m_pD3D9)
	{
		TRACE("Direct3DCreate9 failed.\n");
		return false;
	}

	auto D3DPP = GetD3dPresentParams(m_hWnd);
	HRESULT hr = m_pD3D9->CreateDevice(D3DADAPTER_DEFAULT,
		D3DDEVTYPE_HAL,
		m_hWnd,
		D3DCREATE_FPU_PRESERVE |
		D3DCREATE_MULTITHREADED |
		D3DCREATE_SOFTWARE_VERTEXPROCESSING,
		&D3DPP,
		&m_pD3DD9);
	if (FAILED(hr))
	{
		TRACE("CreateDevice(HAL) failed with error 0x%x.\n", hr);
	}
	//
	// Next try to create a software D3D9 device.
	//
	if (!m_pD3DD9)
	{
		RegisterSoftwareRasterizer(m_pD3D9);

		hr = m_pD3D9->CreateDevice(D3DADAPTER_DEFAULT,
			D3DDEVTYPE_SW,
			m_hWnd,
			D3DCREATE_FPU_PRESERVE |
			D3DCREATE_MULTITHREADED |
			D3DCREATE_SOFTWARE_VERTEXPROCESSING,
			&D3DPP,
			&m_pD3DD9);

		if (FAILED(hr))
		{
			TRACE("CreateDevice(SW) failed with error 0x%x.\n", hr);
		}
	}
	if (!m_pD3DD9)
	{
		return false;
	}
	return true;
}

bool CPDisplayD3D::InitializeExtra(bool createSurface)
{
	HRESULT hr = m_pD3DD9->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &m_pD3DRT);

	if (FAILED(hr))
	{
		TRACE("GetBackBuffer failed with error 0x%x.\n", hr);
		return false;
	}
	if (createSurface)
	{
		hr = m_pD3DD9->CreateOffscreenPlainSurface(
			(m_sourceSize.cx + 7) & ~7,
			m_sourceSize.cy,
			VIDEO_MAIN_FORMAT,
			D3DPOOL_DEFAULT,
			&m_pMainStream,
			NULL);
		if (FAILED(hr))
		{
			TRACE("CreateOffscreenPlainSurface failed with error 0x%x.\n", hr);
			return false;
		}
	}
	return true;
}

void CPDisplayD3D::DestroyExtra()
{
	m_pMainStream.Release();
	m_pD3DRT.Release();
}

void CPDisplayD3D::DestroyD3D9()
{
	m_pD3DD9.Release();
	m_pD3D9.Release();
}

bool CPDisplayD3D::ResetDevice()
{
	bool fullInitialization = true;

	if (m_pD3DD9)
	{
		DestroyExtra();
		auto d3dpp = GetD3dPresentParams(m_hWnd);
		HRESULT hr = m_pD3DD9->Reset(&d3dpp);

		if (FAILED(hr))
		{
			TRACE("Reset failed with error 0x%x.\n", hr);
		}

		if (SUCCEEDED(hr) && InitializeExtra(true))
		{
			fullInitialization = false;
		}
		else
		{
			// If either Reset didn't work or failed to initialize DXVA2 device,
			// try to recover by recreating the devices from the scratch.
			DestroyExtra();
			DestroyD3D9();
		}
	}
	if (fullInitialization && (!InitializeD3D9() || !InitializeExtra(true)))
	{
		return false;
	}

	return true;
}

RECT CPDisplayD3D::GetScreenPosition()
{
	RECT desc;
	GetClientRect(m_hWnd, &desc);

	long long aspectFrameX(m_sourceSize.cx * m_aspectRatio.cx);
	long long aspectFrameY(m_sourceSize.cy * m_aspectRatio.cy);

	RECT target;
	if (aspectFrameY * (desc.right - desc.left) > aspectFrameX * (desc.bottom - desc.top))
	{
		target.top = 0;
		target.bottom = desc.bottom - desc.top;
		LONG width = LONG(aspectFrameX * (desc.bottom - desc.top) / aspectFrameY);
		LONG offset = ((desc.right - desc.left) - width) / 2;
		target.left = offset;
		target.right = width + offset;
	}
	else
	{
		target.left = 0;
		target.right = (desc.right - desc.left);
		LONG height = LONG(aspectFrameY * (desc.right - desc.left) / aspectFrameX);
		LONG offset = ((desc.bottom - desc.top) - height) / 2;
		target.top = offset;
		target.bottom = height + offset;
	}

	return desc;
}
