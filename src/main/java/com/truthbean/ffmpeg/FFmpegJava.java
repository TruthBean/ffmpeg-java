package com.truthbean.ffmpeg;

import java.io.Closeable;
import java.io.IOException;

import com.truthbean.ffmpeg.model.FFmpegInputParams;
import com.truthbean.ffmpeg.model.ImageBuffer;
import com.truthbean.ffmpeg.model.VideoConnectedResult;
import com.truthbean.ffmpeg.util.LibraryLoader;
import com.truthbean.ffmpeg.model.ImageBufferParams;

public class FFmpegJava implements Closeable {
    static {
        LibraryLoader.load();
    }

    private static final Object REFERENCE = new Object();

    private volatile long visPointer;

    /**
     * @param visPointer the visPointer to set
     */
    public synchronized void setVisPointer(long visPointer) {
        this.visPointer = visPointer;
    }

    /**
     * @return the visPointer
     */
    public synchronized long getVisPointer() {
        return visPointer;
    }

    public VideoConnectedResult connectVideo(FFmpegInputParams params) {
        synchronized (REFERENCE) {
            if (params.getVideoAddr() == null || "".equals(params.getVideoAddr().trim())) {
                throw new NullPointerException("video addr cannot be null !");
            }
            visPointer = 0L;
            VideoConnectedResult result = new VideoConnectedResult();
            init(this, params, result);
            return result;
        }
    }

    public ImageBuffer grab(ImageBufferParams params) {
        ImageBuffer imageBuffer = new ImageBuffer();
        getImageBuffer(this, params, imageBuffer);
        return imageBuffer;
    }

    public void doGrab(ImageBufferParams params, GrabListener listener) {
        getImageBufferThreadly(this, params, listener);
    }

    @Override
    public synchronized void close() {
        synchronized (REFERENCE) {
            destroy(this);
            finalize();
        }
    }

    @Override
    public synchronized void finalize() {
        try {
            super.finalize();
        } catch (Throwable throwable) {
            throwable.printStackTrace();
        }
    }

    public static void recordVideo(String videoAddr, String outputFilePath, int recordSeconds, boolean useGpu) {
        if (videoAddr == null || "".equals(videoAddr)) {
            throw new NullPointerException("video addr cannot be null !");
        }

        if (outputFilePath == null || "".equals(outputFilePath)) {
            throw new NullPointerException("output file path cannot be null !");
        }

        if (recordSeconds <= 0) {
            throw new RuntimeException("record seconds must be positive !");
        }

        record(videoAddr, outputFilePath, recordSeconds, useGpu);
    }

    public interface GrabListener {
        void grab(ImageBuffer imageBuffer) throws IOException;
    }

    private native void init(FFmpegJava handle, FFmpegInputParams params, VideoConnectedResult result);

    private synchronized native void getImageBuffer(FFmpegJava handle, ImageBufferParams params, ImageBuffer imageBuffer);

    private native void getImageBufferThreadly(FFmpegJava handle, ImageBufferParams params, GrabListener grabListener);

    private native void destroy(FFmpegJava handle);

    private static native void record(String rtspAddr, String outFile, int recordSeconds, boolean useGpu);
}
