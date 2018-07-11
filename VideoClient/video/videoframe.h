#pragma once

struct VideoFrame
{
    double m_pts;
    int64_t m_duration;
    AVFrame* m_image;
	int		m_nImageWidth;
	int		m_nImageHeight;
	unsigned char* pBGR;

    VideoFrame() 
        : m_pts(0)
        , m_duration(0)
        , m_image(av_frame_alloc())
		, pBGR(nullptr)
		, m_nImageWidth(0)
		, m_nImageHeight(0)
    {}
    ~VideoFrame()
    {
        av_frame_free(&m_image);
		if (nullptr != pBGR)
			delete pBGR;
    }

    VideoFrame(const VideoFrame&) = delete;
    VideoFrame& operator=(const VideoFrame&) = delete;

    void free()
    {
        av_frame_unref(m_image);
		if (nullptr != pBGR)
		{
			delete pBGR;
			pBGR = NULL;
		}

    }
    void realloc(AVPixelFormat pix_fmt, int width, int height)
    {
        if (pix_fmt != m_image->format || width != m_image->width || height != m_image->height)
        {
            free();
            m_image->format = pix_fmt;
            m_image->width = width;
            m_image->height = height;
            av_frame_get_buffer(m_image, 16);
			m_nImageWidth = width;
			m_nImageHeight = height;
			pBGR = new unsigned char[width * height * 3];
        }
    }
};
