#说明<br>
.一个优秀的ffmpeg视频解码器源代码，编译方式：Release.x64版本。<br>
.windows下播放网络视频流低延时<br>
.解码器支持网络摄像头、本地视频文件、电脑自带usb摄像头、桌面抓取方法。
.视频解码处理过程已经经过硬解码，及如何从硬解码后采用SSE，ipp，等intel优化提取数据。单路1080P下视频从解码到复原RGB数据CPU降至%2以内。<br>
.duilib下多路网络视频播放demo.内部采用opencv播放（D3D播放版本暂未上传）<br>
#第三方开源库<br>
.opencv;dlib;opencv;SSE;ipp;boost库<br>
.boost文件过大需自己去官网下载编译<br>
＃其他<br>
.如有其他代码问题，请联系我QQ136111526<br>

