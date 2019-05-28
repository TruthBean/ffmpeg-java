#ifndef FFMPEG_JAVA_VIDEO2IMAGES_H
#define FFMPEG_JAVA_VIDEO2IMAGES_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavfilter/buffersink.h>
#include <libavfilter/buffersrc.h>
#include <libswscale/swscale.h>
#include <libavutil/opt.h>
#include <libavutil/imgutils.h>
#include <libavutil/pixfmt.h>

#include "./common.h"

typedef struct Video2ImageStream {
    AVFormatContext *format_context;
    AVStream *video_stream;
    int video_stream_idx;
    AVCodecContext *video_codec_context;
    AVCodecParserContext *parser_context;
    int ret;
    char *error_message;
    bool isRtsp;
    int frame_rate;
} Video2ImageStream;

typedef struct FFmpegInputParams {
    const char *video_addr;
    int timeout;
    bool nobuffer;
    bool use_gpu;
    bool use_tcp;
    int gpu_id;
} FFmpegInputParams;

typedef void (Video2ImagesCallback)(const AVCodecContext *video_codec_context, FrameData *data);

/**
 * 连接视频地址，获取数据流
 * @param result @see Video2ImageStream
 * @param params: @see FFmpegInputParams
 **/
void open_inputfile(Video2ImageStream *result, FFmpegInputParams params);

/**
 * 视频流获取 图片帧
 * @param vis: 存储在全局变量中的视频流数据变量
 * @param quality: jpeg 图片质量，1~100
 * @param chose_frames: 一秒取多少帧, 超过视频帧率则为视频帧率
 * @param type: 图片类型 @see ImageStreamType
 * @param callback: callback function @see Video2ImagesCallback
 * @param result [out] @see FrameData 
 **/
void video2images_grab(Video2ImageStream *vis, int quality, int chose_frames, enum ImageStreamType type, Video2ImagesCallback callback, FrameData *result);

void release(Video2ImageStream *vis);

#endif // FFMPEG_JAVA_VIDEO2IMAGES_H