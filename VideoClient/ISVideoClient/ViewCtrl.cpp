#include "stdafx.h"
#include "ViewCtrl.h"

using namespace Gdiplus;
GdiplusStartupInput gdiplusStartupInput;
ULONG_PTR gdiplusToken;

CViewCtrlWnd::CViewCtrlWnd() : m_pOwner(NULL), m_bInit(false), m_hBkBrush(NULL)
{
	m_pImageData = NULL;
	m_nImageLen = 0;
}

CViewCtrlWnd::~CViewCtrlWnd()
{
	if (NULL != m_hWnd)
		CloseWindow(m_hWnd);
}

void CViewCtrlWnd::onBufUpdated( const HDC hdc,int x, int y, int cx, int cy )
{
	RECT rcClient;
	GetClientRect(m_hWnd,&rcClient);
	RECT rcInvalid = {rcClient.left + x, rcClient.top + y, rcClient.left + x + cx, rcClient.top + y + cy};
	InvalidateRect(m_hWnd, &rcInvalid ,TRUE);
}


void CViewCtrlWnd::RoundRectPath(INT x, INT y, INT width, INT height, INT cornerX, INT cornerY)
{

}

void CViewCtrlWnd::SetWndImageData(BYTE* pData, UINT nDataLen)
{
	m_pImageData = pData;
	m_nImageLen = nDataLen;
}

void CViewCtrlWnd::SetWindowImage(LPCTSTR pstrName)
{
	CDuiRect rcWnd;
	::GetWindowRect(*this, &rcWnd);
	::InvalidateRect(m_hWnd, &rcWnd, TRUE);
}

void CViewCtrlWnd::Init(CViewCtrlUI* pOwner)
{
	m_pOwner = pOwner;
	m_bInit = true;
	if (NULL != m_hWnd)
		CloseWindow(m_hWnd);
	if (m_hWnd == NULL)
	{
		RECT rcPos = m_pOwner->GetPos();
		UINT uStyle = UI_WNDSTYLE_CHILD;
		HWND hWnd = Create(m_pOwner->GetManager()->GetPaintWindow(), NULL, uStyle, 0, rcPos);
		ASSERT(hWnd);
	}
}

LPCTSTR CViewCtrlWnd::GetWindowClassName() const
{
	return _T("ViewCtrl");
}

void CViewCtrlWnd::OnFinalMessage(HWND /*hWnd*/)
{
	//m_pOwner->Invalidate();
	m_pOwner->m_pWindow = NULL;
	if( m_hBkBrush != NULL ) ::DeleteObject(m_hBkBrush);
	delete this;
}

LRESULT CViewCtrlWnd::OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	//KillTimer(m_hWnd, TM_TICKER);
	bHandled = TRUE;
	return 0;
}

LRESULT CViewCtrlWnd::OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	PAINTSTRUCT ps = { 0 };
	HDC hDC = ::BeginPaint(m_hWnd, &ps);
	DWORD dwBkColor = m_pOwner->GetBkColor();
	DWORD dwBorderColor = m_pOwner->GetBorderColor();
	::RECT rect;
	::GetClientRect(m_hWnd, &rect);
	::SetBkMode(hDC, TRANSPARENT);
	//HBRUSH hBrush =  CreateSolidBrush( RGB(GetBValue(dwBkColor), GetGValue(dwBkColor), GetRValue(dwBkColor)));
	//::FillRect(hDC, &rect, hBrush);
	//::DeleteObject(hBrush);
	Gdiplus::Graphics graphics(hDC);
	CDuiString strFilePath = m_pOwner->GetManager()->GetResourcePath();
	strFilePath += m_pOwner->GetBkImage();
	CRenderEngine::DrawColor(hDC, rect, dwBkColor);
	CRenderEngine::DrawRoundRect(hDC, rect, 1, m_pOwner->GetHostRoundConer().cx, m_pOwner->GetHostRoundConer().cy, dwBorderColor);
	CRenderEngine::DrawText(hDC, m_pOwner->GetManager(), rect, m_pOwner->GetText(), dwBorderColor, 31, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	::SetCaretPos(0, 0);
	::EndPaint(m_hWnd, &ps);
	bHandled = FALSE;
	return 0;
}

LRESULT CViewCtrlWnd::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return 0;
}

LRESULT CViewCtrlWnd::OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
// 	SIZE szRoundCorner = m_pOwner->GetHostRoundConer();
	CDuiRect rcWnd;
	::GetWindowRect(*this, &rcWnd);
	rcWnd.Offset(-rcWnd.left, -rcWnd.top);
	rcWnd.right++; rcWnd.bottom++;
	HRGN hRgn = ::CreateRoundRectRgn(rcWnd.left, rcWnd.top, rcWnd.right, rcWnd.bottom, m_pOwner->GetHostRoundConer().cx, m_pOwner->GetHostRoundConer().cy);
	::SetWindowRgn(*this, hRgn, TRUE);
	::DeleteObject(hRgn);
	m_pOwner->NeedParentUpdate();
	return 0;
}

LRESULT CViewCtrlWnd::OnMouseEvent(UINT uMsg, WPARAM wParam,LPARAM lParam, BOOL& bHandled)
{
	if (uMsg == WM_LBUTTONDOWN || uMsg == WM_MBUTTONDOWN || uMsg == WM_RBUTTONDOWN)
	{
		SetFocus(m_hWnd);
		SetCapture(m_hWnd);
	}
	else if (uMsg == WM_LBUTTONUP || uMsg == WM_MBUTTONUP || uMsg == WM_RBUTTONUP)
	{
		ReleaseCapture();
	}
	if (uMsg == WM_LBUTTONDBLCLK)
	{
		if(TRUE == m_pOwner->GetTag())
			::PostMessage(m_pOwner->GetManager()->GetPaintWindow(), UM_CHANGE_VIDEOMODE, 0, (LPARAM)m_pOwner);
	}

	RECT rcClient;
	GetClientRect(m_hWnd, &rcClient);

	int x = GET_X_LPARAM(lParam)-rcClient.left;
	int y = GET_Y_LPARAM(lParam)-rcClient.top;

	unsigned int flags = 0;

	return 0;
}

LRESULT CViewCtrlWnd::OnMouseWheel(UINT uMsg, WPARAM wParam,LPARAM lParam, BOOL& bHandled)
{
	POINT pt;
	pt.x = GET_X_LPARAM(lParam);
	pt.y = GET_Y_LPARAM(lParam);
	ScreenToClient(m_hWnd, &pt);

	return 0;
}

LRESULT CViewCtrlWnd::OnKeyDown(UINT uMsg, WPARAM wParam,LPARAM lParam, BOOL& bHandled)
{
	return 0;
}
LRESULT CViewCtrlWnd::OnKeyUp(UINT uMsg, WPARAM wParam,LPARAM lParam, BOOL& bHandled)
{

	return 0;
}

LRESULT CViewCtrlWnd::OnChar(UINT uMsg, WPARAM wParam,LPARAM lParam, BOOL& bHandled)
{

	//flags = HIWORD(lParam);

	return 0;
}

LRESULT CViewCtrlWnd::OnSetFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
//	bHandled = TRUE;
	return 0;
}

LRESULT CViewCtrlWnd::OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{

	//bHandled = TRUE;
	return 0;
}

LRESULT CViewCtrlWnd::OnContextMenu(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	POINT pt;
	pt.x = GET_X_LPARAM(lParam);
	pt.y = GET_Y_LPARAM(lParam);

	if (pt.x != -1 && pt.y != -1)
		ScreenToClient(m_hWnd, &pt);

	return 0;
}

LRESULT CViewCtrlWnd::OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
// 	if(wParam == TM_TICKER)
// 	{
// 	}
	bHandled = TRUE;
	return 0;
}

LRESULT CViewCtrlWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LRESULT lRes = 0;
	BOOL bHandled = TRUE;
	switch (uMsg)
	{
	case WM_PAINT:			lRes = OnPaint(uMsg, wParam, lParam, bHandled); break;
	case WM_CREATE:			lRes = OnCreate(uMsg, wParam, lParam, bHandled); break;
	case WM_DESTROY:		lRes = OnDestroy(uMsg, wParam, lParam, bHandled); break;
	case WM_MOUSEWHEEL:		lRes = OnMouseWheel(uMsg, wParam, lParam, bHandled); break;
	case WM_SIZE:			lRes = OnSize(uMsg, wParam, lParam, bHandled); break;
	case WM_CHAR:			lRes = OnChar(uMsg, wParam, lParam, bHandled); break;
	case WM_KEYDOWN:		lRes = OnKeyDown(uMsg, wParam, lParam, bHandled); break;
	case WM_KEYUP:			lRes = OnKeyUp(uMsg, wParam, lParam, bHandled); break;
	case WM_KILLFOCUS:		lRes = OnKillFocus(uMsg, wParam, lParam, bHandled); break;
	case WM_SETFOCUS:		lRes = OnSetFocus(uMsg, wParam, lParam, bHandled); break;
	case WM_TIMER:			lRes = OnTimer(uMsg, wParam, lParam, bHandled); break;
	case WM_CONTEXTMENU:	lRes= OnContextMenu(uMsg, wParam, lParam, bHandled); break;	
	case WM_LBUTTONDOWN:	
	case WM_MBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONDOWN:
	case WM_MBUTTONUP:		
	case WM_RBUTTONUP:
	case WM_LBUTTONDBLCLK:
	case WM_MBUTTONDBLCLK:
	case WM_RBUTTONDBLCLK:
	case WM_MOUSEMOVE:
	case WM_MOUSELEAVE:
		lRes = OnMouseEvent(uMsg, wParam, lParam, bHandled); break;
	default:				bHandled = FALSE; break;
	}
	if (bHandled) return lRes;

	return CWindowWnd::HandleMessage(uMsg, wParam, lParam);
}

///////////////////////////////////////////////////////////////////////////////
//////////////
//////////////

CViewCtrlUI::CViewCtrlUI(void):
m_pWindow(NULL),
m_strStatusImage(_T(""))
{
 	SetAttribute("bkcolor", "#FFC7C5C8");
 	SetAttribute("bordercolor", "#FF003003");
 	SetAttribute("bordersize", "1");
	SetAttribute("borderround", "25,25");
}

CViewCtrlUI::~CViewCtrlUI(void)
{	
	if (m_pWindow != NULL)
	{
		m_pWindow->Close();
	}
}

void CViewCtrlUI::SetVisible(bool bVisible)
{
	CControlUI::SetVisible(bVisible);
	SetInternVisible(bVisible);
}

void CViewCtrlUI::SetInternVisible(bool bVisible)
{
	CControlUI::SetInternVisible(bVisible);
	if (m_pWindow != NULL)
		::ShowWindow(m_pWindow->GetHWND(), bVisible?SW_SHOW:SW_HIDE); 
}

void CViewCtrlUI::ReOpenWindow()
{
	if (m_pWindow)
	{
		m_pWindow->Init(this);
		m_pWindow->ShowWindow();
	}
}

void CViewCtrlUI::SetHostRound(SIZE size)
{
	m_szRoundCorner = size;
}

HWND CViewCtrlUI::GetHostWindow()
{
	if (NULL != m_pWindow)
		return m_pWindow->m_hWnd;
	return NULL;
}

void CViewCtrlUI::SetHostImageData(BYTE* pData, UINT nDataLen)
{
	if(NULL != m_pWindow)
	{
		m_pWindow->SetWndImageData(pData, nDataLen);
		RECT rect;
		::GetClientRect(m_pWindow->GetHWND(), &rect);
		::InvalidateRect(m_pWindow->GetHWND(), &rect, TRUE);
	}
}

void CViewCtrlUI::DoInit()
{
	m_pWindow = new CViewCtrlWnd();
	if(m_pWindow)
	{
		m_pWindow->Init(this);
		m_pWindow->ShowWindow();
	}	
}

void CViewCtrlUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
{
	if (_tcsicmp(pstrName, _T("borderround")) == 0)
	{
		SIZE cxyRound = { 0 };
		LPTSTR pstr = NULL;
		cxyRound.cx = _tcstol(pstrValue, &pstr, 10);  ASSERT(pstr);
		cxyRound.cy = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);
		SetHostRound(cxyRound);
	}
	if( _tcscmp(pstrName, _T("statusImage")) == 0 ) m_strStatusImage = pstrValue;
	CControlUI::SetAttribute(pstrName, pstrValue);
}

void CViewCtrlUI::PaintStatusImage(HDC hDC)
{
	DrawImage(hDC, m_strStatusImage);
}
	
LPCTSTR CViewCtrlUI::GetClass() const
{
	return _T("ViewCtrl");
}

LPVOID CViewCtrlUI::GetInterface(LPCTSTR pstrName)
{
	if( _tcscmp(pstrName, _T("ViewCtrl")) == 0 ) return static_cast<CViewCtrlUI*>(this);
	return CControlUI::GetInterface(pstrName);
}

void CViewCtrlUI::DoEvent(TEventUI& event)
{

	if( event.Type == UIEVENT_SETCURSOR )
	{
		::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(IDC_ARROW)));
		return;
	}
		CControlUI::DoEvent(event);
}

void CViewCtrlUI::SetPos(RECT rc, bool bNeedInvalidate)
{
	CControlUI::SetPos(rc);	
	::SetWindowPos(m_pWindow->GetHWND(), NULL, rc.left, rc.top, rc.right - rc.left, 
		rc.bottom - rc.top, SWP_NOZORDER | SWP_NOACTIVATE);
	::InvalidateRect(m_pWindow->GetHWND(), &rc, TRUE);
}

void CViewCtrlUI::SetURL( wstring strValue)
{
	m_pWindow->m_strUrl = strValue;
}

void CViewCtrlUI::SetFile( wstring strValue)
{
	m_pWindow->m_strUrl = strValue;
}
