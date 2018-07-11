#pragma once


#include "FcListCtrl.h"

#define IDC_EDIT_FTC_LISTCTRLEX_VALUE		9384
#define IDC_COMBO_FTC_LISTCTRLEX_VALUE		9385

#define UM_FTC_LISTCTRLEX_LINKHIT			(WM_USER + 350)
#define UM_FTC_LISTCTRLEX_ITEMEDIT			(WM_USER + 351)

enum
{
	FTC_LISTCTRLEX_COLUMNTYPE_NORMAL		= 0,
	FTC_LISTCTRLEX_COLUMNTYPE_TITLE			= 1,
	FTC_LISTCTRLEX_COLUMNTYPE_EDIT			= 2,
	FTC_LISTCTRLEX_COLUMNTYPE_COMBBOX		= 3,
	FTC_LISTCTRLEX_COLUMNTYPE_READCHECK		= 4,
	FTC_LISTCTRLEX_COLUMNTYPE_WRITECHECK	= 5,
	FTC_LISTCTRLEX_COLUMNTYPE_LINK			= 6
};

class CFcListCtrlEx : public CFcListCtrl
{
	DECLARE_DYNAMIC(CFcListCtrlEx)

public:
	CFcListCtrlEx(void);		//! "ItemData" is occupied
	virtual ~CFcListCtrlEx(void);

	const vector<vector<CString>>& GetComboBoxDesc() const;
	void SetComboBoxDesc(const vector<vector<CString>>& fvArrayComboxDesc);

	void GetColumnType(int iColumn, int& iColumnType, int& iSubColumnType) const;
	void SetColumnType(int iColumn, int iColumnType, int iSubColumnType = 0);

	COLORREF GetDarkBackgroundColor() const;
	void SetDarkBackgroundColor(COLORREF clrDarkBg);

	//! Overload
	virtual int InsertColumn(int nCol, const LVCOLUMN* pColumn);
	virtual int InsertColumn(int nCol, LPCTSTR lpszColumnHeading, int nFormat = LVCFMT_LEFT, int nWidth = -1, int nSubItem = -1);
	virtual BOOL DeleteColumn(int nCol);

protected:
	virtual void DrawItem(LPDRAWITEMSTRUCT /*lpDrawItemStruct*/);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnChangeEditFtcListctrlexValue();
	afx_msg void OnKillfocusEditFtcListctrlexValue();
	afx_msg void OnCbnSelchangeComboFtcListctrlexValue();
	afx_msg void OnKillfocusComboFtcListctrlexValue();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);

	DECLARE_MESSAGE_MAP()

private:
	//! Title color (Default: RGB(28, 55, 122))
	COLORREF m_clrDarkBg;
	//! Combo description
	vector<vector<CString>> m_fvArrayCombBoxDesc;
	//! Column type
	vector<UINT> m_fvArrayColumnType;

	//! Float control
	CEdit m_edtValue;
	CComboBox m_cbxValue;
	BOOL m_bIgnoreChanged;
	int m_iItemSel;
	int m_iSubItemSel;
	//! Cursor
	HCURSOR m_hCursorHand;
	HCURSOR m_hCursorNormal;
	//! Link hit flag
	BOOL m_bHitLink;
};
