﻿#include "./video2images.h"

/**
 * 连接视频地址，获取数据流
 * @param result @see Video2ImageStream
 * @param params: @see FFmpegInputParams
 **/
void open_inputfile(Video2ImageStream *result, FFmpegInputParams params) {

    av_log(NULL, AV_LOG_DEBUG, "...........\n");

    result->video_stream_idx = -1;

    result->video_stream = NULL;

    result->format_context = NULL;

    result->video_codec_context = NULL;

    int ret;

    AVDictionary *dictionary = NULL;

    av_log(NULL, AV_LOG_DEBUG, "input url rtsp addr %s \n", params.video_addr);
    int error = avformat_network_init();
    if (error != 0) {
        av_log(NULL, AV_LOG_ERROR, "network init error\n");
        release(result);
        result->ret = -1;
        return;
    }

    if (params.use_tcp) {
        if (av_dict_set(&dictionary, "rtsp_transport", "tcp", 0) < 0) {
            av_log(NULL, AV_LOG_ERROR, "set rtsp_transport to tcp error\n");
        }
    } else {
        if (av_dict_set(&dictionary, "rtsp_transport", "udp", 0) < 0) {
            av_log(NULL, AV_LOG_ERROR, "set rtsp_transport to udp error\n");
        } else {
            av_dict_set(&dictionary, "flush_packets", "1", 0);
        }
    }

    // 单位 微秒
    char str[12];
#ifdef _WIN32
#else
    sprintf(str, "%d", params.timeout * 500000);
    av_dict_set(&dictionary, "stimeout", str, 0);
#endif // _WIN32

    if (params.nobuffer) {
        av_dict_set(&dictionary, "fflags", "nobuffer", 0);
    } else {
        // 设置缓存大小
        av_dict_set(&dictionary, "buffer_size", "4096", 0);
        av_dict_set(&dictionary, "flush_packets", "1", 0);
        av_dict_set(&dictionary, "max_delay", "0", 0);
        av_dict_set(&dictionary, "rtbufsize", "4096", 0);
    }

    if (params.use_gpu) {
        if (av_dict_set(&dictionary, "hwaccel_device", "1", 0) < 0) {
            av_log(NULL, AV_LOG_ERROR, "no hwaccel device\n");
        }

        // 使用cuda
        if (av_dict_set(&dictionary, "hwaccel", "cuda", 0) < 0) {
            av_log(NULL, AV_LOG_ERROR, "cuda acceleration error\n");
        }

        // 使用 cuvid
        if (av_dict_set(&dictionary, "hwaccel", "cuvid", 0) < 0) {
            av_log(NULL, AV_LOG_ERROR, "cuvid acceleration error\n");
        }

        // 使用 opencl
        if (av_dict_set(&dictionary, "hwaccel", "opencl", 0) < 0) {
            av_log(NULL, AV_LOG_ERROR, "opencl acceleration error\n");
        }
    }

    if (av_dict_set(&dictionary, "allowed_media_types", "video", 0) < 0) {
        av_log(NULL, AV_LOG_ERROR, "set allowed_media_types to video error\n");
    }

    result->format_context = avformat_alloc_context();

    av_log(NULL, AV_LOG_DEBUG, "input file: %s\n", params.video_addr);
    // format_context必须初始化，否则报错
    if ((ret = avformat_open_input(&(result->format_context), params.video_addr, NULL, &dictionary)) != 0) {
        av_log(NULL, AV_LOG_ERROR, "Couldn't open file %s: %d\n", params.video_addr, ret);
        release(result);
        result->error_message = "avformat_open_input error, Couldn't open this file";
        result->ret = -2;
        return;
    }

    av_dict_free(&dictionary);
    result->format_context->max_delay = 1;

    if (avformat_find_stream_info(result->format_context, NULL) < 0) {
        av_log(NULL, AV_LOG_ERROR, "Cannot find stream information\n");
        release(result);
        result->error_message = "avformat_find_stream_info error";
        result->ret = -3;
        return;
    }

    result->video_stream_idx = av_find_best_stream(result->format_context, AVMEDIA_TYPE_VIDEO, -1, -1, NULL, 0);
    if (result->video_stream_idx < 0) {
        av_log(NULL, AV_LOG_ERROR, "Could not find %s stream in input file '%s'\n",
               av_get_media_type_string(AVMEDIA_TYPE_VIDEO), params.video_addr);
        release(result);
        result->error_message = "av_find_best_stream error";
        result->ret = -4;
        return;
    }

    result->video_stream = result->format_context->streams[result->video_stream_idx];

    AVCodec *codec = NULL;
    AVDictionary *opts = NULL;
    av_dict_set(&opts, "refcounted_frames", "false", 0);

    enum AVCodecID codec_id = result->video_stream->codecpar->codec_id;
    // 判断摄像头视频格式是h264还是h265
    if (params.use_gpu && codec_id == AV_CODEC_ID_H264) {
        codec = avcodec_find_decoder_by_name("h264_cuvid");
        char gpu[12];
#ifdef _WIN32
#else
        sprintf(gpu, "%d", params.gpu_id);
#endif
        av_dict_set(&opts, "gpu", gpu, 0);
    } else if (codec_id == AV_CODEC_ID_HEVC) {
        if (params.use_gpu)
            codec = avcodec_find_decoder_by_name("hevc_nvenc");
        av_dict_set(&opts, "flags", "low_delay", 0);
        char gpu[12];
#ifdef _WIN32
#else
        sprintf(gpu, "%d", params.gpu_id);
#endif
        av_dict_set(&opts, "gpu", gpu, 0);
    }
    if (codec == NULL)
        codec = avcodec_find_decoder(result->video_stream->codecpar->codec_id);
    if (!codec) {
        av_log(NULL, AV_LOG_ERROR, "Failed to find %s codec\n", av_get_media_type_string(AVMEDIA_TYPE_VIDEO));
        release(result);
        result->error_message = "avcodec_find_decoder error";
        result->ret = -5;
        return;
    }

    // 每秒多少帧
    result->frame_rate = result->video_stream->r_frame_rate.num / result->video_stream->r_frame_rate.den;
    av_log(NULL, AV_LOG_DEBUG, "input video frame rate: %d\n", result->frame_rate);

    result->parser_context = av_parser_init(codec->id);
    if (!result->parser_context) {
        av_log(NULL, AV_LOG_ERROR, "parser not found\n");
        release(result);
        result->error_message = "parser not found";
        result->ret = -6;
        return;
    }

    result->video_codec_context = avcodec_alloc_context3(codec);
    if (!result->video_codec_context) {
        av_log(NULL, AV_LOG_ERROR, "Failed to allocate the %s codec context\n",
               av_get_media_type_string(AVMEDIA_TYPE_VIDEO));
               
        release(result);
        result->error_message = "avcodec_alloc_context3 error";
        result->ret = -7;
        return;
    }

    if ((ret = avcodec_parameters_to_context(result->video_codec_context, result->video_stream->codecpar)) < 0) {
        av_log(NULL, AV_LOG_ERROR, "Failed to copy %s codec parameters to decoder context\n",
               av_get_media_type_string(AVMEDIA_TYPE_VIDEO));
        release(result);
        result->error_message = "avcodec_parameters_to_context error";
        result->ret = -8;
        return;
    }

    if (avcodec_open2(result->video_codec_context, codec, &opts) < 0) {
        av_log(NULL, AV_LOG_WARNING, "Failed to open %s codec\n", av_get_media_type_string(AVMEDIA_TYPE_VIDEO));

        // 在GPU docker 运行出错后使用CPU
        // =======================================================================================================
        codec = avcodec_find_decoder(result->video_stream->codecpar->codec_id);
        if (!codec) {
            av_log(NULL, AV_LOG_ERROR, "Failed to find %s codec\n", av_get_media_type_string(AVMEDIA_TYPE_VIDEO));
            release(result);
            result->error_message = "avcodec_find_decoder error";
            result->ret = -5;
            return;
        }

        result->parser_context = av_parser_init(codec->id);
        if (!result->parser_context) {
            av_log(NULL, AV_LOG_ERROR, "parser not found\n");
            release(result);
            result->error_message = "parser not found";
            result->ret = -6;
            return;
        }

        avcodec_free_context(&(result->video_codec_context));
        result->video_codec_context = avcodec_alloc_context3(codec);
        if (!(result->video_codec_context)) {
            av_log(NULL, AV_LOG_ERROR, "Failed to allocate the %s codec context\n",
                   av_get_media_type_string(AVMEDIA_TYPE_VIDEO));
            release(result);
            result->error_message = "avcodec_alloc_context3 error";
            result->ret = -7;
            return;
        }

        if ((ret = avcodec_parameters_to_context(result->video_codec_context, result->video_stream->codecpar)) < 0) {
            av_log(NULL, AV_LOG_ERROR, "Failed to copy %s codec parameters to decoder context\n",
                   av_get_media_type_string(AVMEDIA_TYPE_VIDEO));
            release(result);
            result->error_message = "avcodec_parameters_to_context error";
            result->ret = -8;
            return;
        }

        if (avcodec_open2(result->video_codec_context, codec, &opts) < 0) {
            av_log(NULL, AV_LOG_ERROR, "Failed to open %s codec\n", av_get_media_type_string(AVMEDIA_TYPE_VIDEO));
            release(result);
            result->error_message = "Failed to open codec";
            result->ret = -9;
            return;
        }
        // =======================================================================================================
    }

    if (!(result->video_stream)) {
        av_log(NULL, AV_LOG_ERROR, "Could not find audio or video stream in the input, aborting\n");
        release(result);
        result->error_message = "Could not find audio or video stream in the input";
        result->ret = -10;
        return;
    }

    result->ret = 0;
    result->error_message = NULL;

    av_log(NULL, AV_LOG_DEBUG, "init finished ...... \n");
}

/**
 * 释放内存
 */
void release(Video2ImageStream *vis) {
	if (vis != NULL) {
		av_log(NULL, AV_LOG_DEBUG, "---> 3 ---> free memory\n");

		av_log(NULL, AV_LOG_DEBUG, "pointer %p \n", vis);

		int ret;
		if (vis->video_codec_context != NULL) {
			av_log(NULL, AV_LOG_DEBUG, "avcodec_close ... \n");
			ret = avcodec_close(vis->video_codec_context);
			if (ret < 0) {
				av_log(NULL, AV_LOG_ERROR, "avcodec_close error \n");
			}
		}

		if (vis->video_codec_context != NULL) {
			av_log(NULL, AV_LOG_DEBUG, "avcodec_free_context ... \n");
			avcodec_free_context(&(vis->video_codec_context));
			vis->video_codec_context = NULL;
		}

		if (vis->format_context != NULL) {
			av_log(NULL, AV_LOG_DEBUG, "avformat_close_input ... \n");
			avformat_close_input(&(vis->format_context));
			avformat_free_context(vis->format_context);
			vis->format_context = NULL;
		}


		av_log(NULL, AV_LOG_DEBUG, "avformat_network_deinit ... \n");
		ret = avformat_network_deinit();
		if (ret < 0) {
			av_log(NULL, AV_LOG_ERROR, "avformat_network_deinit error \n");
		}

		free(vis);
		vis = NULL;
	}
}

static void video2images_grab_close(AVFrame *frame, AVPacket *packet) {
    if (frame != NULL) {
        av_frame_unref(frame);
        av_frame_free(&frame);
        av_free(frame);
        frame = NULL;
    }

    if (packet != NULL) {
        av_packet_unref(packet);
        av_packet_free(&packet);
        av_free(packet);
        packet = NULL;
    }
}

static int pts = 0;
static int hadHasFrames = false;
static int retry = 0;

/**
 * 视频流获取 图片帧
 * @param vis: 存储在全局变量中的视频流数据变量
 * @param quality: jpeg 图片质量，1~100
 * @param chose_frames: 一秒取多少帧, 超过视频帧率则为视频帧率
 * @param type: 图片类型 @see ImageStreamType
 * @param callback: callback function @see Video2ImagesCallback
 * @param result [out] @see FrameData 
 **/
void video2images_grab(Video2ImageStream *vis, int quality, int chose_frames, enum ImageStreamType type,
                                Video2ImagesCallback callback, FrameData *result) {
    long pts_time = 0;

    int ret;

    int times = 0;
    int c = 0;
    long check;

    int _ret = 0;

    AVFrame* frame;
    AVPacket* orig_pkt;

    frame = av_frame_alloc();
    if (!frame) {
        av_log(NULL, AV_LOG_ERROR, "Could not allocate image frame\n");
        result->ret = -11;
        result->error_message = "Could not allocate image frame";
        return;
    }

    orig_pkt = av_packet_alloc();
    if (!orig_pkt) {
        av_log(NULL, AV_LOG_ERROR, "Couldn't alloc packet\n");
        result->ret = -12;
        result->error_message = "Couldn't alloc packet";
        video2images_grab_close(frame, orig_pkt);
        return;
    }

    /* read frames from the file */
    while (av_read_frame(vis->format_context, orig_pkt) >= 0) {
        if (orig_pkt->stream_index == vis->video_stream_idx) {
            if (orig_pkt->flags & AV_PKT_FLAG_KEY) {
                av_log(NULL, AV_LOG_DEBUG, "key frame\n");
                if (orig_pkt->pts < 0) {
                    pts = 0;
                }
            }
            if (vis->video_stream == NULL) {
                av_log(NULL, AV_LOG_ERROR, "error: video stream is null\n");
                result->ret = -13;
                result->error_message = "video stream is null";
                video2images_grab_close(frame, orig_pkt);
                return;
            }

            // 获取帧数
            if (orig_pkt->pts >= 0)
                pts_time = (long)(orig_pkt->pts * av_q2d(vis->video_stream->time_base) * vis->frame_rate);
            else
                pts_time = pts++;

            ret = avcodec_send_packet(vis->video_codec_context, orig_pkt);
            if (ret < 0) {
                if (!hadHasFrames) {
                    if (retry < 5) {
                        retry += 1;
                        av_log(NULL, AV_LOG_INFO, "retry times: %d \n", retry);
                        continue;
                    }
                } else if (times < vis->frame_rate) {
                    times += 1;
                    av_log(NULL, AV_LOG_INFO, "redecode times: %d \n", retry);
                    continue;
                }

                av_log(NULL, AV_LOG_ERROR, "Error while sending a packet to the decoder\n");
                video2images_grab_close(frame, orig_pkt);
                result->ret = -14;
                result->error_message = "Error while sending a packet to the decoder";
                return;
            }

            chose_frames = chose_frames > vis->frame_rate ? vis->frame_rate : chose_frames;
            c = vis->frame_rate / chose_frames;
            av_log(NULL, AV_LOG_DEBUG, "frame_rate %d chose_frames %d c %d\n", vis->frame_rate, chose_frames ,c);
            check = pts_time % c;
            av_log(NULL, AV_LOG_DEBUG, "check %ld\n", check);

            ret = avcodec_receive_frame(vis->video_codec_context, frame);
            // 解码一帧数据
            if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
                av_log(NULL, AV_LOG_DEBUG, "Decode finished\n");
                continue;
            }
            if (ret < 0) {
                av_log(NULL, AV_LOG_ERROR, "Decode error\n");
                video2images_grab_close(frame, orig_pkt);
                result->ret = -15;
                result->error_message = "Error while receive frame from a packet";
                return;
            }

            hadHasFrames = true;
            times = 0;

            av_log(NULL, AV_LOG_DEBUG, "pts_time: %ld chose_frames: %d frame_rate: %d\n", pts_time,
                    chose_frames, vis->frame_rate);
            // 判断帧数，是否取
            if (check == c - 1) {
                result->frame = frame;
                result->pts = pts_time;
                result->type = type;
                result->quality = quality;
                result->ret = 0;

                av_log(NULL, AV_LOG_DEBUG, "result.pts %d \n", result->pts);

                if (callback != NULL) {
                    av_log(NULL, AV_LOG_DEBUG, "vis->video_codec_context %p \n", vis->video_codec_context);
                    callback((const AVCodecContext *) vis->video_codec_context, result);
                    av_log(NULL, AV_LOG_DEBUG, "result->isThreadly %d \n", result->isThreadly);
                    if (!result->isThreadly) {
                        break;
                    }
                }
            }

#ifdef _WIN32
            Sleep(0);
#else
            usleep(0);
#endif // _WIN32

			if (orig_pkt != NULL)
				av_packet_unref(orig_pkt);
        }
    }

    video2images_grab_close(frame, orig_pkt);
}
