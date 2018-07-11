#include "stdafx.h"
#include "IsOptions.h"
//#include "PsSystem.h"
#include  <stdlib.h>
#include <io.h>
#include <boost/property_tree/ini_parser.hpp>
#include "log4z.h"

#define PASSWORD_KEY	_T("isvision base version 1.0.0.0")
using namespace boost::property_tree;
CIsOptions::CIsOptions()
{
	m_nDetectMinFace						= 20;
	m_nDetectMaxFace						= 800;
	m_nDetectSpeed							= 3;
	m_nSortType								= 1;
	m_nAlgoType								= 2;

	m_bUseNetDog							= false;					//网络狗
	m_strSN									= "";						//序列号
	m_strLIC								="";						//许可号
	m_nVideoType							= 1;
	m_nSeq									= 0;
	m_nExposure								= -6;

	m_vectUrlList.clear();
}

CIsOptions::~CIsOptions()
{
}

int CIsOptions::Load()
{
 	int bRet = 1;
	std::string szIniFilePath = zsummer::log4z::getProcessPath() + "\\Config\\Option.ini";
	if (_access(szIniFilePath.c_str(), 0) == -1)
		return -1;
	ptree pRead;
	read_ini(szIniFilePath, pRead);

	//AlgoParam
	m_nDetectMinFace						= pRead.get("AlgoParam.DetectMinFace", 30);
	m_nDetectMaxFace						= pRead.get("AlgoParam.DetectMaxFace", 800);
	m_nDetectSpeed							= pRead.get("AlgoParam.DetectSpeed", 2);					//1:Slow;2:Normal;3:Faster
	m_nSortType								= pRead.get("AlgoParam.SortType", 1);						//1:None;2:Ascending;3:Descending
	m_nAlgoType								= pRead.get("AlgoParam.AlgoType", 2);						//1:TypeA;2:TypeB;3:TypeC(v9007)
	m_bUseNetDog							= pRead.get("AlgoParam.UseNetDog", 0);						//网络狗
	m_strSN									= pRead.get("AlgoParam.SN", "");							//序列号
	m_strLIC								= pRead.get("AlgoParam.LIC", "");							//许可号

	//Camera&Video
	m_nVideoType							= pRead.get("Video.VideoType", 0);							//视频类型
	m_nSeq									= pRead.get("Video.Seq", 0);								//摄像头编号
	m_nExposure								= pRead.get("Video.Exposure", -6);							//曝光值-13 - 0
	int nCount								= pRead.get("Url.Count", 0);
	for (int i = 0; i < nCount; i++)
	{
		char info[260] = { 0 };
		sprintf_s(info, "Url.detail_%02d", i);
		string strInfo = pRead.get(info, "");
		if(false == strInfo.empty())
			m_vectUrlList.push_back(strInfo);
	}
	
	return bRet;
}

int CIsOptions::Save()
{
	std::string szIniFilePath = zsummer::log4z::getProcessPath() + "\\Config\\Option.ini";

	ptree pWrite;

	//AlgoParam								 
	pWrite.put("AlgoParam.DetectMinFace	", m_nDetectMinFace);
	pWrite.put("AlgoParam.DetectMaxFace", m_nDetectMaxFace);
	pWrite.put("AlgoParam.DetectSpeed	", m_nDetectSpeed);					//1:Slow;2:Normal;3:Faster
	pWrite.put("AlgoParam.SortType", m_nSortType);							//1:None;2:Ascending;3:Descending
	pWrite.put("AlgoParam.AlgoType", m_nAlgoType);							//1:TypeA;2:TypeB;3:TypeC(v9007)
	pWrite.put("AlgoParam.UseNetDog", m_bUseNetDog);						//网络狗
	pWrite.put("AlgoParam.SN", m_strSN);									//序列号
	pWrite.put("AlgoParam.LIC", m_strLIC);									//许可号
											
	//Camera&Video					
	pWrite.put("Video.VideoType", m_nVideoType);							//视频类型
	pWrite.put("Video.Seq", m_nSeq);										//摄像头编号
	pWrite.put("Video.Exposure", m_nExposure);								//曝光值-13 - 0

	pWrite.put("Url.Count", m_vectUrlList.size());
	for (int i = 0; i < m_vectUrlList.size(); i++)
	{
		char info[260] = { 0 };
		sprintf_s(info, "Url.detail_%02d", i);
		pWrite.put(info, m_vectUrlList[i]);
	}
	write_ini(szIniFilePath, pWrite);	
	return 0;
}														 