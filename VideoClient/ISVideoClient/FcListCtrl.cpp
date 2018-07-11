#include "stdafx.h"
#include "FcListCtrl.h"

IMPLEMENT_DYNAMIC(CFcListCtrl, CListCtrl)

CFcListCtrl::CFcListCtrl(void)
{
	m_iItemHeight = -1;
	m_dwExtendedStyle = 0;

	m_bAlterColor = FALSE;
	m_bMarkSelItem = TRUE;

	m_iBorderWidth = 4;
	m_bHScrollVisible = TRUE;
	m_iWidthFreeColumn = -1;
	m_bHasVertScrollBar = FALSE;
	m_bIgnoreSizeChanged = FALSE;

	m_clrLightBg = RGB(247, 247, 247);
	m_clrSelItemBg = RGB(210, 225, 255);
	m_clrItemBorder = RGB(196, 196, 196);
}

CFcListCtrl::~CFcListCtrl(void)
{
}

BEGIN_MESSAGE_MAP(CFcListCtrl, CListCtrl)
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	ON_WM_MEASUREITEM_REFLECT()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONDBLCLK()
	ON_MESSAGE(LVM_SETEXTENDEDLISTVIEWSTYLE, &CFcListCtrl::OnSetExtendedStyle)
	ON_MESSAGE(LVM_GETEXTENDEDLISTVIEWSTYLE, &CFcListCtrl::OnGetExtendedStyle)
END_MESSAGE_MAP()

void CFcListCtrl::PreSubclassWindow()
{
	LOGFONT lFont;
	GetParent()->GetFont()->GetLogFont(&lFont);
	lFont.lfHeight -= 4;
	m_fontDefault.CreateFontIndirect(&lFont);

	CListCtrl::PreSubclassWindow();
}

void CFcListCtrl::DrawItem(LPDRAWITEMSTRUCT lpDIS)
{
	if(lpDIS->itemID != (UINT)-1)
	{
		CDC *pDC = CDC::FromHandle(lpDIS->hDC);
		CRect rc(lpDIS->rcItem);
		CRect rcMemory(lpDIS->rcItem);
		rcMemory -= rcMemory.TopLeft();

		CDC dcMemory;
		dcMemory.CreateCompatibleDC(pDC);
		CBitmap bmpMemory;
		bmpMemory.CreateCompatibleBitmap(pDC, rc.Width(), rc.Height());
		CBitmap *pBmpMemory = dcMemory.SelectObject(&bmpMemory);
		dcMemory.SetBkMode(TRANSPARENT);

		CPen *pPen;
		CPen penBorder(PS_SOLID, 1, m_clrItemBorder);
		pPen = dcMemory.SelectObject(&penBorder);
		CFont *pFont = dcMemory.SelectObject(&m_fontDefault);

		// Draw background
		CRect rcBg(&rcMemory);
		if(TRUE == m_bAlterColor)
		{
			if(lpDIS->itemID % 2 == 1)
				dcMemory.FillSolidRect(&rcBg, m_clrLightBg);
			else
				dcMemory.FillSolidRect(&rcBg, m_clrSelItemBg);

			if((lpDIS->itemState & ODS_SELECTED) && TRUE == m_bMarkSelItem)
			{
				CRect rcSel(&rcBg);
				rcSel.DeflateRect(1, 1);
				CPen penSel(PS_SOLID, 2, RGB(0, 128, 0));
				CPen *pTempP = dcMemory.SelectObject(&penSel);
				CBrush *pTempB = (CBrush*)dcMemory.SelectStockObject(NULL_BRUSH);
				dcMemory.Rectangle(&rcSel);
				dcMemory.SelectObject(pTempP);
				dcMemory.SelectObject(pTempB);
			}
		}
		else
		{
			if((lpDIS->itemState & ODS_SELECTED) && TRUE == m_bMarkSelItem)
				dcMemory.FillSolidRect(&rcBg, m_clrSelItemBg);
			else
				dcMemory.FillSolidRect(&rcBg, m_clrLightBg);
		}
		dcMemory.MoveTo(rcBg.left, rcBg.bottom - 1);
		dcMemory.LineTo(rcBg.right, rcBg.bottom - 1);

		// Show icon and text
		CImageList *pImageListIcon = GetImageList(LVSIL_NORMAL);
		CRect rcCol(&rcMemory);
		rcCol.right = rcCol.left;
		CString szText;
		LV_COLUMN lvc;
		lvc.mask = LVCF_WIDTH | LVCF_FMT;
		for(int iCol = 0; GetColumn(iCol, &lvc); iCol++)
		{
			rcCol.left = rcCol.right;
			rcCol.right += lvc.cx;

			int iTextStart = 5;
			int iIconStart = 2;
			if(iCol == 0)
			{
				if((m_dwExtendedStyle & LVS_EX_CHECKBOXES) != 0)
				{
					int iHOffSet = (16 - rcCol.Height()) / 2;
					CRect rcState(rcCol.left + 2, rcCol.top - iHOffSet + 1, rcCol.left + 2 + 16, rcCol.bottom + iHOffSet);
					dcMemory.DrawFrameControl(&rcState, DFC_BUTTON, DFCS_BUTTONCHECK | (GetCheck(lpDIS->itemID) == FALSE ? 0 : DFCS_CHECKED));
					iTextStart += 2 + 16;
					iIconStart = 2 + 16 + 2;
				}

				if(NULL != pImageListIcon)
				{
					LVITEM lvItem;
					lvItem.mask = LVIF_IMAGE;
					lvItem.iItem = lpDIS->itemID;
					lvItem.iSubItem = 0;
					GetItem(&lvItem);
					if(lvItem.iImage >= 0)
					{
						IMAGEINFO imgInfo;
						pImageListIcon->GetImageInfo(lvItem.iImage, &imgInfo);
						int iHOffSet = (imgInfo.rcImage.bottom - imgInfo.rcImage.top - rcCol.Height()) / 2;
						CRect rcIcon(rcCol.left + iIconStart, rcCol.top - iHOffSet + 1, rcCol.left + iIconStart + imgInfo.rcImage.right - imgInfo.rcImage.left, rcCol.bottom + iHOffSet);
						pImageListIcon->Draw(&dcMemory, lvItem.iImage, rcIcon.TopLeft(), ILD_TRANSPARENT);
						iTextStart += 2 + rcIcon.Width();
					}
				}
			}

			if(iCol < GetHeaderCtrl()->GetItemCount() - 1)
			{
				dcMemory.MoveTo(rcCol.right - 1, rcCol.top);
				dcMemory.LineTo(rcCol.right - 1, rcCol.bottom);
			}

			// Get text and format
			CRect rcText(&rcCol);
			rcText.left += iTextStart;
			rcText.right -= 5;
			szText = GetItemText(lpDIS->itemID, iCol);
			UINT nFormat = DT_SINGLELINE | DT_VCENTER;
			switch(lvc.fmt & LVCFMT_JUSTIFYMASK)
			{
			case LVCFMT_LEFT:
				nFormat |= DT_LEFT;
				break;
			case LVCFMT_RIGHT:
				nFormat |= DT_RIGHT;
				break;
			case LVCFMT_CENTER:
				nFormat |= DT_CENTER;
				break;
			}

			// Draw text
			dcMemory.DrawText(szText, rcText, nFormat);
		}

		dcMemory.SelectObject(pPen);
		dcMemory.SelectObject(pFont);

		pDC->BitBlt(rc.left, rc.top, rc.Width(), rc.Height(), &dcMemory, 0, 0, SRCCOPY);
		dcMemory.SelectObject(pBmpMemory);
	}
}

BOOL CFcListCtrl::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	NMHEADER *pNMHeader = (NMHEADER*)lParam;
	if(FALSE == m_bHScrollVisible && (pNMHeader->hdr.code == HDN_BEGINTRACKW || pNMHeader->hdr.code == HDN_DIVIDERDBLCLICKW))
	{
		*pResult = 1;
		return TRUE;
	}
	return CListCtrl::OnNotify(wParam, lParam, pResult);
}

BOOL CFcListCtrl::OnEraseBkgnd(CDC* pDC)
{
	CRect rc, rcEnd;
	GetClientRect(&rc);
	int iCount = GetItemCount();
	if(iCount > 0)
		GetItemRect(iCount - 1, &rcEnd, LVIR_BOUNDS);
	else
		rcEnd.bottom = 0;

	if(rcEnd.bottom < rc.bottom)
	{
		CRect rcFill(&rc);
		rcFill.top = rcEnd.bottom;
		pDC->FillSolidRect(&rcFill, GetSysColor(COLOR_WINDOW));
	}

	int iTotalWidth = 0;
	LV_COLUMN lvc;
	lvc.mask = LVCF_WIDTH;
	for(int iCol = 0; GetColumn(iCol, &lvc); iCol++)
		iTotalWidth += lvc.cx;
	if(iTotalWidth < rc.right)
	{
		CRect rcFill(&rc);
		rcFill.left += iTotalWidth;
		rcFill.bottom = min(rcEnd.bottom, rc.bottom);
		pDC->FillSolidRect(&rcFill, GetSysColor(COLOR_WINDOW));
	}

	return TRUE;
}

void CFcListCtrl::MeasureItem(LPMEASUREITEMSTRUCT lpMIS)
{
	if(m_iItemHeight != -1)
		lpMIS->itemHeight = m_iItemHeight;
}

void CFcListCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	LVHITTESTINFO lvHitInfo;
	lvHitInfo.pt = point;
	SubItemHitTest(&lvHitInfo);

	if(lvHitInfo.iItem >= 0 && lvHitInfo.iSubItem >= 0)
	{
		if(lvHitInfo.iSubItem == 0 && (m_dwExtendedStyle & LVS_EX_CHECKBOXES) != 0)
		{
			CRect rcItem;
			GetItemRect(lvHitInfo.iItem, &rcItem, LVIR_BOUNDS);
			int iHOffSet = (16 - rcItem.Height()) / 2;
			CRect rcState(rcItem.left + 2, rcItem.top - iHOffSet + 1, rcItem.left + 2 + 16, rcItem.bottom + iHOffSet);
			if(TRUE == rcState.PtInRect(point))
				SetCheck(lvHitInfo.iItem, !GetCheck(lvHitInfo.iItem));
		}

		CListCtrl::OnLButtonDown(nFlags, point);
	}
}

void CFcListCtrl::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	LVHITTESTINFO lvHitInfo;
	lvHitInfo.pt = point;
	SubItemHitTest(&lvHitInfo);

	if(lvHitInfo.iItem >= 0 && lvHitInfo.iSubItem >= 0)
	{
		if((m_dwExtendedStyle & LVS_EX_CHECKBOXES) != 0)
		{
			CRect rcItem;
			GetItemRect(lvHitInfo.iItem, &rcItem, LVIR_BOUNDS);
			int iHOffSet = (16 - rcItem.Height()) / 2;
			CRect rcState(rcItem.left + 2, rcItem.top - iHOffSet + 1, rcItem.left + 2 + 16, rcItem.bottom + iHOffSet);
			if(TRUE == rcState.PtInRect(point))
				SetCheck(lvHitInfo.iItem, !GetCheck(lvHitInfo.iItem));
		}

		CListCtrl::OnLButtonDblClk(nFlags, point);
	}
}

void CFcListCtrl::OnRButtonDown(UINT nFlags, CPoint point)
{
	LVHITTESTINFO lvHitInfo;
	lvHitInfo.pt = point;
	SubItemHitTest(&lvHitInfo);

	if(lvHitInfo.iItem >= 0 && lvHitInfo.iSubItem >= 0)
		CListCtrl::OnRButtonDown(nFlags, point);
}

void CFcListCtrl::OnRButtonDblClk(UINT nFlags, CPoint point)
{
	LVHITTESTINFO lvHitInfo;
	lvHitInfo.pt = point;
	SubItemHitTest(&lvHitInfo);

	if(lvHitInfo.iItem >= 0 && lvHitInfo.iSubItem >= 0)
		CListCtrl::OnRButtonDblClk(nFlags, point);
}

LRESULT CFcListCtrl::OnGetExtendedStyle(WPARAM wParam, LPARAM lParam)
{
	return m_dwExtendedStyle;
}

LRESULT CFcListCtrl::OnSetExtendedStyle(WPARAM wParam, LPARAM lParam)
{
	LRESULT dwResult = m_dwExtendedStyle;
	if(!wParam) wParam = ~0U;
	m_dwExtendedStyle = static_cast<DWORD>(wParam & lParam);
	if(NULL != m_hWnd)
		Invalidate(TRUE);
	return dwResult;
}

void CFcListCtrl::InitHScrollMode(BOOL bHScrollVisible, int iWidthFreeColumn)
{
	m_bHScrollVisible = bHScrollVisible;
	m_iWidthFreeColumn = iWidthFreeColumn;
	if(FALSE == m_bHScrollVisible)
	{
		CRect rc;
		GetWindowRect(&rc);
		m_bIgnoreSizeChanged = TRUE;
		fnEqualRateColumn(rc.Width() - m_iBorderWidth);
		m_bIgnoreSizeChanged = FALSE;
	}
}

int CFcListCtrl::InsertItem(const LVITEM* pItem)
{
	if(FALSE == m_bHScrollVisible)
		fnInsertItemAdjustColumnWidth();
	int iIndex = CListCtrl::InsertItem(pItem);
	if(iIndex != -1)
		SetCheck(iIndex, FALSE);
	return iIndex;
}

int CFcListCtrl::InsertItem(int nItem, LPCTSTR lpszItem)
{
	if(FALSE == m_bHScrollVisible)
		fnInsertItemAdjustColumnWidth();
	int iIndex = CListCtrl::InsertItem(nItem, lpszItem);
	if(iIndex != -1)
		SetCheck(iIndex, FALSE);
	return iIndex;
}

int CFcListCtrl::InsertItem(int nItem, LPCTSTR lpszItem, int nImage)
{
	if(FALSE == m_bHScrollVisible)
		fnInsertItemAdjustColumnWidth();
	int iIndex = CListCtrl::InsertItem(nItem, lpszItem, nImage);
	if(iIndex != -1)
		SetCheck(iIndex, FALSE);
	return iIndex;
}

void CFcListCtrl::fnInsertItemAdjustColumnWidth()
{
	int iPageCount = GetCountPerPage();
	int iCurCount = GetItemCount();

	if(iCurCount + 1 > iPageCount && FALSE == m_bHasVertScrollBar)
	{
		m_bHasVertScrollBar = TRUE;

		int iColumnNum = GetHeaderCtrl()->GetItemCount();
		m_bIgnoreSizeChanged = TRUE;
		if(m_iWidthFreeColumn >= 0 && m_iWidthFreeColumn < iColumnNum)
			SetColumnWidth(m_iWidthFreeColumn, GetColumnWidth(m_iWidthFreeColumn) - GetSystemMetrics(SM_CXVSCROLL));
		else
		{
			CRect rc;
			GetWindowRect(&rc);
			fnEqualRateColumn(rc.Width() - m_iBorderWidth - GetSystemMetrics(SM_CXVSCROLL));
		}
		m_bIgnoreSizeChanged = FALSE;
	}
}

void CFcListCtrl::fnEqualRateColumn(int iTotalSize)
{
	int iTotalWidth = 0;
	int iColumnNum = GetHeaderCtrl()->GetItemCount();
	if(iColumnNum > 0)
	{
		for(int i = 0; i < iColumnNum; i++)
			iTotalWidth += GetColumnWidth(i);
		if(iTotalWidth > 0)
		{
			int iRemainSize = iTotalSize;
			for(int i = 0; i < iColumnNum - 1; i++)
			{
				int iNewSize = iTotalSize * GetColumnWidth(i) / iTotalWidth;
				SetColumnWidth(i, iNewSize);
				iRemainSize -= iNewSize;
			}
			SetColumnWidth(iColumnNum - 1, iRemainSize);
		}
	}
}

BOOL CFcListCtrl::DeleteItem(int nItem)
{
	m_bIgnoreSizeChanged = TRUE;
	BOOL bRet = CListCtrl::DeleteItem(nItem);
	if(FALSE == m_bHScrollVisible && TRUE == bRet)
	{
		int iPageCount = GetCountPerPage();
		int iCurCount = GetItemCount();
		if(iCurCount <= iPageCount && TRUE == m_bHasVertScrollBar)
		{
			m_bHasVertScrollBar = FALSE;

			int iColumnNum = GetHeaderCtrl()->GetItemCount();
			if(m_iWidthFreeColumn >= 0 && m_iWidthFreeColumn < iColumnNum)
				SetColumnWidth(m_iWidthFreeColumn, GetColumnWidth(m_iWidthFreeColumn) + GetSystemMetrics(SM_CXVSCROLL));
			else
			{
				CRect rc;
				GetWindowRect(&rc);
				fnEqualRateColumn(rc.Width() - m_iBorderWidth);
			}
		}
	}
	m_bIgnoreSizeChanged = FALSE;
	return bRet;
}

BOOL CFcListCtrl::DeleteAllItems()
{
	m_bIgnoreSizeChanged = TRUE;
	BOOL bRet = CListCtrl::DeleteAllItems();
	if(FALSE == m_bHScrollVisible && TRUE == m_bHasVertScrollBar)
	{
		m_bHasVertScrollBar = FALSE;

		int iColumnNum = GetHeaderCtrl()->GetItemCount();
		if(m_iWidthFreeColumn >= 0 && m_iWidthFreeColumn < iColumnNum)
			SetColumnWidth(m_iWidthFreeColumn, GetColumnWidth(m_iWidthFreeColumn) + GetSystemMetrics(SM_CXVSCROLL));
		else
		{
			CRect rc;
			GetWindowRect(&rc);
			fnEqualRateColumn(rc.Width() - m_iBorderWidth);
		}
	}
	m_bIgnoreSizeChanged = FALSE;
	return bRet;
}

void CFcListCtrl::OnSize(UINT nType, int cx, int cy)
{
	CListCtrl::OnSize(nType, cx, cy);

	int iColumnNum = GetHeaderCtrl()->GetItemCount();
	if(FALSE == m_bHScrollVisible && iColumnNum > 0 && FALSE == m_bIgnoreSizeChanged)
	{
		m_bIgnoreSizeChanged = TRUE;

		// Adjust width
		int iActualWidth = 0;
		for(int i = 0; i < iColumnNum; i++)
			iActualWidth += GetColumnWidth(i);
		if(TRUE == m_bHasVertScrollBar)
			iActualWidth += GetSystemMetrics(SM_CXVSCROLL);

		CRect rc;
		GetWindowRect(&rc);
		if(m_iWidthFreeColumn >= 0 && m_iWidthFreeColumn < iColumnNum)
			SetColumnWidth(m_iWidthFreeColumn, GetColumnWidth(m_iWidthFreeColumn) + rc.Width() - m_iBorderWidth - iActualWidth);
		else
			fnEqualRateColumn(rc.Width() - m_iBorderWidth - (TRUE == m_bHasVertScrollBar ? GetSystemMetrics(SM_CXVSCROLL) : 0));

		// Adjust height
		int iPageCount = GetCountPerPage();
		int iCurCount = GetItemCount();
		if(iCurCount > iPageCount && FALSE == m_bHasVertScrollBar)
		{
			m_bHasVertScrollBar = TRUE;

			if(m_iWidthFreeColumn >= 0 && m_iWidthFreeColumn < iColumnNum)
				SetColumnWidth(m_iWidthFreeColumn, GetColumnWidth(m_iWidthFreeColumn) - GetSystemMetrics(SM_CXVSCROLL));
			else
				fnEqualRateColumn(rc.Width() - m_iBorderWidth);
		}
		else if(iCurCount <= iPageCount && TRUE == m_bHasVertScrollBar)
		{
			m_bHasVertScrollBar = FALSE;

			if(m_iWidthFreeColumn >= 0 && m_iWidthFreeColumn < iColumnNum)
				SetColumnWidth(m_iWidthFreeColumn, GetColumnWidth(m_iWidthFreeColumn) + GetSystemMetrics(SM_CXVSCROLL));
			else
				fnEqualRateColumn(rc.Width() - m_iBorderWidth);
		}
		ShowScrollBar(SB_HORZ, FALSE);

		m_bIgnoreSizeChanged = FALSE;
	}
}

int CFcListCtrl::GetItemHeight() const
{
	return m_iItemHeight;
}

void CFcListCtrl::SetItemHeight(int iHeight)
{
	m_iItemHeight = iHeight;

	CRect rc;
	GetWindowRect(&rc);
	// Cheat the window to respond the "MeasureItem"
	WINDOWPOS wp;
	wp.hwnd = m_hWnd;
	wp.cx = rc.Width();
	wp.cy = rc.Height();
	wp.flags = SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOOWNERZORDER | SWP_NOZORDER;

	SendMessage(WM_WINDOWPOSCHANGED, 0,(LPARAM)&wp);
}

BOOL CFcListCtrl::IsAlternateColor() const
{
	return m_bAlterColor;
}

void CFcListCtrl::SetAlternateColor(BOOL bAlterColor)
{
	if(m_bAlterColor != bAlterColor)
	{
		m_bAlterColor = bAlterColor;
		if(NULL != m_hWnd)
			Invalidate(TRUE);
	}
}

BOOL CFcListCtrl::IsMarkSelectedItem() const
{
	return m_bMarkSelItem;
}

void CFcListCtrl::SetMarkSelectedItem(BOOL bMarkSelItem)
{
	if(m_bMarkSelItem != bMarkSelItem)
	{
		m_bMarkSelItem = bMarkSelItem;
		if(NULL != m_hWnd)
			Invalidate(TRUE);
	}
}

int CFcListCtrl::GetBorderWidth() const
{
	return m_iBorderWidth;
}

void CFcListCtrl::SetBorderWidth(int iBorderWidth)
{
	if(m_iBorderWidth != iBorderWidth)
	{
		m_iBorderWidth = iBorderWidth;
		if(FALSE == m_bHScrollVisible)
		{
			CRect rc;
			GetWindowRect(&rc);
			m_bIgnoreSizeChanged = TRUE;
			fnEqualRateColumn(rc.Width() - m_iBorderWidth);
			m_bIgnoreSizeChanged = FALSE;
		}
	}
}

COLORREF CFcListCtrl::GetLightBackgroundColor() const
{
	return m_clrLightBg;
}

void CFcListCtrl::SetLightBackgroundColor(COLORREF clrLightBg)
{
	if(m_clrLightBg != clrLightBg)
	{
		m_clrLightBg = clrLightBg;
		if(NULL != m_hWnd)
			Invalidate(TRUE);
	}
}

COLORREF CFcListCtrl::GetSelectedItemBackgroundColor() const
{
	return m_clrSelItemBg;
}

void CFcListCtrl::SetSelectedItemBackgroundColor(COLORREF clrSelItemBg)
{
	if(m_clrSelItemBg != clrSelItemBg)
	{
		m_clrSelItemBg = clrSelItemBg;
		if(NULL != m_hWnd)
			Invalidate(TRUE);
	}
}

COLORREF CFcListCtrl::GetItemBorderColor() const
{
	return m_clrItemBorder;
}

void CFcListCtrl::SetItemBorderColor(COLORREF clrItemBorder)
{
	if(m_clrItemBorder != clrItemBorder)
	{
		m_clrItemBorder = clrItemBorder;
		if(NULL != m_hWnd)
			Invalidate(TRUE);
	}
}
