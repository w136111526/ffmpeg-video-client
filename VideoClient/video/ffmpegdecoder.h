#pragma once

#include "decoderinterface.h"

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libswresample/swresample.h>
//#include <libavdevice/avdevice.h>
}

#include <string>
#include <boost/thread/thread.hpp>
#include <boost/atomic.hpp>
#include "videoframe.h"
#include <memory>
#include <vector>

#include <boost/log/sources/channel_logger.hpp>
#include <boost/log/common.hpp>

namespace channel_logger
{

extern boost::log::sources::channel_logger_mt<> 
    ffmpeg_audio,
    ffmpeg_closing, 
    ffmpeg_opening, 
    ffmpeg_pause,
    ffmpeg_readpacket, 
    ffmpeg_seek, 
    ffmpeg_sync,
    ffmpeg_threads, 
    ffmpeg_volume;

} // namespace channel_logger

#define CHANNEL_LOG(channel) BOOST_LOG(::channel_logger::channel)

#include "fqueue.h"
#include "videoframe.h"
#include "vqueue.h"
#include <io.h>
#include <time.h>

double GetHiResTime();

// Inspired by http://dranger.com/ffmpeg/ffmpeg.html

class FFmpegDecoder : public IFrameDecoder
{
   public:
    FFmpegDecoder();
    ~FFmpegDecoder();

    FFmpegDecoder(const FFmpegDecoder&) = delete;
    FFmpegDecoder& operator=(const FFmpegDecoder&) = delete;

	static void WriteErrorInfo(const char* format, ...);

    void SetFrameFormat(FrameFormat format, bool allowDirect3dData) override;

	bool openFile(const PathType& file) override;
	bool openUrl(const std::string& url) override;
	bool openCamera();
	bool openDesktop();
	void SetLoopEnable(bool bLoop);

    bool seekDuration(int64_t duration);
    bool seekByPercent(double percent) override;
    void videoReset() override;

    inline bool isPlaying() const override { return m_isPlaying; }
    inline bool isPaused() const override { return m_isPaused; }

    void setFrameListener(IFrameListener* listener) override { m_frameListener = listener; }

    void setDecoderListener(FrameDecoderListener* listener) override
    {
        m_decoderListener = listener;
    }

    bool getFrameRenderingData(FrameRenderingData* data) override;

    double getDurationSecs(int64_t duration) const override
    {
        return av_q2d((m_videoStream != nullptr)? m_videoStream->time_base : AVRational()) * duration;
    }

    void finishedDisplayingFrame(unsigned int generation) override;

    void close() override;
    void play(bool isPaused = false) override;

   private:
    class IOContext;

    // Threads
    void parseRunnable();
    void videoParseRunnable();
    void displayRunnable();

	void dispatchPacket(AVPacket& packet);
    void startVideoThread();
    bool resetDecoding(int64_t seekDuration, bool resetVideo);
    void fixDuration();
    bool handleVideoPacket(
        const AVPacket& packet,
        double& videoClock,
        bool& initialized);

    void resetVariables();
    void closeProcessing();
	bool openDecoder(const PathType& file, const std::string& url, bool isFile, bool bCamera = false, bool bDesktop = false);
    bool resetVideoProcessing();
    void seekWhilePaused();

    void handleDirect3dData(AVFrame* videoFrame, VideoFrame& video);

	// Indicators
	bool m_isPlaying;
    // Frame display listener
    IFrameListener* m_frameListener;
    FrameDecoderListener* m_decoderListener;
    std::unique_ptr<boost::thread> m_mainVideoThread;
    std::unique_ptr<boost::thread> m_mainParseThread;
    std::unique_ptr<boost::thread> m_mainDisplayThread;

    // Synchronization
    boost::atomic<double> m_audioPTS;

    // Real duration from video stream
    int64_t m_startTime;
    boost::atomic_int64_t m_currentTime;
    int64_t m_duration;

    // Basic stuff
    AVFormatContext* m_formatContext;
    boost::atomic_int64_t m_seekDuration;
    boost::atomic_int64_t m_videoResetDuration;
    boost::atomic_bool m_videoResetting;

    // Video Stuff
    boost::atomic<double> m_videoStartClock;

    AVCodec* m_videoCodec;
    AVCodecContext* m_videoCodecContext;
    AVStream* m_videoStream;
    int m_videoStreamNumber;

    // Stuff for converting image
    AVFrame* m_videoFrame;
    SwsContext* m_imageCovertContext;
    AVPixelFormat m_pixelFormat;
    bool m_allowDirect3dData;
	bool m_bValidHardWare;

    // Video queues
    enum
    {
        MAX_QUEUE_SIZE = (15 * 1024 * 1024),
        MAX_VIDEO_FRAMES = 200,
    };
    FQueue<MAX_QUEUE_SIZE, MAX_VIDEO_FRAMES> m_videoPacketsQueue;

    VQueue m_videoFramesQueue;

    bool m_frameDisplayingRequested;
	bool m_bIsFile;
	bool m_bIsCamera;
	bool m_bDesktop;
	bool m_bLoopEnable;
	bool m_bValidDxva2;

    unsigned int m_generation;

    boost::mutex m_videoFramesMutex;
    boost::condition_variable m_videoFramesCV;
    boost::atomic_bool m_isPaused;
    boost::mutex m_isPausedMutex;
    boost::condition_variable m_isPausedCV;
    double m_pauseTimer;
    bool m_isVideoSeekingWhilePaused;
    std::unique_ptr<IOContext> m_ioCtx;
};
