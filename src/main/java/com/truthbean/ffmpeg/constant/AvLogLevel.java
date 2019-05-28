package com.truthbean.ffmpeg.constant;

public enum AvLogLevel {
    ERROR(0),
    
    DEBUG(1),

    INFO(2);

    private int level;

    /**
     * @return the level
     */
    public int getLevel() {
        return level;
    }

    AvLogLevel(int level) {
        this.level = level;
    }
}