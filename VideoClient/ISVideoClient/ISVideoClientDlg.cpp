#include "stdafx.h"
#include "ISVideoClientDlg.h"
#include "DlgUrlList.h"

/////////////////////////////////////////////////////////////////////////////////////////
//////////////////
//////////////////

CISVideoClientWnd::CISVideoClientWnd( LPCTSTR pszXMLPath ) : m_strXmlPath(pszXMLPath)
{
	 m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	 m_bFullScreenMode = false;
	 m_nVideoColums = 1;
	 m_nCurrentVideoIndex = 0;

	 m_pButtonStart = NULL;
	 m_pbuttonStop = NULL;
	 m_pbuttonSetting = NULL;
	 m_pVerPlayPanel = NULL;
	 m_pIsVideoManageThread = new CIsVideoManageThread;
}

CISVideoClientWnd::~CISVideoClientWnd()
{
}

LPCTSTR CISVideoClientWnd::GetWindowClassName() const
{
	return _T("MainWnd");
}

CDuiString CISVideoClientWnd::GetSkinFile()
{
	return m_strXmlPath;
}

CControlUI* CISVideoClientWnd::CreateControl( LPCTSTR pstrClassName )
{
// 	if (_tcsicmp(pstrClassName, _T("AnimLayout")) == 0)
// 		return new AnimLayout;
	if (_tcsicmp(pstrClassName, _T("ViewCtrl")) == 0)
		return  new CViewCtrlUI;
	return NULL;
}

//初始化界面
void CISVideoClientWnd::InitWindow()
{
	m_pVideoCtrl = GetControlByName<CViewCtrlUI>("video1");
	m_pButtonHideLeftPanel = static_cast<CButtonUI*>(m_pm.FindControl(_T("btnPlaylistHide")));
	m_pButtonShowLeftPanel = static_cast<CButtonUI*>(m_pm.FindControl(_T("btnPlaylistShow")));
	m_pVerLeftPanel = static_cast<CVerticalLayoutUI*>(m_pm.FindControl(_T("leftpanel")));
	ASSERT(m_pVerLeftPanel);
	m_pListUrl = GetControlByName<CListUI>("UrlList");
	ASSERT(m_pListUrl);
	m_pTileLayoutList = GetControlByName<CTileLayoutUI>("VideoList");
	ASSERT(m_pTileLayoutList);
	m_nVideoColums = m_pTileLayoutList->GetColumns();
	m_pVerPlayPanel = GetControlByName<CVerticalLayoutUI>("PlayPanel");
	ASSERT(m_pVerPlayPanel);
	m_pButtonStart = GetSubControlByName<CButtonUI>("btnPlay", m_pVerPlayPanel);
	InitVideoDisplayInfo();
	::SendMessage(m_hWnd, WM_SETICON, TRUE, (LPARAM)m_hIcon);			// 设置大图标
	::SendMessage(m_hWnd, WM_SETICON, FALSE, (LPARAM)m_hIcon);		// 设置小图标

	//m_pIsPlayOpencv.reset();
	CenterWindow();
}

void CISVideoClientWnd::OnFinalMessage( HWND hWnd)
{
	ASSERT(m_pPsThread == NULL);	
	if (nullptr != m_pIsVideoManageThread)
		delete m_pIsVideoManageThread;
}

void CISVideoClientWnd::OnPrepare(TNotifyUI& msg)
{
	//Add Anim
}

void CISVideoClientWnd::Notify( TNotifyUI& msg )
{
	if(msg.sType == _T("windowinit"))
	{
		OnPrepare(msg);
	}
	else if( msg.sType == _T("click") ) 
	{	
		if(msg.pSender->GetName() == _T("setting"))
		{
			CDlgUrlList dlg;
			if(IDOK == dlg.DoModal())
				InitVideoDisplayInfo();
			//((CViewCtrlUI*)m_pVideoLayoutList->GetItemAt(1))->SetBorderColor(RGB(128, 0, 128));
		}
		if (msg.pSender->GetName() == _T("btnPlay"))
		{
			StartTaskProcess(TRUE);
		}
		if (msg.pSender->GetName() == _T("btnPause"))
		{
			StartTaskProcess(FALSE);
		}
		if (msg.pSender->GetName() == _T("btnPlaylistShow"))
		{
			SetShowLeftPanel(true);
		}
		if (msg.pSender->GetName() == _T("btnPlaylistHide"))
		{
			SetShowLeftPanel(false);
		}
		/*if (msg.pSender->GetName() == _T("btnScreenNorm"))
		{
			SetFullScreenModeByClass(false);
		}
		if (msg.pSender->GetName() == _T("btnScreenFull"))
		{
			SetFullScreenModeByClass(true, m_pTileLayoutList->GetItemAt(0));
		}
		if (msg.pSender->GetName() == _T("btnPrevious"))
		{
			m_nCurrentVideoIndex--;
			if (m_nCurrentVideoIndex < 0)
				m_nCurrentVideoIndex = m_pTileLayoutList->GetCount() - 1;
			SetFullScreenModeByClass(true, m_pTileLayoutList->GetItemAt(m_nCurrentVideoIndex));
		}
		if (msg.pSender->GetName() == _T("btnNext"))
		{
			m_nCurrentVideoIndex++;
			if (m_nCurrentVideoIndex > m_pTileLayoutList->GetCount() - 1)
				m_nCurrentVideoIndex = 0;
			SetFullScreenModeByClass(true, m_pTileLayoutList->GetItemAt(m_nCurrentVideoIndex));
		}*/
	}
	__super::Notify(msg);
}

void CISVideoClientWnd::StartTaskProcess(BOOL bStart)
{
	if (bStart)
	{
		m_pIsVideoManageThread->InitVideoInfo(m_vectCurrentVideoInfo);
		m_pIsVideoManageThread->StartVideoDisPlay();
	}
	else
	{
		for (int i = 0; i < m_pTileLayoutList->GetCount(); i++)
		{
			CViewCtrlUI* pViewCtrl = (CViewCtrlUI*)m_pTileLayoutList->GetItemAt(i);
			pViewCtrl->ReOpenWindow();
		}
		m_pIsVideoManageThread->StopAllVideoDisPlay();
		SetFullScreenModeByClass(false);
	}
	GetControlByName<CButtonUI>("btnPlay")->SetVisible(!bStart);
	GetControlByName<CButtonUI>("btnPause")->SetVisible(bStart);
	GetControlByName<CButtonUI>("setting")->SetVisible(!bStart);
}

void CISVideoClientWnd::SetShowLeftPanel(bool bShow)
{
	m_pVerLeftPanel->SetVisible(bShow);
	m_pButtonShowLeftPanel->SetVisible(!bShow);
	m_pButtonHideLeftPanel->SetVisible(bShow);
	for (int i = 0; i < m_pTileLayoutList->GetCount(); i++)
		::InvalidateRgn(((CViewCtrlUI*)m_pTileLayoutList->GetItemAt(i))->m_pWindow->GetHWND(), NULL, TRUE);
}

void CISVideoClientWnd::SetFullScreenModeByClass(bool bFull, CControlUI * pViewCtrl)
{
	m_bFullScreenMode = bFull;
	m_pTileLayoutList->SetColumns(m_bFullScreenMode ? 1 : m_nVideoColums);			//全屏模式下Tile列数改变
	m_pTileLayoutList->EnableScrollBar(!m_bFullScreenMode);
	if (m_bFullScreenMode)
	{
		for (int i = 0; i < m_pTileLayoutList->GetCount(); i++)
		{
			if (pViewCtrl != m_pTileLayoutList->GetItemAt(i))
			{
				m_pTileLayoutList->GetItemAt(i)->SetVisible(!m_bFullScreenMode);
			}
			else
			{
				m_pTileLayoutList->GetItemAt(i)->SetFixedHeight(m_pTileLayoutList->GetHeight() - 2);
				m_pTileLayoutList->GetItemAt(i)->SetVisible(true);
			}
		}
	}
	else
	{
		for (int i = 0; i < m_pTileLayoutList->GetCount(); i++)
		{
			m_pTileLayoutList->GetItemAt(i)->SetVisible(true);
			m_pTileLayoutList->GetItemAt(i)->SetFixedHeight(150);
		}
	}
	m_pTileLayoutList->NeedUpdate();
}

void CISVideoClientWnd::ChangeVideoDisplayByID(int index)
{

}

void CISVideoClientWnd::InitVideoDisplayInfo()
{
	vector<string>& UrlList = CIsSystem::GetInstance()->m_IsOption.GetUrlList();
	CDialogBuilder builder1;
	m_pTileLayoutList->RemoveAll();
	m_pListUrl->RemoveAll();
	m_vectCurrentVideoInfo.clear();
	for (int i = 0; i < UrlList.size(); i++)
	{
		Tuple_VideoInfo tuple_video;
		CViewCtrlUI* pViewCtrl = new CViewCtrlUI;
		pViewCtrl->SetFixedHeight(150);
		CDuiString str;
		str.Format(_T("1080P高清视频%d"), i);
		pViewCtrl->SetText(str);
		pViewCtrl->SetUserData(UrlList[i].c_str());
		m_pTileLayoutList->Add(pViewCtrl);
		CListContainerElementUI* pListItem1 = (CListContainerElementUI*)builder1.Create(_T("listitem.xml"), NULL, this, &m_pm, NULL);
		CLabelUI*	pLabel		= GetSubControlByName<CLabelUI>("Info", pListItem1);
		CButtonUI*	pUrlStatus	= GetSubControlByName<CButtonUI>("UrlStatus", pListItem1);
		str.Format(_T("%02d. 192.168.110.64:554"), i + 1);
		pLabel->SetText(str);
		pUrlStatus->SetUserData(UrlList[i].c_str());
		str.Format("地址详情: %s", UrlList[i].c_str());
		pLabel->SetToolTip(str);
		m_pListUrl->Add(pListItem1);
		m_vectCurrentVideoInfo.push_back(make_tuple(pViewCtrl->GetHostWindow(), UrlList[i].c_str(), pViewCtrl));
	}
}

LRESULT CISVideoClientWnd::HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	LRESULT lRes = 0;

	switch (uMsg)
	{
		case WM_TIMER:				lRes = OnTimer(uMsg, wParam, lParam, bHandled); break;
		case UM_CHANGE_VIDEOMODE:	lRes = OnChangeVideoMode(uMsg, wParam, lParam, bHandled); break;
		case  WM_LBUTTONDBLCLK :
		{
			CViewCtrlUI* pView = (CViewCtrlUI*)lParam;
			break;
		}
		default: break;
	}
	return lRes;
}

LRESULT CISVideoClientWnd::OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{

	return 0;
}

LRESULT CISVideoClientWnd::OnChangeVideoMode(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled)
{
	CViewCtrlUI* pViewCtrl = (CViewCtrlUI*)lParam;
	m_pVerLeftPanel = static_cast<CVerticalLayoutUI*>(m_pm.FindControl(_T("leftpanel")));
	ASSERT(m_pVerLeftPanel);
	if(!m_bFullScreenMode)
		SetFullScreenModeByClass(true, pViewCtrl);
	else
		SetFullScreenModeByClass(false);

	return 0;
}

