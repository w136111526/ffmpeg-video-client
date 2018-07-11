#include "StdAfx.h"
#include "FcListCtrlEx.h"

IMPLEMENT_DYNAMIC(CFcListCtrlEx, CFcListCtrl)

CFcListCtrlEx::CFcListCtrlEx(void)
{
	m_clrDarkBg = RGB(28, 55, 122);

	m_bIgnoreChanged = FALSE;
	m_iItemSel = -1;
	m_iSubItemSel = -1;

	m_hCursorHand = AfxGetApp()->LoadStandardCursor(IDC_HAND);
	m_hCursorNormal = NULL;
	m_bHitLink = FALSE;
}

CFcListCtrlEx::~CFcListCtrlEx(void)
{
}

BEGIN_MESSAGE_MAP(CFcListCtrlEx, CFcListCtrl)
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	ON_EN_CHANGE(IDC_EDIT_FTC_LISTCTRLEX_VALUE, &CFcListCtrlEx::OnChangeEditFtcListctrlexValue)
	ON_EN_KILLFOCUS(IDC_EDIT_FTC_LISTCTRLEX_VALUE, &CFcListCtrlEx::OnKillfocusEditFtcListctrlexValue)
	ON_CBN_SELCHANGE(IDC_COMBO_FTC_LISTCTRLEX_VALUE, &CFcListCtrlEx::OnCbnSelchangeComboFtcListctrlexValue)
	ON_CBN_KILLFOCUS(IDC_COMBO_FTC_LISTCTRLEX_VALUE, &CFcListCtrlEx::OnKillfocusComboFtcListctrlexValue)
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	ON_WM_MOUSEWHEEL()
END_MESSAGE_MAP()

void CFcListCtrlEx::DrawItem(LPDRAWITEMSTRUCT lpDIS)
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
		if((lpDIS->itemState & ODS_SELECTED) && TRUE == m_bMarkSelItem)
			dcMemory.FillSolidRect(&rcBg, m_clrSelItemBg);
		else
			dcMemory.FillSolidRect(&rcBg, m_clrLightBg);
		dcMemory.MoveTo(rcBg.left, rcBg.bottom - 1);
		dcMemory.LineTo(rcBg.right, rcBg.bottom - 1);

		// Show icon and text
		UINT nItemData = (UINT)GetItemData(lpDIS->itemID);
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

			int iColumnType = LOWORD(m_fvArrayColumnType[iCol]);
			int iSubColumnType = HIWORD(m_fvArrayColumnType[iCol]);
			if(iColumnType == FTC_LISTCTRLEX_COLUMNTYPE_TITLE)
			{
				CRect rcTitle(&rcCol);
				rcTitle.bottom--;
				dcMemory.FillSolidRect(&rcTitle, m_clrDarkBg);
			}

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

			CRect rcText(&rcCol);
			rcText.left += iTextStart;
			rcText.right -= 5;
			szText = GetItemText(lpDIS->itemID, iCol);
			if(iColumnType == FTC_LISTCTRLEX_COLUMNTYPE_READCHECK || iColumnType == FTC_LISTCTRLEX_COLUMNTYPE_WRITECHECK)
			{
				// Draw compact
				CPoint rcCenter = rcText.CenterPoint();
				CSize size(10, 10);
				CRect rcCompact(CPoint(rcCenter.x - size.cx / 2, rcCenter.y - size.cy / 2), size);
				if((nItemData & 0x80000000) != 0)
					dcMemory.Draw3dRect(&rcCompact, RGB(128, 128, 128), RGB(128, 128, 128));
				else
					dcMemory.Draw3dRect(&rcCompact, RGB(0, 0, 0), RGB(0, 0, 0));

				BOOL bCheck = (_ttoi(szText) != 0);
				if(TRUE == bCheck)
				{
					// Draw selection
					CPen penMark(PS_SOLID, 2, (nItemData & 0x80000000) != 0 ? RGB(128, 128, 128) : RGB(0, 128, 0));
					CPen *pPenMark = dcMemory.SelectObject(&penMark);
					dcMemory.MoveTo(rcCompact.left - 3, rcCompact.top + 2);
					dcMemory.LineTo(rcCompact.left + 2, rcCompact.bottom - 2);
					dcMemory.LineTo(rcCompact.right + 3, rcCompact.top - 3);
					dcMemory.SelectObject(pPenMark);
				}
			}
			else
			{
				// Get text and format
				if(iColumnType == FTC_LISTCTRLEX_COLUMNTYPE_COMBBOX)
				{
					int iIndex = _ttoi(szText);
					if(iSubColumnType >= 0 && iSubColumnType < (int)m_fvArrayCombBoxDesc.size() &&
						iIndex >= 0 && iIndex < (int)m_fvArrayCombBoxDesc[iSubColumnType].size())
						szText = m_fvArrayCombBoxDesc[iSubColumnType][iIndex];
					else
						szText = _T("");
				}
				UINT nFormat = DT_SINGLELINE | DT_VCENTER | DT_CENTER;
// 				switch(lvc.fmt & LVCFMT_JUSTIFYMASK)
// 				{
// 				case LVCFMT_LEFT:
// 					nFormat |= DT_LEFT;
// 					break;
// 				case LVCFMT_RIGHT:
// 					nFormat |= DT_RIGHT;
// 					break;
// 				case LVCFMT_CENTER:
// 					nFormat |= DT_CENTER;
// 					break;
// 				}

				// Draw text
				COLORREF clrTextColor = GetSysColor(COLOR_WINDOWTEXT);
				if(iColumnType == FTC_LISTCTRLEX_COLUMNTYPE_TITLE)
					clrTextColor = RGB(255, 255, 255);
				else if((nItemData & 0x80000000) != 0)
					clrTextColor = RGB(128, 128, 128);
				dcMemory.SetTextColor(clrTextColor);
				if(iColumnType == FTC_LISTCTRLEX_COLUMNTYPE_LINK)
				{
					LOGFONT logFont;
					m_fontDefault.GetLogFont(&logFont);
					logFont.lfUnderline = TRUE;
					CFont fontLink;
					fontLink.CreateFontIndirect(&logFont);
					CFont *pFontLink = dcMemory.SelectObject(&fontLink);
					COLORREF clrLink = dcMemory.SetTextColor(RGB(0, 0, 255));
					dcMemory.DrawText(szText, rcText, nFormat);
					dcMemory.SetTextColor(clrLink);
					dcMemory.SelectObject(pFontLink);
				}
				else
					dcMemory.DrawText(szText, rcText, nFormat);
			}
		}

		dcMemory.SelectObject(pPen);
		dcMemory.SelectObject(pFont);

		pDC->BitBlt(rc.left, rc.top, rc.Width(), rc.Height(), &dcMemory, 0, 0, SRCCOPY);
		dcMemory.SelectObject(pBmpMemory);
	}
}

void CFcListCtrlEx::OnMouseMove(UINT nFlags, CPoint point)
{
	LVHITTESTINFO lvHitInfo;
	lvHitInfo.pt = point;
	SubItemHitTest(&lvHitInfo);

	m_bHitLink = FALSE;
	if(lvHitInfo.iItem >= 0 && lvHitInfo.iSubItem >= 0 && FTC_LISTCTRLEX_COLUMNTYPE_LINK == LOWORD(m_fvArrayColumnType[lvHitInfo.iSubItem]))
	{
		CString szText = GetItemText(lvHitInfo.iItem, lvHitInfo.iSubItem);
		if(FALSE == szText.IsEmpty())
		{
			CRect rcText;
			GetSubItemRect(lvHitInfo.iItem, lvHitInfo.iSubItem, LVIR_BOUNDS, rcText);
			CDC *pDC = GetDC();
			CFont *pFont = pDC->SelectObject(&m_fontDefault);
			CSize size = pDC->GetTextExtent(szText);
			pDC->SelectObject(pFont);
			ReleaseDC(pDC);
			rcText.top += (rcText.Height() - size.cy) / 2;
			rcText.bottom = rcText.top + size.cy;

			int iTextStart = 5;
			if(lvHitInfo.iSubItem == 0)
			{
				if((m_dwExtendedStyle & LVS_EX_CHECKBOXES) != 0)
					iTextStart += 2 + 16;
				CImageList *pImageListIcon = GetImageList(LVSIL_NORMAL);
				if(NULL != pImageListIcon)
				{
					LVITEM lvItem;
					lvItem.mask = LVIF_IMAGE;
					lvItem.iItem = lvHitInfo.iItem;
					lvItem.iSubItem = 0;
					GetItem(&lvItem);
					if(lvItem.iImage >= 0)
					{
						IMAGEINFO imgInfo;
						pImageListIcon->GetImageInfo(lvItem.iImage, &imgInfo);
						iTextStart += 2 + imgInfo.rcImage.right - imgInfo.rcImage.left;
					}
				}
			}
			LV_COLUMN lvc;
			lvc.mask = LVCF_FMT;
			GetColumn(lvHitInfo.iSubItem, &lvc);
			switch(lvc.fmt & LVCFMT_JUSTIFYMASK)
			{
			case LVCFMT_LEFT:
				rcText.left += iTextStart;
				rcText.right = rcText.left + size.cx;
				break;
			case LVCFMT_RIGHT:
				rcText.right -= 5;
				rcText.left = rcText.right - size.cx;
				break;
			case LVCFMT_CENTER:
				rcText.left += (rcText.Width() - iTextStart + 5 - size.cx) / 2;
				rcText.right = rcText.left + size.cx;
				break;
			}

			if(TRUE == rcText.PtInRect(point))
				m_bHitLink = TRUE;
		}
	}
	if(TRUE == m_bHitLink)
	{
		if(m_hCursorHand != GetCursor())
			m_hCursorNormal = SetCursor(m_hCursorHand);
	}
	else
	{
		if(m_hCursorHand == GetCursor())
			SetCursor(m_hCursorNormal);
	}

	CFcListCtrl::OnMouseMove(nFlags, point);
}

void CFcListCtrlEx::OnLButtonDown(UINT nFlags, CPoint point)
{
	LVHITTESTINFO lvHitInfo;
	lvHitInfo.pt = point;
	SubItemHitTest(&lvHitInfo);

	if(lvHitInfo.iItem >= 0 && lvHitInfo.iSubItem >= 0)
	{
		// !!! Important for avoid horizontal scroll movement automatically !!!
		POSITION pos = GetFirstSelectedItemPosition();
		while(NULL != pos)
		{
			int iItem = GetNextSelectedItem(pos);
			if(iItem == lvHitInfo.iItem)
			{
				SetItemState(iItem, 0, LVIS_SELECTED);
				break;
			}
		}

		CFcListCtrl::OnLButtonDown(nFlags, point);

		int iColumnType = LOWORD(m_fvArrayColumnType[lvHitInfo.iSubItem]);
		int iSubColumnType = HIWORD(m_fvArrayColumnType[lvHitInfo.iSubItem]);
		UINT nItemData = (UINT)GetItemData(lvHitInfo.iItem);
		if((iColumnType == FTC_LISTCTRLEX_COLUMNTYPE_EDIT || iColumnType == FTC_LISTCTRLEX_COLUMNTYPE_COMBBOX || iColumnType == FTC_LISTCTRLEX_COLUMNTYPE_WRITECHECK) && (nItemData & 0x80000000) == 0)
		{
			if(iColumnType == FTC_LISTCTRLEX_COLUMNTYPE_EDIT || iColumnType == FTC_LISTCTRLEX_COLUMNTYPE_COMBBOX)
				EnsureVisible(lvHitInfo.iItem, TRUE);

			m_iItemSel = lvHitInfo.iItem;
			m_iSubItemSel = lvHitInfo.iSubItem;
			CRect rc;
			GetSubItemRect(m_iItemSel, m_iSubItemSel, LVIR_BOUNDS, rc);
			CRect rcClient;
			GetClientRect(&rcClient);
			rc &= rcClient;

			if(iColumnType == FTC_LISTCTRLEX_COLUMNTYPE_EDIT)
			{
				DWORD dwTextAlign = ES_LEFT;
				LV_COLUMN lvc;
				lvc.mask = LVCF_FMT;
				GetColumn(m_iSubItemSel, &lvc);
				switch(lvc.fmt & LVCFMT_JUSTIFYMASK)
				{
				case LVCFMT_LEFT:
					dwTextAlign = ES_LEFT;
					break;
				case LVCFMT_RIGHT:
					dwTextAlign = ES_RIGHT;
					break;
				case LVCFMT_CENTER:
					dwTextAlign = ES_CENTER;
					break;
				}

				if(NULL == m_edtValue.m_hWnd)
				{
					m_edtValue.Create(dwTextAlign | ES_AUTOHSCROLL | WS_CHILD | WS_BORDER, rc, this, IDC_EDIT_FTC_LISTCTRLEX_VALUE);
					m_edtValue.SetFont(&m_fontDefault);
				}
				else
				{
					long dwStyle =::GetWindowLong(m_edtValue.m_hWnd, GWL_STYLE);
					dwStyle &= ~(ES_CENTER | ES_RIGHT);
					dwStyle |= dwTextAlign;
					::SetWindowLong(m_edtValue.m_hWnd, GWL_STYLE, dwStyle);
					m_edtValue.MoveWindow(&rc);
				}

				m_bIgnoreChanged = TRUE;
				m_edtValue.SetWindowText(GetItemText(m_iItemSel, m_iSubItemSel));
				m_edtValue.SetSel(0, -1, FALSE);
				m_bIgnoreChanged = FALSE;
				m_edtValue.ShowWindow(SW_SHOW);
				m_edtValue.SetFocus();
			}
			else if(iColumnType == FTC_LISTCTRLEX_COLUMNTYPE_COMBBOX)
			{
				if(NULL == m_cbxValue.m_hWnd)
				{
					m_cbxValue.Create(CBS_DROPDOWNLIST | CBS_OWNERDRAWFIXED | CBS_HASSTRINGS | WS_CHILD | WS_BORDER, rc, this, IDC_COMBO_FTC_LISTCTRLEX_VALUE);
					m_cbxValue.SetFont(&m_fontDefault);
				}
				else
					m_cbxValue.MoveWindow(&rc);

				m_bIgnoreChanged = TRUE;
				m_cbxValue.ResetContent();
				if(iSubColumnType >= 0 && iSubColumnType < (int)m_fvArrayCombBoxDesc.size())
				{
					for(int i = 0; i < (int)m_fvArrayCombBoxDesc[iSubColumnType].size(); i++)
						m_cbxValue.AddString(m_fvArrayCombBoxDesc[iSubColumnType][i]);
				}
				m_cbxValue.SetCurSel(_ttoi(GetItemText(m_iItemSel, m_iSubItemSel)));
				m_bIgnoreChanged = FALSE;
				m_cbxValue.ShowWindow(SW_SHOW);
				m_cbxValue.SetFocus();
			}
			else if(iColumnType == FTC_LISTCTRLEX_COLUMNTYPE_WRITECHECK)
			{
				BOOL bCheck = (_ttoi(GetItemText(m_iItemSel, m_iSubItemSel)) != 0);
				if(TRUE == bCheck)
					SetItemText(m_iItemSel, m_iSubItemSel, _T("0"));
				else
					SetItemText(m_iItemSel, m_iSubItemSel, _T("1"));
			}
		}
		else if(iColumnType == FTC_LISTCTRLEX_COLUMNTYPE_LINK)
		{
			if(TRUE == m_bHitLink)
				GetParent()->PostMessage(UM_FTC_LISTCTRLEX_LINKHIT, MAKEWPARAM(lvHitInfo.iItem, lvHitInfo.iSubItem), (LPARAM)this);
		}
	}
	else
	{
		if(NULL != m_edtValue.m_hWnd)
			m_edtValue.ShowWindow(SW_HIDE);
		if(NULL != m_cbxValue.m_hWnd)
			m_cbxValue.ShowWindow(SW_HIDE);
	}
}

void CFcListCtrlEx::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	LVHITTESTINFO lvHitInfo;
	lvHitInfo.pt = point;
	SubItemHitTest(&lvHitInfo);

	if(lvHitInfo.iItem >= 0 && lvHitInfo.iSubItem >= 0)
	{
		CFcListCtrl::OnLButtonDblClk(nFlags, point);

		int iColumnType = LOWORD(m_fvArrayColumnType[lvHitInfo.iSubItem]);
		int iSubColumnType = HIWORD(m_fvArrayColumnType[lvHitInfo.iSubItem]);
		UINT nItemData = (UINT)GetItemData(lvHitInfo.iItem);
		if(iColumnType == FTC_LISTCTRLEX_COLUMNTYPE_WRITECHECK && (nItemData & 0x80000000) == 0)
		{
			m_iItemSel = lvHitInfo.iItem;
			m_iSubItemSel = lvHitInfo.iSubItem;

			BOOL bCheck = (_ttoi(GetItemText(m_iItemSel, m_iSubItemSel)) != 0);
			if(TRUE == bCheck)
				SetItemText(m_iItemSel, m_iSubItemSel, _T("0"));
			else
				SetItemText(m_iItemSel, m_iSubItemSel, _T("1"));
		}
		else if(iColumnType == FTC_LISTCTRLEX_COLUMNTYPE_LINK)
		{
			if(TRUE == m_bHitLink)
				GetParent()->PostMessage(UM_FTC_LISTCTRLEX_LINKHIT, MAKEWPARAM(lvHitInfo.iItem, lvHitInfo.iSubItem), (LPARAM)this);
		}
	}
}

void CFcListCtrlEx::OnChangeEditFtcListctrlexValue()
{
	if(FALSE == m_bIgnoreChanged)
	{
		CString szValue;
		m_edtValue.GetWindowText(szValue);

		if(GetItemText(m_iItemSel, m_iSubItemSel) != szValue)
		{
			SetItemText(m_iItemSel, m_iSubItemSel, szValue);
			GetParent()->PostMessage(UM_FTC_LISTCTRLEX_ITEMEDIT, MAKEWPARAM(m_iItemSel, m_iSubItemSel), (LPARAM)this);
		}
	}
}

void CFcListCtrlEx::OnKillfocusEditFtcListctrlexValue()
{
	if(NULL != m_edtValue.m_hWnd)
		m_edtValue.ShowWindow(SW_HIDE);
}

void CFcListCtrlEx::OnCbnSelchangeComboFtcListctrlexValue()
{
	if(FALSE == m_bIgnoreChanged)
	{
		int iCurSel = m_cbxValue.GetCurSel();
		CString szValue;
		szValue.Format(_T("%d"), iCurSel);

		if(GetItemText(m_iItemSel, m_iSubItemSel) != szValue)
		{
			SetItemText(m_iItemSel, m_iSubItemSel, szValue);
			GetParent()->PostMessage(UM_FTC_LISTCTRLEX_ITEMEDIT, MAKEWPARAM(m_iItemSel, m_iSubItemSel), (LPARAM)this);
		}
	}
}

void CFcListCtrlEx::OnKillfocusComboFtcListctrlexValue()
{
	if(NULL != m_cbxValue.m_hWnd)
		m_cbxValue.ShowWindow(SW_HIDE);
}

void CFcListCtrlEx::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	SetFocus();
	CFcListCtrl::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CFcListCtrlEx::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	SetFocus();
	CFcListCtrl::OnVScroll(nSBCode, nPos, pScrollBar);
}

BOOL CFcListCtrlEx::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	SetFocus();
	return CFcListCtrl::OnMouseWheel(nFlags, zDelta, pt);
}

int CFcListCtrlEx::InsertColumn(int nCol, const LVCOLUMN* pColumn)
{
	if(nCol < 0) nCol = 0;
	if(nCol < (int)m_fvArrayColumnType.size())
		m_fvArrayColumnType.insert(m_fvArrayColumnType.begin() + nCol, 0);
	else
		m_fvArrayColumnType.push_back(0);
	return CFcListCtrl::InsertColumn(nCol, pColumn);
}

int CFcListCtrlEx::InsertColumn(int nCol, LPCTSTR lpszColumnHeading, int nFormat, int nWidth, int nSubItem)
{
	if(nCol < 0) nCol = 0;
	if(nCol < (int)m_fvArrayColumnType.size())
		m_fvArrayColumnType.insert(m_fvArrayColumnType.begin() + nCol, 0);
	else
		m_fvArrayColumnType.push_back(0);
	return CFcListCtrl::InsertColumn(nCol, lpszColumnHeading, nFormat, nWidth, nSubItem);
}

BOOL CFcListCtrlEx::DeleteColumn(int nCol)
{
	if(nCol >= 0 && nCol < (int)m_fvArrayColumnType.size())
		m_fvArrayColumnType.erase(m_fvArrayColumnType.begin() + nCol);
	return CFcListCtrl::DeleteColumn(nCol);
}

const vector<vector<CString>>& CFcListCtrlEx::GetComboBoxDesc() const
{
	return m_fvArrayCombBoxDesc;
}

void CFcListCtrlEx::SetComboBoxDesc(const vector<vector<CString>>& fvArrayComboxDesc)
{
	m_fvArrayCombBoxDesc = fvArrayComboxDesc;

	// Refresh
	if(NULL != m_edtValue.m_hWnd)
		m_edtValue.ShowWindow(SW_HIDE);
	if(NULL != m_cbxValue.m_hWnd)
		m_cbxValue.ShowWindow(SW_HIDE);
	if(NULL != m_hWnd)
		Invalidate(TRUE);
}

void CFcListCtrlEx::GetColumnType(int iColumn, int& iColumnType, int& iSubColumnType) const
{
	if(iColumn >= 0 && iColumn < (int)m_fvArrayColumnType.size())
	{
		iColumnType = LOWORD(m_fvArrayColumnType[iColumn]);
		iSubColumnType = HIWORD(m_fvArrayColumnType[iColumn]);
	}
}

void CFcListCtrlEx::SetColumnType(int iColumn, int iColumnType, int iSubColumnType)
{
	if(iColumn >= 0 && iColumn < (int)m_fvArrayColumnType.size())
	{
		m_fvArrayColumnType[iColumn] = MAKELONG(iColumnType, iSubColumnType);

		// Refresh
		if(NULL != m_edtValue.m_hWnd)
			m_edtValue.ShowWindow(SW_HIDE);
		if(NULL != m_cbxValue.m_hWnd)
			m_cbxValue.ShowWindow(SW_HIDE);
		if(NULL != m_hWnd)
			Invalidate(TRUE);
	}
}

COLORREF CFcListCtrlEx::GetDarkBackgroundColor() const
{
	return m_clrDarkBg;
}

void CFcListCtrlEx::SetDarkBackgroundColor(COLORREF clrDarkBg)
{
	if(m_clrDarkBg != clrDarkBg)
	{
		m_clrDarkBg = clrDarkBg;
		if(NULL != m_hWnd)
			Invalidate(TRUE);
	}
}
