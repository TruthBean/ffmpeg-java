package com.truthbean.ffmpeg.model;

public class ImageBuffer {
    private int code;
    private String message;

    private int size;
    private byte[] data;

    /**
     * @param data the data to set
     */
    public void setData(byte[] data) {
        this.data = data;
    }

    /**
     * @param size the size to set
     */
    public void setSize(int size) {
        this.size = size;
    }

    /**
     * @return the data
     */
    public byte[] getData() {
        return data;
    }

    /**
     * @return the size
     */
    public int getSize() {
        return size;
    }

    public int getCode() {
        return code;
    }

    public void setCode(int code) {
        this.code = code;
    }

    public String getMessage() {
        return message;
    }

    public void setMessage(String message) {
        this.message = message;
    }

    @Override
    public synchronized void finalize() {
        try {
            super.finalize();
            data = null;
            System.gc();
        } catch (Throwable throwable) {
            throwable.printStackTrace();
        }
    }

    public ImageBuffer deepClone() {
        ImageBuffer imageBuffer = new ImageBuffer();
        imageBuffer.code = code;
        imageBuffer.message = message;
        imageBuffer.size = size;
        imageBuffer.data = new byte[Math.toIntExact(size)];
        System.arraycopy(data, 0, imageBuffer.data, 0, size);
        return imageBuffer;
    }
}