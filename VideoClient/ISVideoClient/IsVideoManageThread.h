#pragma once

#include <tuple>
#include "IsPlayOpencv.h"
#include "IsVideoDetectThread.h"
#define  MAX_VIDEO_LIST			100

typedef tuple<HWND, string, LPVOID>				Tuple_VideoInfo;
typedef std::vector<Tuple_VideoInfo>			Vect_VideoInfo;

class CIsVideoManageThread
{
public:
	CIsVideoManageThread();
	~CIsVideoManageThread();

	void InitVideoInfo(Vect_VideoInfo vectVideoInfo);
	void StartVideoDisPlay();
	void StopAllVideoDisPlay();
	void OutDisplayVideo(TaskInfo* jobTask);

	Vect_VideoInfo					m_vectVideoInfo;
	CIsPlayOpencv*					m_IsVideoList[MAX_VIDEO_LIST];
};

