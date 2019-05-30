/* Header for class com_truthbean_jni_ffmpeg_FFmpegJava */
#ifndef _Included_com_truthbean_jni_ffmpeg_FFmpegJava
#define _Included_com_truthbean_jni_ffmpeg_FFmpegJava

#include <string.h>
#include <stdlib.h>
#ifdef _WIN32
#include <malloc.h>
#else

#include <pthread.h>
#include <semaphore.h>

#endif // _WIN32

#include <jni.h>

#include "./common.h"
#include "./video2images.h"
#include "./record_rtsp.h"

/*
 * Class:     com_truthbean_jni_ffmpeg_FFmpegJava
 * Method:    init
 * Signature: (Lcom/truthbean/ffmpeg/FFmpegJava;Lcom/truthbean/ffmpeg/model/FFmpegInputParams;Lcom/truthbean/ffmpeg/model/VideoConnectedResult;)V
 */
JNIEXPORT void JNICALL Java_com_truthbean_ffmpeg_FFmpegJava_init(JNIEnv *env, jobject obj, jobject handle, jobject params, jobject result);

/*
 * Class:     com_truthbean_jni_ffmpeg_FFmpegJava
 * Method:    getImageBuffer
 * Signature: (Lcom/truthbean/ffmpeg/FFmpegJava;Lcom/truthbean/ffmpeg/model/ImageBufferParams;Lcom/truthbean/ffmpeg/model/ImageBuffer;)V
 */
JNIEXPORT void JNICALL Java_com_truthbean_ffmpeg_FFmpegJava_getImageBuffer(JNIEnv *env, jobject obj, jobject handle, jobject params, jobject result);

/*
 * Class:     com_truthbean_jni_ffmpeg_FFmpegJava
 * Method:    getImageBufferThreadly
 * Signature: (Lcom/truthbean/ffmpeg/FFmpegJava;Lcom/truthbean/ffmpeg/model/ImageBufferParams;Lcom/truthbean/ffmpeg/GrabListener;)V
 */
JNIEXPORT void JNICALL Java_com_truthbean_ffmpeg_FFmpegJava_getImageBufferThreadly(JNIEnv *env, jobject obj, jobject handle, jobject params, jobject listener);

/*
 * Class:     com_truthbean_jni_ffmpeg_FFmpegJava
 * Method:    destroy
 * Signature: (Lcom/truthbean/ffmpeg/model/FFmpegInputParams;)V
 */
JNIEXPORT void JNICALL Java_com_truthbean_ffmpeg_FFmpegJava_destroy(JNIEnv *env, jobject obj, jobject handle);

/*
 * Class:     com_truthbean_jni_ffmpeg_FFmpegJava
 * Method:    record
 * Signature: (Ljava/lang/String;Ljava/lang/String;IZ)V
 */
JNIEXPORT void JNICALL Java_com_truthbean_ffmpeg_FFmpegJava_record(JNIEnv *, jobject, jstring, jstring, jint, jboolean);

#endif //_Included_com_truthbean_jni_ffmpeg_FFmpegJava
