package com.truthbean.ffmpeg.model;

import com.truthbean.ffmpeg.constant.AvLogLevel;

public class FFmpegInputParams {
    private String videoAddr;
    private int timeout = 5;
    private boolean nobuffer = false;
    private boolean useGpu = false;
    private boolean useTcp = true;
    private int level = AvLogLevel.INFO.getLevel();
    private int gpuId;

    /**
     * @param gpuId the gpuId to set
     */
    public void setGpuId(int gpuId) {
        this.gpuId = gpuId;
    }

    /**
     * @param level the level to set
     */
    public void setLevel(int level) {
        this.level = level;
    }

    /**
     * @param level the level to set
     */
    public void setLevel(AvLogLevel level) {
        this.level = level.getLevel();
    }
    
    /**
     * @param nobuffer the nobuffer to set
     */
    public void setNobuffer(boolean nobuffer) {
        this.nobuffer = nobuffer;
    }

    /**
     * @param videoAddr the videoAddr to set
     */
    public void setVideoAddr(String videoAddr) {
        this.videoAddr = videoAddr;
    }

    /**
     * @param timeout the timeout to set
     */
    public void setTimeout(int timeout) {
        this.timeout = timeout;
    }

    /**
     * @param useGpu the useGpu to set
     */
    public void setUseGpu(boolean useGpu) {
        this.useGpu = useGpu;
    }

    /**
     * @return the gpuId
     */
    public int getGpuId() {
        return gpuId;
    }

    /**
     * @return the level
     */
    public int getLevel() {
        return level;
    }

    /**
     * @return the rtspAddr
     */
    public String getVideoAddr() {
        return videoAddr;
    }

    /**
     * @return the timeout
     */
    public int getTimeout() {
        return timeout;
    }

    /**
     * @return the nobuffer
     */
    public boolean isNobuffer() {
        return nobuffer;
    }

    /**
     * @return the useGpu
     */
    public boolean isUseGpu() {
        return useGpu;
    }

    public boolean isUseTcp() {
        return useTcp;
    }

    public void setUseTcp(boolean useTcp) {
        this.useTcp = useTcp;
    }

    @Override
    public String toString() {
        return "{" +
                "\"videoAddr\":\"" + videoAddr + '\"' +
                ",\"timeout\":" + timeout +
                ",\"nobuffer\":" + nobuffer +
                ",\"useGpu\":" + useGpu +
                ",\"level\":" + level +
                ",\"useTcp\":" + useTcp +
                ",\"gpuId\":" + gpuId +
                '}';
    }
}