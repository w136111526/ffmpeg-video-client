#pragma once

#include <memory>
#include <mutex>
#include "../video/decoderinterface.h"
struct IFrameListener;

struct IDirect3D9;
struct IDirect3DDevice9;
struct IDirect3DSurface9;
struct IDirectXVideoProcessorService;
struct IDirectXVideoProcessor;

class CPDisplayD3D
{
	friend class FrameListener;

public:
	//explicit CPDisplayD3D(HWND hWnd);
	CPDisplayD3D();
	~CPDisplayD3D();
	void OpenVideoUrl(HWND hwnd, CString strUrl, bool bUseD3D);
	void CloseVideo();
	void updateFrame();
	bool ProcessVideo();
private:
	bool InitializeD3D9();
	bool InitializeExtra(bool createSurface);
	void DestroyExtra();
	void DestroyD3D9();
	bool ResetDevice();



	RECT GetScreenPosition();
	HWND	m_hWnd;
	std::unique_ptr<IFrameListener> m_frameListener;
	std::unique_ptr<IFrameDecoder> m_frameDecoder;
	CSize m_sourceSize;
	CSize m_aspectRatio;

	CRITICAL_SECTION m_csLock;

	CComPtr<IDirect3D9> m_pD3D9;
	CComPtr<IDirect3DDevice9>  m_pD3DD9;
	CComPtr<IDirect3DSurface9> m_pD3DRT;
	CComPtr<IDirect3DSurface9> m_pMainStream;
};

