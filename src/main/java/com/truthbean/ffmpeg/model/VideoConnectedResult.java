package com.truthbean.ffmpeg.model;

public class VideoConnectedResult {
    private int code;
    private String message;
    private int frameRate;

    /**
     * @param code the code to set
     */
    public void setCode(int code) {
        this.code = code;
    }

    /**
     * @param frameRate the frameRate to set
     */
    public void setFrameRate(int frameRate) {
        this.frameRate = frameRate;
    }

    /**
     * @param message the message to set
     */
    public void setMessage(String message) {
        this.message = message;
    }

    /**
     * @return the code
     */
    public int getCode() {
        return code;
    }

    /**
     * @return the frameRate
     */
    public int getFrameRate() {
        return frameRate;
    }

    /**
     * @return the message
     */
    public String getMessage() {
        return message;
    }

    @Override
    public String toString() {
        return "{\"code\":" + code + ",\"message\":\"" + message + "\",\"frameRate\":" + frameRate +"}";
    }

}