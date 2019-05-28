package com.truthbean.ffmpeg.model;

import com.truthbean.ffmpeg.constant.ImageStreamType;

public class ImageBufferParams {

    private int frames = 1;
    private int type = ImageStreamType.JPEG.getValue();
    private int quality = 80;

    /**
     * @param frames the frames to set
     */
    public void setFrames(int frames) {
        this.frames = frames;
    }

    /**
     * @param quality the quality to set
     */
    public void setQuality(int quality) {
        this.quality = quality;
    }

    /**
     * @param type the type to set
     */
    public void setType(int type) {
        this.type = type;
    }

    /**
     * @param type the type to set
     */
    public void setType(ImageStreamType type) {
        this.type = type.getValue();
    }

    /**
     * @return the frames
     */
    public int getFrames() {
        return frames;
    }

    /**
     * @return the quality
     */
    public int getQuality() {
        return quality;
    }

    /**
     * @return the type
     */
    public int getType() {
        return type;
    }

}