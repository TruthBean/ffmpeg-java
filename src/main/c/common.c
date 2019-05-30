﻿#include "./common.h"

time_t get_now_microseconds() {
    time_t result;
    time_t now_seconds;

#ifdef _WIN32
    now_seconds = time(NULL);
    result = now_seconds * 1000000 + GetTickCount64();
#else
    struct timeval tv;
    gettimeofday(&tv, NULL);

    result = 1000000 * tv.tv_sec + tv.tv_usec;
#endif

    return result;
}

/**
 * 将rgb 数据 转换成 jpeg数据，存储在内存中
 * @param raw_data: [in] rgb数据
 * @param quality: [in] jpeg 质量，1~100
 * @param width: [in] 图片宽度
 * @param height: [in] 图片高度
 * @param jpeg_data: [out] 存储在内存中的JPEG数据
 * @param jpeg_size: [out] JPEG数据的大小
 **/
void jpeg_write_mem(uint8_t *raw_data, int quality, unsigned int width, unsigned int height,
                           unsigned char **jpeg_data, unsigned long *jpeg_size) {
    av_log(NULL, AV_LOG_DEBUG, "begin jpeg_write_mem time: %ld\n", get_now_microseconds());

    // 分配和一个jpeg压缩对象
    struct jpeg_compress_struct jpeg_struct;
    struct jpeg_error_mgr jpeg_error;

    jpeg_struct.err = jpeg_std_error(&jpeg_error);
    // 初始化
    jpeg_create_compress(&jpeg_struct);

    jpeg_struct.image_width = width;
    jpeg_struct.image_height = height;
    jpeg_struct.input_components = 3;
    jpeg_struct.in_color_space = JCS_RGB;

    // 设置压缩参数
    jpeg_set_defaults(&jpeg_struct);

    // 设置质量参数
    jpeg_set_quality(&jpeg_struct, quality, TRUE);

    // 存放在内存中
    jpeg_mem_dest(&jpeg_struct, jpeg_data, jpeg_size);

    // 开始压缩循环，逐行进行压缩：使用jpeg_start_compress开始一个压缩循环
    jpeg_start_compress(&jpeg_struct, TRUE);

    // 一行数据所占字节数:如果图片为RGB，这个值要*3.灰度图像不用
    int row_stride = width * 3;
    JSAMPROW row_pointer[1];
    while (jpeg_struct.next_scanline < height) {
        // 需要压缩成jpeg图片的位图数据缓冲区
        row_pointer[0] = &raw_data[jpeg_struct.next_scanline * row_stride];
        // 1表示写入一行
        (void)jpeg_write_scanlines(&jpeg_struct, row_pointer, 1);
    }

    // 结束压缩循环
    jpeg_finish_compress(&jpeg_struct);
    // 释放jpeg压缩对象
    jpeg_destroy_compress(&jpeg_struct);

    av_log(NULL, AV_LOG_DEBUG, "compress one picture success: %ld.\n", get_now_microseconds());
}

/**
 * 拷贝avframe的数据，并转变成YUV或RGB格式
 * @param frame: 图片帧
 * @param codec_context: 视频AVCodecContext
 * @param type: 转换的格式, YUV 或 RGB @see ImageStreamType
 * @return FrameData @see FrameData
 **/
void copy_frame_raw_data(const AVCodecContext *codec_context, FrameData *result) {

    av_log(NULL, AV_LOG_DEBUG, "begin copy_frame_raw_data time: %ld\n", get_now_microseconds());

    av_log(NULL, AV_LOG_DEBUG, "saving frame %3d\n", codec_context->frame_number);

    // 申请一个新的 frame, 用于转换格式
    AVFrame *target_frame = av_frame_alloc();
    if (!target_frame) {
        av_log(NULL, AV_LOG_ERROR, "Could not allocate target images frame\n");
        result->ret = -20;
        result->error_message = "Could not allocate target images frame";
        return;
    }
    target_frame->quality = 1;

    //保存jpeg格式
    enum AVPixelFormat target_pixel_format = AV_PIX_FMT_YUV420P;
    if (result->type == RGB) target_pixel_format = AV_PIX_FMT_RGB24;

    int align = 1; // input_pixel_format->linesize[0] % 32;

    // 获取一帧图片的体积
    int picture_size = av_image_get_buffer_size(target_pixel_format, codec_context->width,
                                                codec_context->height, align);

    // 申请一张图片数据的存储空间
    uint8_t *outBuff = (uint8_t *)av_malloc((size_t)picture_size);
    av_image_fill_arrays(target_frame->data, target_frame->linesize, outBuff, target_pixel_format,
                         codec_context->width, codec_context->height, align);

    // 设置图像转换上下文
    int target_width = codec_context->width;
    int target_height = codec_context->height;

    enum AVPixelFormat pixFormat;
    switch (codec_context->pix_fmt) {
        case AV_PIX_FMT_YUVJ420P:
            pixFormat = AV_PIX_FMT_YUV420P;
            break;
        case AV_PIX_FMT_YUVJ422P:
            pixFormat = AV_PIX_FMT_YUV422P;
            break;
        case AV_PIX_FMT_YUVJ444P:
            pixFormat = AV_PIX_FMT_YUV444P;
            break;
        case AV_PIX_FMT_YUVJ440P:
            pixFormat = AV_PIX_FMT_YUV440P;
            break;
        default:
            pixFormat = codec_context->pix_fmt;
            break;
    }

    // 转换图像格式
    struct SwsContext *sws_context = sws_getContext(codec_context->width, codec_context->height,
                                                    pixFormat,
                                                    target_width, target_height,
                                                    target_pixel_format,
                                                    SWS_BICUBIC, NULL, NULL, NULL);

    // 图片拉伸，数据转换
    sws_scale(sws_context, result->frame->data, result->frame->linesize, 0, result->frame->height,
              target_frame->data, target_frame->linesize);

    target_frame->height = target_height;
    target_frame->width = target_width;
    target_frame->format = target_pixel_format;

    // avframe data to buffer
    int yuv_data_size = av_image_get_buffer_size(target_frame->format, target_frame->width, target_frame->height, 1);
    av_log(NULL, AV_LOG_DEBUG, "buffer size: %d\n", yuv_data_size);
    result->file_data = (unsigned char *)malloc((size_t) (yuv_data_size));
    result->file_size = (unsigned long)av_image_copy_to_buffer(result->file_data, yuv_data_size,
                                                              (const uint8_t **) target_frame->data, target_frame->linesize,
                                                              target_frame->format, target_frame->width, target_frame->height, 1);

    sws_freeContext(sws_context);
    sws_context = NULL;
    av_freep(&outBuff);
    av_free(outBuff);
    outBuff = NULL;
    av_frame_unref(target_frame);
    av_frame_free(&target_frame);
    target_frame = NULL;
    av_log(NULL, AV_LOG_DEBUG, "finish copy_frame_raw_data time: %ld\n", get_now_microseconds());
    return;
}

/**
 * 拷贝avframe的数据，并转变成JPEG格式数据
 * @param frame: 图片帧
 * @param quality: jpeg 图片质量，1~100
 * @param codec_context: 视频AVCodecContext
 **/
void copy_frame_data_and_transform_to_jpeg(const AVCodecContext *codec_context, FrameData *result) {
    av_log(NULL, AV_LOG_DEBUG, "begin copy_frame_data_and_transform_2_jpeg time: %li\n", get_now_microseconds());

    uint8_t *images_dst_data[4] = {NULL};
    int images_dst_linesize[4];

    result->type = JPEG;
    result->file_data = NULL;
    result->file_size = 0;

    AVFrame *target_frame = av_frame_alloc();
    if (!target_frame) {
        av_log(NULL, AV_LOG_ERROR, "Could not allocate target images frame\n");
        result->ret = -30;
        result->error_message = "Could not allocate target images frame";
        return;
    }

    av_log(NULL, AV_LOG_DEBUG, "saving frame %3d\n", codec_context->frame_number);

    target_frame->quality = 1;

    //保存jpeg格式
    enum AVPixelFormat target_pixel_format = AV_PIX_FMT_RGB24;

    int align = 1; // input_pixel_format->linesize[0] % 32;

    // 获取一帧图片的体积
    int picture_size = av_image_get_buffer_size(target_pixel_format, codec_context->width,
                                                codec_context->height, align);

    // 申请一张图片数据的存储空间
    uint8_t *outBuff = (uint8_t *)av_malloc((size_t)picture_size);
    av_image_fill_arrays(target_frame->data, target_frame->linesize, outBuff, target_pixel_format,
                         codec_context->width, codec_context->height, align);

    // 设置图像转换上下文
    int target_width = codec_context->width;
    int target_height = codec_context->height;

    enum AVPixelFormat pixFormat;
    switch (codec_context->pix_fmt) {
        case AV_PIX_FMT_YUVJ420P:
            pixFormat = AV_PIX_FMT_YUV420P;
            break;
        case AV_PIX_FMT_YUVJ422P:
            pixFormat = AV_PIX_FMT_YUV422P;
            break;
        case AV_PIX_FMT_YUVJ444P:
            pixFormat = AV_PIX_FMT_YUV444P;
            break;
        case AV_PIX_FMT_YUVJ440P:
            pixFormat = AV_PIX_FMT_YUV440P;
            break;
        default:
            pixFormat = codec_context->pix_fmt;
            break;
    }

    struct SwsContext *sws_context = sws_getContext(codec_context->width, codec_context->height,
                                                    pixFormat,
                                                    target_width, target_height,
                                                    target_pixel_format,
                                                    SWS_BICUBIC, NULL, NULL, NULL);

    // 转换图像格式
    sws_scale(sws_context, result->frame->data, result->frame->linesize, 0, result->frame->height,
              target_frame->data, target_frame->linesize);

    target_frame->height = target_height;
    target_frame->width = target_width;
    target_frame->format = target_pixel_format;

    av_image_alloc(images_dst_data, images_dst_linesize,
                   target_frame->width, target_frame->height, target_frame->format, 16);

    /* copy decoded frame to destination buffer:
     * this is required since rawvideo expects non aligned data
     */
    av_image_copy(images_dst_data, images_dst_linesize,
                  (const uint8_t **)(target_frame->data), target_frame->linesize,
                  target_frame->format, target_frame->width, target_frame->height);

    // 图片压缩成Jpeg格式
    unsigned char *jpeg_data;
    unsigned long jpeg_size = 0L;
    jpeg_write_mem(images_dst_data[0], result->quality, (unsigned int)target_width,
                    (unsigned int)target_height, &jpeg_data, &jpeg_size);
	av_log(NULL, AV_LOG_DEBUG, "jpeg_size %ld \n", jpeg_size);
    result->file_data = (unsigned char *)malloc((size_t)(jpeg_size));
	// memset(result->file_data, 0, jpeg_size);
    memcpy(result->file_data, jpeg_data, jpeg_size);
    free(jpeg_data);
    jpeg_data = NULL;

    result->file_size = jpeg_size;

    // 释放内存
    av_freep(&images_dst_data[0]);
    av_free(images_dst_data[0]);
    images_dst_data[0] = NULL;
    *images_dst_data = NULL;

    sws_freeContext(sws_context);
    sws_context = NULL;
    av_freep(&outBuff);
    av_free(outBuff);
    outBuff = NULL;
    av_frame_unref(target_frame);
    av_frame_free(&target_frame);
    av_free(target_frame);
    target_frame = NULL;
    av_log(NULL, AV_LOG_DEBUG, "finish copy_frame_data_and_transform_2_jpeg time: %li\n", get_now_microseconds());
    return;
}

// ============================================================================================

int push_lock = UNLOCK;
int pop_lock = UNLOCK;

/**
 * 创建一个后进先出的队列
 * @param size
 * @return
 */
LinkedQueue *create_linkedQueue(LinkedQueue *queue, const size_t size) {
    // 申请队列内存长度
    if (queue == NULL)
        queue = malloc(size * sizeof(LinkedQueue));

    queue->head = queue->tail = malloc(sizeof(LinkedQueueNode));

    queue->size = size;
    // head previous always null
    // queue->head->next = NULL;
    // tail next always null
    // queue->tail->next = NULL;

    // empty queue: head next null, tail previous null
    queue->head->next = NULL;
    queue->tail->next = NULL;

    queue->real_size = 0;
    return queue;
}

void destroy_linkedQueue(LinkedQueue *queue) {
    if (queue != NULL) {
        LinkedQueueNode *node = queue->head;
        LinkedQueueNode *tmp = node;
        do {
            if (node != NULL) {
                if (node->next != NULL)
                    node = node->next;
                else node = NULL;

                tmp->next = NULL;
                free(tmp);
                queue->real_size -= 1;
            }
        } while (node != NULL);
        queue->size = 0;
        queue = NULL;
        free(queue);
    }
}

/**
 * 删除旧数据
 * 删除头部的数据
 * @param queue
 * @return
 */
static void __remove_linkedQueue(LinkedQueue *queue) {
    LinkedQueueNode *head = queue->head;
    if (head != NULL) {
        head = queue->head->next;
        if (head != NULL) {
            queue->head->next = head->next;
            if (queue->tail == head) queue->tail = queue->head;
            if (head->data.frame != NULL) {
                av_frame_unref(head->data.frame);
                av_frame_free(&(head->data.frame));
                free(head);
            }
            queue->real_size -= 1;
        }
    }
    
}

static LinkedQueueNodeData copy_nodeData(LinkedQueueNodeData data) {
    LinkedQueueNodeData result = {
            .ret = data.ret,
            .pts = data.pts,
            .error_message = data.error_message
    };

    AVFrame *dist_frame = av_frame_alloc();
    if (av_frame_copy(dist_frame, data.frame) >= 0) av_frame_copy_props(dist_frame, data.frame);
    av_frame_unref(data.frame);
    return result;
}

/**
 * 尾进
 * @param queue
 * @param data
 */
void push_linkedQueue(LinkedQueue *queue, LinkedQueueNodeData data) {
    // if (push_lock == LOCK && pop_lock == LOCK) push_lock = UNLOCK;
    // while (push_lock == LOCK);
    // push_lock = LOCK;
    // fprintf(stdout, "......... push_linkedQueue with data : %ld \n", data.pts);

    if (queue->size > 1 && queue->real_size >= queue->size) {
        __remove_linkedQueue(queue);
    }

    LinkedQueueNode *node = malloc(sizeof(LinkedQueueNode));
    node->data = copy_nodeData(data);
    node->next = NULL;

    queue->tail->next = node;
    queue->tail = node;
    queue->real_size += 1;

    // push_lock = UNLOCK;
    // pop_lock = UNLOCK;
    // fprintf(stdout, "......... queue size: %d \n", queue->real_size);
}

/**
 * 头出
 * @param queue
 * @return
 */
LinkedQueueNodeData pop_linkedQueue(LinkedQueue *queue) {
    // if (push_lock == LOCK && pop_lock == LOCK) pop_lock = UNLOCK;
    // while (pop_lock == LOCK);
    // pop_lock = LOCK;
    fprintf(stdout, "......... pop_linkedQueue 1 \n");

    LinkedQueueNodeData result = {
        .frame = NULL,
        .pts = 0L,
        .ret = 0,
        .error_message = NULL
    };

    if (queue->real_size == 0 || queue->head == queue->tail) return result;

    LinkedQueueNode *head = queue->head;
    if (!(head == NULL || head->next == NULL)) head = queue->head->next;

    if (head != NULL) {
        result = head->data;

        queue->head->next = head->next;

        if (queue->tail == head) queue->tail = queue->head;
        free(head);
        queue->real_size -= 1;

        fprintf(stdout, "......... after pop queue size: %d \n", queue->real_size);
        //push_lock = UNLOCK;
        // pop_lock = UNLOCK;
    }

    return result;
}
