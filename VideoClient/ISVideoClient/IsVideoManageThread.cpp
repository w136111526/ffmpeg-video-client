#include "stdafx.h"

#include "IsVideoManageThread.h"
#include "ViewCtrl.h"
#include "IsSystem.h"

CIsVideoManageThread::CIsVideoManageThread()
{
	for (int i = 0; i < MAX_VIDEO_LIST; i++)
		m_IsVideoList[i] = nullptr;
	CIsSystem::GetInstance()->callback_hander2 = make_mfp(*this, &CIsVideoManageThread::OutDisplayVideo);
}

CIsVideoManageThread::~CIsVideoManageThread()
{
	StopAllVideoDisPlay();
}

void CIsVideoManageThread::OutDisplayVideo(TaskInfo* jobTask)
{
	if (nullptr != jobTask)
	{
		int nVideoIndex = jobTask->nVideoIndex;
		if (nVideoIndex != -1 && nVideoIndex >= 0 && nVideoIndex < m_vectVideoInfo.size())
			m_IsVideoList[nVideoIndex]->DisplayVideo(jobTask);
		delete jobTask;
		jobTask = nullptr;
	}
}

void CIsVideoManageThread::InitVideoInfo(Vect_VideoInfo vectVideoInfo)
{
	m_vectVideoInfo.clear();
	m_vectVideoInfo.swap(vectVideoInfo);
}

void CIsVideoManageThread::StartVideoDisPlay()
{
	for (int i = 0; i < m_vectVideoInfo.size() && i < MAX_VIDEO_LIST; i++)
	{
		Tuple_VideoInfo& tuple_video = m_vectVideoInfo[i];
		if(nullptr == m_IsVideoList[i])
			m_IsVideoList[i] = new CIsPlayOpencv(get<0>(tuple_video), i);
		if (m_IsVideoList[i]->OpenVideoUrl(get<1>(tuple_video)))
		{
			CViewCtrlUI* pViewCtrl = (CViewCtrlUI*)get<2>(tuple_video);
			pViewCtrl->SetTag(TRUE);
		}
	}
}

void CIsVideoManageThread::StopAllVideoDisPlay()
{
	for (int i = 0; i < m_vectVideoInfo.size() && i < MAX_VIDEO_LIST; i++)
	{
		Tuple_VideoInfo& tuple_video = m_vectVideoInfo[i];
		if (nullptr != m_IsVideoList[i])
		{
			m_IsVideoList[i]->CloseVideo();
			CViewCtrlUI* pViewCtrl = (CViewCtrlUI*)get<2>(tuple_video);
			pViewCtrl->SetTag(FALSE);
			delete m_IsVideoList[i];
			m_IsVideoList[i] = nullptr;
		}
	}
	m_vectVideoInfo.clear();
}
