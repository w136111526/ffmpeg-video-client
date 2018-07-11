// DlgAddressList.cpp : 实现文件
//

#include "stdafx.h"
#include "ISVideoClient.h"
#include "DlgUrlList.h"
#include "afxdialogex.h"
#include <fstream>

// CDlgAddressList 对话框

IMPLEMENT_DYNAMIC(CDlgUrlList, CDialogEx)

CDlgUrlList::CDlgUrlList(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG_ADDRESS, pParent)
{
	m_pIsOptions = &CIsSystem::GetInstance()->m_IsOption;
}

CDlgUrlList::~CDlgUrlList()
{
}

void CDlgUrlList::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_ADDRESS, m_lstAdress);
}


BEGIN_MESSAGE_MAP(CDlgUrlList, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_ADD, &CDlgUrlList::OnBnClickedButtonAdd)
	ON_BN_CLICKED(IDC_BUTTON_DEL, &CDlgUrlList::OnBnClickedButtonDel)
END_MESSAGE_MAP()


// CDlgAddressList 消息处理程序


BOOL CDlgUrlList::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	m_lstAdress.SetItemHeight(23);
	m_lstAdress.InsertColumn(0, _T("序号"), LVCFMT_CENTER, 18);
	m_lstAdress.InsertColumn(1, _T("IP 地址"), LVCFMT_CENTER, 200);
	m_lstAdress.SetColumnType(1, FTC_LISTCTRLEX_COLUMNTYPE_EDIT);
	m_lstAdress.InitHScrollMode(FALSE, 1);

	vector<string>& vectUrl = m_pIsOptions->GetUrlList();
	for (int i = 0; i < vectUrl.size(); i++)
	{
		CString strValue;
		strValue.Format(_T("%02d"), i + 1);
		m_lstAdress.InsertItem(i, strValue);
		m_lstAdress.SetItemText(i, 1, vectUrl[i].c_str());
	}
	CenterWindow();
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}

void CDlgUrlList::OnOK()
{
	// TODO: 在此添加专用代码和/或调用基类
	fstream file;
	vector<string> vectUrl;
	for (int i = 0; i < m_lstAdress.GetItemCount(); i++)
	{
		CString strValue = m_lstAdress.GetItemText(i, 1);
		if (!strValue.IsEmpty())
		{
			if (strValue.Left(4) == "rtsp")
				vectUrl.push_back((LPCTSTR)strValue);
			else
			{
				file.open(strValue, ios::in);
				if (file)
					vectUrl.push_back((LPCTSTR)strValue);
				file.close();
			}
		}
	}
	m_pIsOptions->SetUrlList(vectUrl);
	m_pIsOptions->Save();

	CDialogEx::OnOK();
}

void CDlgUrlList::OnBnClickedButtonAdd()
{
	int nCount = m_lstAdress.GetItemCount();
	if (nCount > 20)
		return;
	CString strValue;
	strValue.Format(_T("%02d"), nCount + 1);
	m_lstAdress.InsertItem(nCount, strValue);
	if (nCount > 0)
		m_lstAdress.SetItemText(nCount, 1, m_lstAdress.GetItemText(nCount - 1, 1));
	CUIntArray arAddItem;
	int nCurSel = m_lstAdress.GetNextItem(-1, LVNI_ALL | LVNI_SELECTED);
}

void CDlgUrlList::OnBnClickedButtonDel()
{
	POSITION pos = m_lstAdress.GetFirstSelectedItemPosition();
	int iCurSel = m_lstAdress.GetNextSelectedItem(pos);
	m_lstAdress.DeleteItem(iCurSel);
	int iCount = m_lstAdress.GetItemCount();
	if (iCount > 0)
	{
		m_lstAdress.SetItemState(iCurSel, LVIS_FOCUSED | LVIS_SELECTED, LVIS_FOCUSED | LVIS_SELECTED);
		m_lstAdress.EnsureVisible(iCurSel, TRUE);
		CString szValue;
		for (int i = iCurSel; i < iCount; i++)
		{
			szValue.Format(_T("%02d"), i + 1);
			m_lstAdress.SetItemText(i, 0, szValue);
		}
	}
}
