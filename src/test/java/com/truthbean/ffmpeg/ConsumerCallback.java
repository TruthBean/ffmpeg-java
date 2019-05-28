package com.truthbean.ffmpeg;

public interface ConsumerCallback<T> {

    void callback(T target);

}