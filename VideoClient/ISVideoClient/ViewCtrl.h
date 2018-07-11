#pragma once

#define UM_CHANGE_VIDEOMODE		WM_USER	+ 200
class CViewCtrlUI;
class CViewCtrlWnd : public WindowImplBase
{
	friend class CViewCtrlUI;
public:
	CViewCtrlWnd();
	~CViewCtrlWnd();

	void SetWndImageData(BYTE* pData, UINT nDataLen);
	void SetWindowImage(LPCTSTR pstrName);
	
	void Init(CViewCtrlUI* pOwner);
	LPCTSTR GetWindowClassName() const;
	CDuiString GetSkinFile() { return ""; };
	void OnFinalMessage(HWND hWnd);

	LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnMouseEvent(UINT uMsg, WPARAM wParam,LPARAM lParam, BOOL& bHandled); 
	LRESULT OnMouseWheel(UINT uMsg, WPARAM wParam,LPARAM lParam, BOOL& bHandled); 
	LRESULT OnKeyDown(UINT uMsg, WPARAM wParam,LPARAM lParam, BOOL& bHandled);
	LRESULT OnKeyUp(UINT uMsg, WPARAM wParam,LPARAM lParam, BOOL& bHandled);
	LRESULT OnChar(UINT uMsg, WPARAM wParam,LPARAM lParam, BOOL& bHandled);
	LRESULT OnSetFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);	
	LRESULT OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnContextMenu(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

protected:
	virtual void onBufUpdated (const HDC hdc,int x, int y, int cx, int cy);
	void RoundRectPath(INT x, INT y, INT width, INT height, INT cornerX, INT cornerY);
protected:
	CViewCtrlUI* m_pOwner;
	bool		 m_bInit;
	wstring		 m_strUrl;	
	HBRUSH		 m_hBkBrush;
	BYTE*	 	 m_pImageData;
	UINT		 m_nImageLen;
};

class CViewCtrlUI :public CControlUI
{
	friend class CViewCtrlWnd;
public:
	CViewCtrlUI(void);
	~CViewCtrlUI(void);

	LPCTSTR	GetClass() const override;
	LPVOID	GetInterface(LPCTSTR pstrName) override;
	HWND	GetHostWindow();
	void	SetHostImageData(BYTE* pData, UINT nDataLen);
	void	SetVisible(bool bVisible) override;
	void	SetInternVisible(bool bVisible = true) override;
	void	ReOpenWindow();
	void	DoInit() override;
	void	SetPos(RECT rc, bool bNeedInvalidate = true) override;	
	void	DoEvent(TEventUI& event) override;	
	void	SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue) override;
	void	SetURL( wstring strValue);
	void	SetFile(wstring strValue);

	SIZE	GetHostRoundConer() { return m_szRoundCorner; }
	void	SetHostRound(SIZE size);

	void	PaintStatusImage(HDC hDC);

public:
	CViewCtrlWnd *	m_pWindow;
	SIZE			m_szRoundCorner;
	CDuiString		m_strStatusImage;
};

