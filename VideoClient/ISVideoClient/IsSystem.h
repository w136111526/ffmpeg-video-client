#pragma once
#include <functional>
#include "IsOptions.h"
#include "boost/function.hpp"
#include "boost/bind.hpp"
#include "boost/signal.hpp"
#include "boost/ref.hpp"
#include "dlib/noncopyable.h"
#include "dlib/any.h"

typedef struct TaskInfo
{
	HWND				m_hWnd;
	cv::Mat				mat;
	int					nVideoIndex = -1;
	float*				fData;
	int					fDataLen;
	int                 nWidth;
	int                 nHeight;
	TaskInfo()
	{
		fData = nullptr;
	}
	~TaskInfo()
	{
		if (nullptr != fData)
			delete fData;
	}
	std::vector<RECT>	vectFacePos;
}TaskInfo;

//using namespace boost;
using namespace std;
#define INI_FILENAME_OPTION					_T("Option.ini")
#define INI_FILENAME_RECENTUSED				_T("RecentUsed.ini")
//! Timer ID
#define MAIN_TIMER_ID				1
#define SYSLOG_TIMER_ID				2
#define  TIMER_ID_INFO				3

//! Message
#define UM_WINDOW_HIDE			(WM_USER + 1)
#define UM_TRAY_CALLBACK		(WM_USER + 2)
#define UM_PROCESS_TASKEND		(WM_USER + 3)
#define UM_PROCESS_TASKUPDATE	(WM_USER + 4)

class CIsSystem : public dlib::noncopyable
{
public:

	static CIsSystem* GetInstance();
	//!Path
	string m_szConfigPath;
	string m_szLogPath;
	string m_szUploadPath;

	//System
	CIsOptions	m_IsOption;
	dlib::any_function<void(TaskInfo*) >	callback_hander2;
private:
	CIsSystem();
	~CIsSystem() {};
 };

