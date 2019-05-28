#include "./com_truthbean_jni_ffmpeg_FFmpegJava.h"

/*
 * Class:     com_truthbean_jni_ffmpeg_FFmpegJava
 * Method:    init
 * Signature: (Lcom/truthbean/ffmpeg/FFmpegJava;Lcom/truthbean/ffmpeg/model/FFmpegInputParams;Lcom/truthbean/ffmpeg/model/VideoConnectedResult;)
 */
JNIEXPORT void JNICALL Java_com_truthbean_ffmpeg_FFmpegJava_init(JNIEnv *env, jobject obj, jobject handle, jobject params, jobject result)
{
    av_log(NULL, AV_LOG_DEBUG, "Java_com_truthbean_ffmpeg_FFmpegJava_init start \n");
	Video2ImageStream* vis = malloc(sizeof(Video2ImageStream));

    jclass params_class = (*env)->GetObjectClass(env, params);
    // log level
    jmethodID level_id = (*env)->GetMethodID(env, params_class, "getLevel", "()I");
    jint level = (*env)->CallIntMethod(env, params, level_id);
    switch (level)
    {
        case 0:
            av_log_set_level(AV_LOG_ERROR);
            av_log(NULL, AV_LOG_FATAL, "set av_log level AV_LOG_ERROR \n");
            break;
        case 1:
            av_log_set_level(AV_LOG_DEBUG);
            av_log(NULL, AV_LOG_INFO, "set av_log level AV_LOG_DEBUG \n");
            break;
        case 2:
        default:
            av_log_set_level(AV_LOG_INFO);
            av_log(NULL, AV_LOG_INFO, "set av_log level AV_LOG_INFO \n");
            break;
    }

    // addr
    jmethodID addr_id = (*env)->GetMethodID(env, params_class, "getVideoAddr", "()Ljava/lang/String;");
    jstring addr = (jstring) (*env)->CallObjectMethod(env, params, addr_id);
    jboolean _true = JNI_TRUE;
    const char *addr_chars = (*env)->GetStringUTFChars(env, addr, &_true);
    av_log(NULL, AV_LOG_DEBUG, "Java_com_truthbean_ffmpeg_FFmpegJava_init --> rtps addr %s \n", addr_chars);

    // nobuffer
    jmethodID nobuffer_id = (*env)->GetMethodID(env, params_class, "isNobuffer", "()Z");
    jboolean nobuffer = (*env)->CallBooleanMethod(env, params, nobuffer_id);
    av_log(NULL, AV_LOG_DEBUG, "Java_com_truthbean_ffmpeg_FFmpegJava_init --> nobuffer %d \n", nobuffer);
    bool nb = nobuffer ? true : false;

    // useGpu
    jmethodID useGpu_id = (*env)->GetMethodID(env, params_class, "isUseGpu", "()Z");
    jboolean useGpu = (*env)->CallBooleanMethod(env, params, useGpu_id);
    av_log(NULL, AV_LOG_DEBUG, "Java_com_truthbean_ffmpeg_FFmpegJava_init --> useGpu %d \n", useGpu);
    bool use_gpu = useGpu ? true : false;

    // useTcp
    jmethodID useTcp_id = (*env)->GetMethodID(env, params_class, "isUseTcp", "()Z");
    jboolean useTcp = (*env)->CallBooleanMethod(env, params, useTcp_id);
    av_log(NULL, AV_LOG_DEBUG, "Java_com_truthbean_ffmpeg_FFmpegJava_init --> useTcp %d \n", useTcp);
    bool use_tcp = useTcp ? true : false;

    // gpuId
    jmethodID gpuId_id = (*env)->GetMethodID(env, params_class, "getGpuId", "()I");
    jint gpuId = (*env)->CallIntMethod(env, params, gpuId_id);
    av_log(NULL, AV_LOG_DEBUG, "Java_com_truthbean_ffmpeg_FFmpegJava_init --> gpuId %d \n", gpuId);
    char gpu_id[12];

    // timeout
    jmethodID timeout_id = (*env)->GetMethodID(env, params_class, "getTimeout", "()I");
    jint timeout = (*env)->CallIntMethod(env, params, timeout_id);
    av_log(NULL, AV_LOG_DEBUG, "Java_com_truthbean_ffmpeg_FFmpegJava_init --> timeout %d \n", timeout);

    FFmpegInputParams _params = {
        .video_addr = addr_chars,
        .timeout = timeout,
        .nobuffer = nb,
        .use_gpu = use_gpu,
        .gpu_id = gpu_id,
        .use_tcp = use_tcp
    };

    // 获取视频流数据
    open_inputfile(vis, _params);
    av_log(NULL, AV_LOG_DEBUG, "Java_com_truthbean_ffmpeg_FFmpegJava_init --> vis->frame_rate %d \n", vis->frame_rate);

    jlong _pointer = (jlong)vis;
    av_log(NULL, AV_LOG_DEBUG, "Java_com_truthbean_ffmpeg_FFmpegJava_init --> _pointer %ld \n", _pointer);
    jclass handle_class = (*env)->GetObjectClass(env, handle);

    jmethodID pointer_id = (*env)->GetMethodID(env, handle_class, "setVisPointer", "(J)V");
    (*env)->CallVoidMethod(env, handle, pointer_id, _pointer);
    av_log(NULL, AV_LOG_DEBUG, "Java_com_truthbean_ffmpeg_FFmpegJava_init --> _pointer %ld \n", _pointer);

    jclass result_class = (*env)->GetObjectClass(env, result);

    // code
    jmethodID code_id = (*env)->GetMethodID(env, result_class, "setCode", "(I)V");
    (*env)->CallVoidMethod(env, result, code_id, vis->ret);
    av_log(NULL, AV_LOG_DEBUG, "Java_com_truthbean_ffmpeg_FFmpegJava_init --> vis->ret %d \n", vis->ret);

    // frameRate
    jmethodID frameRate_id = (*env)->GetMethodID(env, result_class, "setFrameRate", "(I)V");
    (*env)->CallVoidMethod(env, result, frameRate_id, vis->frame_rate);
    av_log(NULL, AV_LOG_DEBUG, "Java_com_truthbean_ffmpeg_FFmpegJava_init --> vis->frame_rate %d \n", vis->frame_rate);

    // message
    jmethodID message_id = (*env)->GetMethodID(env, result_class, "setMessage", "(Ljava/lang/String;)V");

    jclass strClass = (*env)->FindClass(env, "Ljava/lang/String;");
    jmethodID str_constructor = (*env)->GetMethodID(env, strClass, "<init>", "([B)V");

    char *_message = "success";
    if (vis->error_message != NULL) {
        av_log(NULL, AV_LOG_DEBUG, "Java_com_truthbean_ffmpeg_FFmpegJava_init --> vis->error_message %s \n", vis->error_message);
        _message = vis->error_message;
    }

    jsize _message_len = strlen(_message);
    jbyteArray bytes = (*env)->NewByteArray(env, (jsize)_message_len);
    av_log(NULL, AV_LOG_DEBUG, "Java_com_truthbean_ffmpeg_FFmpegJava_init --> _message %s \n", _message);
    (*env)->SetByteArrayRegion(env, bytes, 0, (jsize)strlen((const char *)_message), (jbyte*)(const char *)_message);

    jstring message = (jstring) (*env)->NewObject(env, strClass, str_constructor, bytes);
    (*env)->CallVoidMethod(env, result, message_id, message);

    (*env)->DeleteLocalRef(env, message);
    (*env)->DeleteLocalRef(env, bytes);
    (*env)->ReleaseStringUTFChars(env, addr, addr_chars);
    av_log(NULL, AV_LOG_DEBUG, "Java_com_truthbean_ffmpeg_FFmpegJava_init finish \n");

#ifndef _WIN32
    free(vis);
#endif // !_WIN32

}

static void callback_frame(const AVCodecContext *video_codec_context, FrameData *data) 
{
    av_log(NULL, AV_LOG_DEBUG, "callback_frame --> video_codec_context .... %p \n", video_codec_context);
    if (data->ret == 0 && data->frame != NULL) {
        if (data->type == JPEG)
            copy_frame_data_and_transform_to_jpeg(video_codec_context, data);
        else
            copy_frame_raw_data(video_codec_context, data);
        
        av_log(NULL, AV_LOG_DEBUG, "file_size: %ld\n", data->file_size);
        if (data->file_size == 0 || data->file_data == NULL) {
            av_log(NULL, AV_LOG_DEBUG, "file_data NULL\n");
        } else {
            if (data->ret < 0 ) {
                data->ret = -40;
                data->error_message = "image data is null";
            }
        }
        if (!data->isThreadly && data->frame != NULL) {
            data->frame = NULL;
        }
    }
}

// ================================================================================================================================
#ifdef _WIN32

#else

static JavaVM *g_jvm;

static FrameData *share_data;

static sem_t semaphore;

static void grab_image(void *data) {
    // 等待工作
    while(true) {
        sem_wait(&semaphore);
        if (data != NULL) {
            JNIEnv *env;
            //获取当前native线程是否有没有被附加到jvm环境中
            int env_stat  = (*g_jvm)->GetEnv(g_jvm, (void **)&env, JNI_VERSION_1_8);

            bool detach = false;
            if (env_stat == JNI_EDETACHED) {
                //如果没有， 主动附加到jvm环境中，获取到env
                if ((*g_jvm)->AttachCurrentThread(g_jvm, &env, NULL) != 0) {
                    return;
                }
                detach = true;
            }
            jobject callback = (jobject) data;

            //通过强转后的jcallback 获取到要回调的类
            jclass callback_class = (*env)->GetObjectClass(env, callback);

            if (callback_class == NULL) {
                av_log(NULL, AV_LOG_ERROR, "grab_image --> Unable to find callback class");
                (*g_jvm)->DetachCurrentThread(g_jvm);
                return;
            }

            jmethodID grab_method = (*env)->GetMethodID(env, callback_class, "grab", "(Lcom/truthbean/ffmpeg/model/ImageBuffer;)V");

            jclass result_class = (*env)->FindClass(env, "com/truthbean/ffmpeg/model/ImageBuffer");
            jmethodID result_constructor = (*env)->GetMethodID(env, result_class, "<init>", "()V");
            jobject result = (*env)->NewObject(env, result_class, result_constructor);

            if (share_data != NULL) {
                // code
                jmethodID code_id = (*env)->GetMethodID(env, result_class, "setCode", "(I)V");
                (*env)->CallVoidMethod(env, result, code_id, share_data->ret);
                av_log(NULL, AV_LOG_DEBUG, "Java_com_truthbean_ffmpeg_FFmpegJava_getImageBuffer --> share_data->ret %d \n", share_data->ret);

                if (share_data->ret == 0) {
                    // size
                    jmethodID size_id = (*env)->GetMethodID(env, result_class, "setSize", "(J)V");
                    (*env)->CallVoidMethod(env, result, size_id, share_data->file_size);

                    if (share_data->file_size > 0) {
                        // data
                        jmethodID data_id = (*env)->GetMethodID(env, result_class, "setData", "([B)V");
                        jbyteArray _data_array = (*env)->NewByteArray(env, (jsize) share_data->file_size + 1);
                        int i = 0;
                        (*env)->SetByteArrayRegion(env, _data_array, 0, (jsize) share_data->file_size + 1, (const jbyte *) share_data->file_data);
                        (*env)->CallVoidMethod(env, result, data_id, _data_array);

                        // 释放 图片数据 的 内存
                        av_freep(&share_data->file_data);
                        free(share_data->file_data);
                        share_data->file_data = NULL;

                        (*env)->DeleteLocalRef(env, _data_array);
                    }
                }

                // message
                jmethodID message_id = (*env)->GetMethodID(env, result_class, "setMessage", "(Ljava/lang/String;)V");

                jclass strClass = (*env)->FindClass(env, "Ljava/lang/String;");
                jmethodID str_constructor = (*env)->GetMethodID(env, strClass, "<init>", "([B)V");

                char *_message = "success";
                if (share_data->error_message != NULL) {
                    av_log(NULL, AV_LOG_DEBUG, "Java_com_truthbean_ffmpeg_FFmpegJava_getImageBuffer --> vis->error_message %s \n", share_data->error_message);
                    _message = share_data->error_message;
                }

                jsize _message_len = strlen(_message);
                jbyteArray bytes = (*env)->NewByteArray(env, (jsize)_message_len);
                av_log(NULL, AV_LOG_DEBUG, "Java_com_truthbean_ffmpeg_FFmpegJava_getImageBuffer --> _message %s \n", _message);
                (*env)->SetByteArrayRegion(env, bytes, 0, (jsize)strlen((const char *)_message), (jbyte*)(const char *)_message);

                jstring message = (jstring) (*env)->NewObject(env, strClass, str_constructor, bytes);
                (*env)->CallVoidMethod(env, result, message_id, message);

                (*env)->DeleteLocalRef(env, bytes);
                (*env)->DeleteLocalRef(env, message);

            }

            (*env)->CallVoidMethod(env, callback, grab_method, result);

            //释放当前线程
            if(detach) {
                (*g_jvm)->DetachCurrentThread(g_jvm);
            }

            //释放你的全局引用的接口，生命周期自己把控
            // (*env)->DeleteGlobalRef(env, callback);
            // callback = NULL;

            env = NULL;
        }
    }

}

static void callback_frame_threadly(const AVCodecContext *video_codec_context, FrameData *data) 
{
    av_log(NULL, AV_LOG_DEBUG, "callback_frame --> video_codec_context .... %p \n", video_codec_context);
    if (data->ret == 0 && data->frame != NULL) {
        if (data->type == JPEG)
            copy_frame_data_and_transform_to_jpeg(video_codec_context, data);
        else
            copy_frame_raw_data(video_codec_context, data);

        av_log(NULL, AV_LOG_DEBUG, "file_size: %ld\n", data->file_size);
        if (data->file_size == 0 || data->file_data == NULL) {
            av_log(NULL, AV_LOG_DEBUG, "file_data NULL\n");
        } else {
            if (data->ret < 0 ) {
                data->ret = -40;
                data->error_message = "image data is null";
            }
        }
        if (!data->isThreadly && data->frame != NULL) {
            data->frame = NULL;
        }
    }

    share_data = data;

    sem_post(&semaphore);
}

JNIEXPORT void JNICALL Java_com_truthbean_ffmpeg_FFmpegJava_getImageBufferThreadly(JNIEnv *env, jobject obj, jobject handle, jobject params, jobject listener)
{

    //JavaVM是虚拟机在JNI中的表示，等下再其他线程回调java层需要用到
    (*env)->GetJavaVM(env, &g_jvm);

    //生成一个全局引用，回调的时候findclass才不会为null
    jobject callback = (*env)->NewGlobalRef(env, listener);

    // `````````````````````````````````````````````````````````````````````````````````````````````````````````

    av_log(NULL, AV_LOG_DEBUG, "Java_com_truthbean_ffmpeg_FFmpegJava_getImageBuffer start \n");
    // visPointer
    jclass handle_class = (*env)->GetObjectClass(env, handle);
    jmethodID handle_id = (*env)->GetMethodID(env, handle_class, "getVisPointer", "()J");
    jlong pointer = (*env)->CallLongMethod(env, handle, handle_id);

    jclass params_class = (*env)->GetObjectClass(env, params);
    // quality
    jmethodID quality_id = (*env)->GetMethodID(env, params_class, "getQuality", "()I");
    jint quality = (*env)->CallIntMethod(env, params, quality_id);
    av_log(NULL, AV_LOG_DEBUG, "Java_com_truthbean_ffmpeg_FFmpegJava_getImageBuffer --> quality %d \n", quality);

    // frames
    jmethodID frames_id = (*env)->GetMethodID(env, params_class, "getFrames", "()I");
    jint frames = (*env)->CallIntMethod(env, params, frames_id);
    av_log(NULL, AV_LOG_DEBUG, "Java_com_truthbean_ffmpeg_FFmpegJava_getImageBuffer --> frames %d \n", frames);

    // type
    jmethodID type_id = (*env)->GetMethodID(env, params_class, "getType", "()I");
    jint type = (*env)->CallIntMethod(env, params, type_id);
    av_log(NULL, AV_LOG_DEBUG, "Java_com_truthbean_ffmpeg_FFmpegJava_getImageBuffer --> type %d \n", type);

    Video2ImageStream *vis = (Video2ImageStream *)pointer;
    FrameData frameData = {
        .isThreadly = true
    };

    // `````````````````````````````````````````````````````````````````````````````````````````````````````````

    sem_init(&semaphore, 0, 0);

    // 把接口传进去，或者保存在一个结构体里面的属性， 进行传递也可以
    pthread_t callback_id;
    pthread_create(callback_id, NULL, grab_image, callback);

    video2images_grab(vis, quality, frames, type, callback_frame_threadly, &frameData);
    av_log(NULL, AV_LOG_DEBUG, "Java_com_truthbean_ffmpeg_FFmpegJava_getImageBuffer --> frameData.file_size %d \n", frameData.file_size);
}

#endif // _WIN32

// ================================================================================================================================

/*
 * Class:     com_truthbean_jni_ffmpeg_FFmpegJava
 * Method:    getImageBuffer
 * Signature: (Lcom/truthbean/ffmpeg/FFmpegJava;Lcom/truthbean/ffmpeg/model/ImageBufferParams;Lcom/truthbean/ffmpeg/model/ImageBuffer;)
 */
JNIEXPORT void JNICALL Java_com_truthbean_ffmpeg_FFmpegJava_getImageBuffer(JNIEnv *env, jobject obj, jobject handle, jobject params, jobject result)
{
    av_log(NULL, AV_LOG_DEBUG, "Java_com_truthbean_ffmpeg_FFmpegJava_getImageBuffer start \n");
    // visPointer
    jclass handle_class = (*env)->GetObjectClass(env, handle);
    jmethodID handle_id = (*env)->GetMethodID(env, handle_class, "getVisPointer", "()J");
    jlong pointer = (*env)->CallLongMethod(env, handle, handle_id);

    jclass params_class = (*env)->GetObjectClass(env, params);
    // quality
    jmethodID quality_id = (*env)->GetMethodID(env, params_class, "getQuality", "()I");
    jint quality = (*env)->CallIntMethod(env, params, quality_id);
    av_log(NULL, AV_LOG_DEBUG, "Java_com_truthbean_ffmpeg_FFmpegJava_getImageBuffer --> quality %d \n", quality);

    // frames
    jmethodID frames_id = (*env)->GetMethodID(env, params_class, "getFrames", "()I");
    jint frames = (*env)->CallIntMethod(env, params, frames_id);
    av_log(NULL, AV_LOG_DEBUG, "Java_com_truthbean_ffmpeg_FFmpegJava_getImageBuffer --> frames %d \n", frames);

    // type
    jmethodID type_id = (*env)->GetMethodID(env, params_class, "getType", "()I");
    jint type = (*env)->CallIntMethod(env, params, type_id);
    av_log(NULL, AV_LOG_DEBUG, "Java_com_truthbean_ffmpeg_FFmpegJava_getImageBuffer --> type %d \n", type);

    Video2ImageStream *vis = (Video2ImageStream *)pointer;
	av_log(NULL, AV_LOG_DEBUG, "Java_com_truthbean_ffmpeg_FFmpegJava_getImageBuffer --> _pointer %ld \n", vis);
	av_log(NULL, AV_LOG_DEBUG, "Java_com_truthbean_ffmpeg_FFmpegJava_getImageBuffer --> frame_rate %ld \n", vis->frame_rate);
    FrameData frameData = {
        .isThreadly = false
    };
    video2images_grab(vis, quality, frames, type, callback_frame, &frameData);
    av_log(NULL, AV_LOG_DEBUG, "Java_com_truthbean_ffmpeg_FFmpegJava_getImageBuffer --> frameData.file_size %d \n", frameData.file_size);

    jclass result_class = (*env)->GetObjectClass(env, result);

    // code
    jmethodID code_id = (*env)->GetMethodID(env, result_class, "setCode", "(I)V");
    (*env)->CallVoidMethod(env, result, code_id, frameData.ret);
    av_log(NULL, AV_LOG_DEBUG, "Java_com_truthbean_ffmpeg_FFmpegJava_getImageBuffer --> frameData.ret %d \n", frameData.ret);

    if (frameData.ret == 0) {
        // size
        jmethodID size_id = (*env)->GetMethodID(env, result_class, "setSize", "(J)V");
        (*env)->CallVoidMethod(env, result, size_id, frameData.file_size);

        if (frameData.file_size > 0) {
            // data
            jmethodID data_id = (*env)->GetMethodID(env, result_class, "setData", "([B)V");
            jbyteArray _data_array = (*env)->NewByteArray(env, (jsize) frameData.file_size);
            int i = 0;
            (*env)->SetByteArrayRegion(env, _data_array, 0, (jsize) frameData.file_size, (const jbyte *) frameData.file_data);
            (*env)->CallVoidMethod(env, result, data_id, _data_array);

			// 释放 图片数据 的 内存
#ifndef _WIN32
			av_freep(&frameData.file_data);
			free(frameData.file_data);
#endif // _WIN32
			frameData.file_data = NULL;

            (*env)->DeleteLocalRef(env, _data_array);
        }
    }

    // message
    jmethodID message_id = (*env)->GetMethodID(env, result_class, "setMessage", "(Ljava/lang/String;)V");

    jclass strClass = (*env)->FindClass(env, "Ljava/lang/String;");
    jmethodID str_constructor = (*env)->GetMethodID(env, strClass, "<init>", "([B)V");

    char *_message = "success";
    if (frameData.error_message != NULL) {
        av_log(NULL, AV_LOG_DEBUG, "Java_com_truthbean_ffmpeg_FFmpegJava_getImageBuffer --> vis->error_message %s \n", frameData.error_message);
        _message = frameData.error_message;
    }

    jsize _message_len = strlen(_message);
    jbyteArray bytes = (*env)->NewByteArray(env, (jsize)_message_len);
    av_log(NULL, AV_LOG_DEBUG, "Java_com_truthbean_ffmpeg_FFmpegJava_getImageBuffer --> _message %s \n", _message);
    (*env)->SetByteArrayRegion(env, bytes, 0, (jsize)strlen((const char *)_message), (jbyte*)(const char *)_message);

    jstring message = (jstring) (*env)->NewObject(env, strClass, str_constructor, bytes);
    (*env)->CallVoidMethod(env, result, message_id, message);

    (*env)->DeleteLocalRef(env, bytes);
    (*env)->DeleteLocalRef(env, message);
    av_log(NULL, AV_LOG_DEBUG, "Java_com_truthbean_ffmpeg_FFmpegJava_getImageBuffer finish \n");
}

/*
 * Class:     com_truthbean_jni_ffmpeg_FFmpegJava
 * Method:    destroy
 * Signature: (Lcom/truthbean/ffmpeg/model/FFmpegInputParams;)V
 */
JNIEXPORT void JNICALL Java_com_truthbean_ffmpeg_FFmpegJava_destroy(JNIEnv *env, jobject obj, jobject handle)
{
    av_log(NULL, AV_LOG_DEBUG, "Java_com_truthbean_ffmpeg_FFmpegJava_destroy start \n");
    // visPointer
    jclass handle_class = (*env)->GetObjectClass(env, handle);
    jmethodID handle_id = (*env)->GetMethodID(env, handle_class, "getVisPointer", "()J");
    jlong pointer = (*env)->CallLongMethod(env, handle, handle_id);
    Video2ImageStream *vis = (Video2ImageStream *)pointer;
    av_log(NULL, AV_LOG_DEBUG, "Java_com_truthbean_ffmpeg_FFmpegJava_destroy --> pointer %p \n", vis);
    release(vis);
    av_log(NULL, AV_LOG_DEBUG, "Java_com_truthbean_ffmpeg_FFmpegJava_destroy finished \n");
}

/*
 * Class:     com_truthbean_jni_ffmpeg_FFmpegJava
 * Method:    record
 * Signature: (Ljava/lang/String;Ljava/lang/String;IZ)V
 */
JNIEXPORT void JNICALL Java_com_truthbean_ffmpeg_FFmpegJava_record(JNIEnv *env, jobject obj, jstring inputAddr,
                                                                       jstring outputAddr, jint recordSeconds, jboolean useGpu)
{
    av_log(NULL, AV_LOG_DEBUG, "Java_com_truthbean_ffmpeg_FFmpegJava_record start \n");
    jboolean _true = JNI_TRUE;
    const char *input_addr = (*env)->GetStringUTFChars(env, inputAddr, &_true);
    const char *output_addr = (*env)->GetStringUTFChars(env, outputAddr, &_true);
    bool use_gpu = useGpu ? true : false;
    int result = record_rtsp(input_addr, output_addr, recordSeconds, use_gpu);
    av_log(NULL, AV_LOG_DEBUG, "Java_com_truthbean_ffmpeg_FFmpegJava_record --> record rtsp result : %d\n", result);
    (*env)->ReleaseStringUTFChars(env, inputAddr, input_addr);
    (*env)->ReleaseStringUTFChars(env, outputAddr, output_addr);
    av_log(NULL, AV_LOG_DEBUG, "Java_com_truthbean_ffmpeg_FFmpegJava_record finish \n");
}