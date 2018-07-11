#include "stdafx.h"
#include "IsSystem.h"
#include "boost/filesystem.hpp"
using namespace boost::filesystem;

CIsSystem::CIsSystem()
{
	string strAppPath = getProcessPath();
	m_szConfigPath = strAppPath + _T("\\Config\\");
	CreateDirectory(m_szConfigPath.c_str(), NULL);
	ILog4zManager::getInstance()->start();

	LOGFMTI(_T("----------------------------------------------------------------------------------------------------"));
	LOGFMTI(_T("导入系统参数配置文件 (%s)"), m_szConfigPath + _T("Options.ini"));
	if (0 == m_IsOption.Load())
		LOGFMTE("读取配置文件失败");
}

CIsSystem* CIsSystem::GetInstance()
{
	static CIsSystem obj;
	return &obj;
}
