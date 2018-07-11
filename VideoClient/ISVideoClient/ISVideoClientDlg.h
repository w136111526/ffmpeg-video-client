#pragma once

#include "ViewCtrl.h"
#include "IsPlayOpencv.h"
#include "IsVideoManageThread.h"

class CISVideoClientWnd: public WindowImplBase
{
public:
	CISVideoClientWnd(LPCTSTR pszXMLPath);
	~CISVideoClientWnd();

	//override WindowImplBase
	LPCTSTR GetWindowClassName() const override;
	CDuiString GetSkinFile() override;

     void InitWindow();
	 void OnPrepare(TNotifyUI& msg);
	 void OnFinalMessage( HWND hWnd );
     void Notify(TNotifyUI& msg);
	 CControlUI* CreateControl(LPCTSTR pstrClassName) override;
	 LRESULT HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) override;
	 LRESULT OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	 LRESULT OnChangeVideoMode(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

private:

	//界面管理
	HICON								m_hIcon;
	bool								m_bFullScreenMode;
	int									m_nCurrentVideoIndex;
	int									m_nVideoColums;

	CDuiString							m_strXmlPath;
	CButtonUI							*m_pbuttonSetting;
	CButtonUI							*m_pButtonStart;
	CButtonUI							*m_pbuttonStop;
	CButtonUI							*m_pButtonShowLeftPanel;
	CButtonUI							*m_pButtonHideLeftPanel;
	CListUI								*m_pUrlList;
	CViewCtrlUI							*m_pVideoCtrl;
	CListUI								*m_pListUrl;
	CVerticalLayoutUI					*m_pVerLeftPanel;
	CVerticalLayoutUI					*m_pVerPlayPanel;
	CTileLayoutUI						*m_pTileLayoutList;

	CIsPlayOpencv*						m_pIsPlayOpencv[MAX_VIDEO_LIST];
	CIsVideoManageThread*				m_pIsVideoManageThread;

	//VideoData
	Vect_VideoInfo					m_vectCurrentVideoInfo;

	//内部函数
	void StartTaskProcess(BOOL bStart);
	void SetShowLeftPanel(bool bShow);
	void SetFullScreenModeByClass(bool bFull, CControlUI * pViewCtrl = NULL);                                    // 全屏
	void ChangeVideoDisplayByID(int index);
	void InitVideoDisplayInfo();
};