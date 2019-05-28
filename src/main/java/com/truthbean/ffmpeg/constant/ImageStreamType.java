package com.truthbean.ffmpeg.constant;

/**
 * @author truthbean
 */
public enum ImageStreamType {
    /**
     * yuv420p
     */
    YUV(0),

    /**
     * rgba
     */
    RGB(1),

    /**
     * jpeg
     */
    JPEG(2);

    private int value;

    ImageStreamType(int value) {
        this.value = value;
    }

    /**
     * @return the value
     */
    public int getValue() {
        return value;
    }

}