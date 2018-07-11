#include "stdafx.h"
#include "dlib/misc_api.h"
#include "IsVideoDetectThread.h"


CIsVideoDetectThread::CIsVideoDetectThread() : m_jobDetectTask(25)
{
	register_thread(*this, &CIsVideoDetectThread::DetectThread);
}

CIsVideoDetectThread::~CIsVideoDetectThread()
{
	StopThread();
}

void CIsVideoDetectThread::PushFrame(TaskInfo * jobTask)
{
	m_jobDetectTask.enqueue(jobTask);
}

void CIsVideoDetectThread::StartThread()
{
	////Init Algo
	start();
}

void CIsVideoDetectThread::DetectThread()
{
	TaskInfo* jobTask = nullptr;
	vector<cv::Mat> vBatchImageData;
	TaskInfo* jobTaskInfo;
	int batch_size = 10;
	int Width = 1920;
	int Height = 1080;
	int shrink = 4;
	int item = 0;
	
	while (m_jobDetectTask.dequeue(jobTaskInfo))
	{
		//CIsSystem::GetInstance()->callback_hander2(jobTaskInfo);
		DisplayVideo(jobTaskInfo);
		delete jobTaskInfo;
		item = 0;
	}
}

void CIsVideoDetectThread::DisplayVideo(TaskInfo * jobTask)
{
	RECT desc;
	GetClientRect(jobTask->m_hWnd, &desc);
	CRect target(POINT{}, POINT{ desc.right - desc.left, desc.bottom - desc.top });
	HDC hDC = ::GetDC(jobTask->m_hWnd);
	if (jobTask->vectFacePos.size() > 0)
	{
		for (int i = 0; i < jobTask->vectFacePos.size(); i++)
		{
			RECT& rc = jobTask->vectFacePos[i];
			cv::rectangle(jobTask->mat, cvRect(rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top), cv::Scalar(0, 255, 0), 1, 8, 0);
		}
	}
	IplImage pImage = jobTask->mat;
	m_cvImage.CopyOf(&pImage);
	m_cvImage.DrawToHDC(hDC, &target);
	ReleaseDC(jobTask->m_hWnd, hDC);
}

void CIsVideoDetectThread::StopThread()
{
	m_jobDetectTask.wait_until_empty();
	m_jobDetectTask.disable();
	wait();
}
