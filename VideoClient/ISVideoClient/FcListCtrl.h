#pragma once

#include <vector>
using namespace std;

class CFcListCtrl : public CListCtrl
{
	DECLARE_DYNAMIC(CFcListCtrl)

public:
	CFcListCtrl(void);
	virtual ~CFcListCtrl(void);

	int GetItemHeight() const;
	void SetItemHeight(int iHeight);

	BOOL IsAlternateColor() const;
	void SetAlternateColor(BOOL bAlterColor);

	BOOL IsMarkSelectedItem() const;
	void SetMarkSelectedItem(BOOL bMarkSelItem);

	int GetBorderWidth() const;
	void SetBorderWidth(int iBorderWidth);

	COLORREF GetLightBackgroundColor() const;
	void SetLightBackgroundColor(COLORREF clrLightBg);

	COLORREF GetSelectedItemBackgroundColor() const;
	void SetSelectedItemBackgroundColor(COLORREF clrSelItemBg);

	COLORREF GetItemBorderColor() const;
	void SetItemBorderColor(COLORREF clrItemBorder);

	//! Used after adding column and before adding item
	void InitHScrollMode(BOOL bHScrollVisible, int iWidthFreeColumn);

	//! Overload
	virtual int InsertItem(const LVITEM* pItem);
	virtual int InsertItem(int nItem, LPCTSTR lpszItem);
	virtual int InsertItem(int nItem, LPCTSTR lpszItem, int nImage);
	virtual BOOL DeleteItem(int nItem);
	virtual BOOL DeleteAllItems();

protected:
	virtual void PreSubclassWindow();
	virtual void DrawItem(LPDRAWITEMSTRUCT /*lpDrawItemStruct*/);
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDblClk(UINT nFlags, CPoint point);
	afx_msg LRESULT OnSetExtendedStyle(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnGetExtendedStyle(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()

	//! Item height (Default: -1)
	int m_iItemHeight;

	//! Extend style
	DWORD m_dwExtendedStyle;

	//! Alternate color (Default: FALSE)
	BOOL m_bAlterColor;
	//! Mark the selected item (Default: TRUE)
	BOOL m_bMarkSelItem;

	//! Border width (Default: 4)
	int m_iBorderWidth;
	//! Horz scroll status
	BOOL m_bHScrollVisible;
	//! The width of column is free (Default: -1)
	int m_iWidthFreeColumn;
	BOOL m_bHasVertScrollBar;
	BOOL m_bIgnoreSizeChanged;

	//! Color (Default: RGB(247, 247, 247))
	COLORREF m_clrLightBg;
	//! (Default: RGB(210, 225, 255))
	COLORREF m_clrSelItemBg;
	//! (Default: RGB(196, 196, 196))
	COLORREF m_clrItemBorder;

	//! Font
	CFont m_fontDefault;

	void fnEqualRateColumn(int iTotalSize);
	void fnInsertItemAdjustColumnWidth();
};
