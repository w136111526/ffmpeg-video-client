#include "stdafx.h"
#include "IsPlayOpencv.h"
#include <vector>

#define CONVERT_FROM_YUV420P
uchar average[] = { 104, 117, 123 };

CIsPlayOpencv::CIsPlayOpencv(HWND hWnd, int nChannelIndex) : m_frameDecoder(GetFrameDecoder())
{
	if (::IsWindow(hWnd))
	{
		m_hWndPlay = hWnd;
	}

	m_pIsVideoDetectThread = new CIsVideoDetectThread;
	m_bIsPlaying = false;
	m_nChannelIndex = nChannelIndex;
	m_pIsVideoDetectThread->StartThread();
}

CIsPlayOpencv::~CIsPlayOpencv()
{
	CloseVideo();
	delete m_pIsVideoDetectThread;
}

bool CIsPlayOpencv::OpenVideoUrl(string strUrl, bool bUseD3D)
{
	m_frameDecoder->setFrameListener(this);
	m_frameDecoder->SetFrameFormat(IFrameDecoder::PIX_FMT_YUV420P, bUseD3D);
	CloseVideo();
	//if (strUrl.GetAt(0) != 'r');
	bool	bIsplay = false;
	if (strUrl.substr(0, 4) != "rtsp")
	{
		m_frameDecoder->SetLoopEnable(true);
		bIsplay = m_frameDecoder->openFile(strUrl);
	}
	else
	{
		bIsplay = m_frameDecoder->openUrl(strUrl);
	}
	if (bIsplay)
		m_frameDecoder->play();
	else
		AfxMessageBox("视频路径打开失败");
	m_bIsPlaying = bIsplay;

	return bIsplay;
}

void CIsPlayOpencv::CloseVideo()
{
	m_frameDecoder->close();
	m_bIsPlaying = false;
}

void CIsPlayOpencv::updateFrame()
{
	FrameRenderingData data;
	if (!m_frameDecoder->getFrameRenderingData(&data))
		return;
	//m_pImage = data.pImg;
	if (m_sourceSize.cx != data.width || m_sourceSize.cy != data.height)
	{
		m_sourceSize.cx = data.width;
		m_sourceSize.cy = data.height;
	}
	TaskInfo* pTaskInfo = new TaskInfo;
	pTaskInfo->nVideoIndex = m_nChannelIndex;
	pTaskInfo->m_hWnd = m_hWndPlay;
	cv::Mat mat_(data.height, data.width, CV_8UC3, data.pBGR);
	cv::resize(mat_, pTaskInfo->mat, cv::Size(data.width / 2, data.height / 1.5));
	if (nullptr != m_pIsVideoDetectThread)
		m_pIsVideoDetectThread->PushFrame(pTaskInfo);
}

void CIsPlayOpencv::drawFrame(IFrameDecoder* decoder, unsigned int generation)
{

}

void CIsPlayOpencv::DisplayVideo(TaskInfo * jobTask)
{
	RECT screenPosition = GetScreenPosition();
	CRect target(POINT{}, POINT{ screenPosition.right - screenPosition.left, screenPosition.bottom - screenPosition.top });
	HDC hDC = ::GetDC(m_hWndPlay);
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
	ReleaseDC(m_hWndPlay, hDC);
}

RECT CIsPlayOpencv::GetScreenPosition()
{
	RECT desc;
	GetClientRect(m_hWndPlay, &desc);
	return desc;
}
