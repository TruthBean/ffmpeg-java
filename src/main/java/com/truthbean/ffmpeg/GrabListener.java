package com.truthbean.ffmpeg;

import com.truthbean.ffmpeg.model.ImageBuffer;

import java.io.IOException;

public interface GrabListener {
        void grab(ImageBuffer imageBuffer) throws IOException;
    }