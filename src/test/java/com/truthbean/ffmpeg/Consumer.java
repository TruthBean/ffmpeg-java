package com.truthbean.ffmpeg;

/**
 * 消费者
 */
public class Consumer<T extends Resource, Callback> implements Runnable {
    private T resource;
    private ConsumerCallback<Callback> consumerCallback;

    public Consumer(T resource, ConsumerCallback<Callback> consumerCallback) {
        this.resource = resource;
        this.consumerCallback = consumerCallback;
    }

    @Override
    public void run() {
        while (true) {
            try {
                Thread.sleep(1);
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
            resource.consumer(consumerCallback);
        }
    }
}