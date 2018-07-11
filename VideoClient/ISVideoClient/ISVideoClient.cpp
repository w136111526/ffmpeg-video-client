
// ISVideoClient.cpp : 定义应用程序的类行为。
//

#include "stdafx.h"
#include "ISVideoClient.h"
#include "ISVideoClientDlg.h"
#include "IsSystem.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CISVideoClientApp

BEGIN_MESSAGE_MAP(CISVideoClientApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CISVideoClientApp 构造

CISVideoClientApp::CISVideoClientApp()
{
	// 支持重新启动管理器
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;

	// TODO: 在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
}


// 唯一的一个 CISVideoClientApp 对象

CISVideoClientApp theApp;


// CISVideoClientApp 初始化

BOOL CISVideoClientApp::InitInstance()
{
	// 如果一个运行在 Windows XP 上的应用程序清单指定要
	// 使用 ComCtl32.dll 版本 6 或更高版本来启用可视化方式，
	//则需要 InitCommonControlsEx()。  否则，将无法创建窗口。
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// 将它设置为包括所有要在应用程序中使用的
	// 公共控件类。
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();


	AfxEnableControlContainer();

	// 创建 shell 管理器，以防对话框包含
	// 任何 shell 树视图控件或 shell 列表视图控件。
	CShellManager *pShellManager = new CShellManager;

	// 激活“Windows Native”视觉管理器，以便在 MFC 控件中启用主题
	CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));

	// 标准初始化
	// 如果未使用这些功能并希望减小
	// 最终可执行文件的大小，则应移除下列
	// 不需要的特定初始化例程
	// 更改用于存储设置的注册表项
	// TODO: 应适当修改该字符串，
	// 例如修改为公司或组织名
	SetRegistryKey(_T("应用程序向导生成的本地应用程序"));


	HANDLE hMutex = CreateMutex(NULL, FALSE, _T("ISVideoClient"));
	if (hMutex == NULL || GetLastError() == ERROR_ALREADY_EXISTS)
	{
		MessageBox(NULL, "当前操作系统中已经运行了另一个实例", "提示:", MB_OK);
		return FALSE;
	}

	::CoInitialize(NULL);
	CIsSystem* pIsSystem = CIsSystem::GetInstance();
	CPaintManagerUI::SetInstance(AfxGetInstanceHandle());
	CPaintManagerUI::SetCurrentPath(CPaintManagerUI::GetInstancePath());
	CPaintManagerUI::SetResourcePath(_T("Skin"));
	//	CPaintManagerUI::SetResourceZip(_T("Main_dlg.zip"));

	CISVideoClientWnd *pFrame = new CISVideoClientWnd("Main_dlg.xml");

	pFrame->Create(NULL, _T("视频播放器"), UI_WNDSTYLE_FRAME, WS_EX_WINDOWEDGE);
	pFrame->ShowModal();

	::CoUninitialize();

	// 由于对话框已关闭，所以将返回 FALSE 以便退出应用程序，
	//  而不是启动应用程序的消息泵。
	return FALSE;
}

