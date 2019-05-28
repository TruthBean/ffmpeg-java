#ifndef FFMPEG_JAVA_COMMON_H
#define FFMPEG_JAVA_COMMON_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <time.h>
#include <windows.h>
#else
#include <sys/time.h>
#endif // _WIN32


#include <libavutil/log.h>
#include <libavutil/frame.h>
#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>

#include <jpeglib.h>

enum ImageStreamType {
    YUV = 0,
    RGB,
    JPEG
};

enum FrameStatus {
    START = 0,
    END = 1,
    GRAB = 2,
    PENDIING = 3
};

typedef struct FrameData {
    AVFrame *frame;
    long pts;
    enum ImageStreamType type;
    int quality;

    unsigned long file_size;
    unsigned char *file_data;
    int ret;
    char *error_message;

    bool isThreadly;
} FrameData;

time_t get_now_microseconds();

void jpeg_write_mem(uint8_t *raw_data, int quality, unsigned int width, unsigned int height,
                           unsigned char **jpeg_data, unsigned long *jpeg_size);

/**
 * 拷贝avframe的数据，并转变成JPEG格式数据
 * @param frame: 图片帧
 * @param quality: jpeg 图片质量，1~100
 * @param codec_context: 视频AVCodecContext
 **/
void copy_frame_data_and_transform_to_jpeg(const AVCodecContext *codec_context, FrameData *result);

/**
 * 拷贝avframe的数据，并转变成YUV或RGB格式
 * @param frame: 图片帧
 * @param codec_context: 视频AVCodecContext
 * @param type: 转换的格式, YUV 或 RGB @see ImageStreamType
 * @return FrameData @see FrameData
 **/
void copy_frame_raw_data(const AVCodecContext *codec_context, FrameData *result);

// ================================================================================

#define LOCK 1
#define UNLOCK 0

typedef struct LinkedQueueNodeData {
    AVFrame *frame;
    long pts;
    int ret;
    char *error_message;
} LinkedQueueNodeData;

typedef struct LinkedQueueNode {
    struct LinkedQueueNode *next;
    struct LinkedQueueNode *previous;
    LinkedQueueNodeData data;
} LinkedQueueNode;

typedef struct LinkedQueue {
    
    LinkedQueueNode *head;
    LinkedQueueNode *tail;
    // 队列容量大小
    size_t size;
    // 队列实际大小
    int real_size;
} LinkedQueue;

LinkedQueue *create_linkedQueue(LinkedQueue *queue, const size_t size);

void destroy_linkedQueue(LinkedQueue *queue);

void push_linkedQueue(LinkedQueue *queue, LinkedQueueNodeData data);

LinkedQueueNodeData pop_linkedQueue(LinkedQueue *queue);

#endif // FFMPEG_JAVA_COMMON_H