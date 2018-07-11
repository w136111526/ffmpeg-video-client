#pragma once

#include "FcListCtrlEx.h"
#include "IsSystem.h"

// CDlgAddressList 对话框

class CDlgUrlList : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgUrlList)

public:
	CDlgUrlList(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgUrlList();

	// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_ADDRESS };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	virtual void OnOK();

	DECLARE_MESSAGE_MAP()


private:
	CFcListCtrlEx	m_lstAdress;
	CIsOptions*		m_pIsOptions;
public:
	afx_msg void OnBnClickedButtonAdd();
	afx_msg void OnBnClickedButtonDel();
};
