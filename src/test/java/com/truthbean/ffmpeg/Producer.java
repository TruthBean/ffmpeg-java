package com.truthbean.ffmpeg;

/**
 * 生产者
 */
public class Producer<T extends Resource> implements Runnable {
    private T resource;

    public Producer(T resource) {
        this.resource = resource;
    }

    @Override
    public void run() {
        while (true) {
            try {
                Thread.sleep(1);
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
            resource.create();
        }

    }
}