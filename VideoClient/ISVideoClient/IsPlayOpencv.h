#pragma once
#include "../video/decoderinterface.h"
#include "CvvImage.h"
#include "IsVideoDetectThread.h"

using namespace cv;
class CIsPlayOpencv : public IFrameListener
{
public:
	CIsPlayOpencv(HWND hWnd, int nChannelIndex);
	~CIsPlayOpencv();

	HWND GetHWND() { return m_hWndPlay; }                         // 获取视频显示的窗口句柄
	bool OpenVideoUrl(std::string strUrl, bool bUseD3D = true);
	bool IsPlaying() const { return m_bIsPlaying; }
	void CloseVideo();
	void updateFrame();
	void drawFrame(IFrameDecoder* decoder, unsigned int generation);
	void DisplayVideo(TaskInfo* jobTask);

private:
	bool							m_bIsPlaying;
	CSize							m_sourceSize;
	CSize							m_aspectRatio;
	HWND							m_hWndPlay;
	int								m_nChannelIndex;
	IplImage*						m_pImage;
	CvvImage						m_cvImage;
	CIsVideoDetectThread*			m_pIsVideoDetectThread;
	std::unique_ptr<IFrameDecoder>	m_frameDecoder;
	RECT GetScreenPosition();
};

