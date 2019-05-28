package com.truthbean.ffmpeg;

public interface Resource<Callback> {
    void consumer(ConsumerCallback<Callback> consumerCallback);
  
    void create();
  }