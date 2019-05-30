package com.truthbean.ffmpeg;

import com.truthbean.ffmpeg.model.FFmpegInputParams;
import com.truthbean.ffmpeg.model.ImageBuffer;
import com.truthbean.ffmpeg.model.VideoConnectedResult;
import com.truthbean.ffmpeg.constant.AvLogLevel;
import com.truthbean.ffmpeg.constant.ImageStreamType;
import com.truthbean.ffmpeg.model.ImageBufferParams;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.util.ArrayDeque;
import java.util.Queue;
import java.util.concurrent.*;
import java.util.concurrent.atomic.AtomicInteger;

public class FFmpegJavaTest {

    private static final Queue<ImageBuffer> queue = new ArrayDeque<>();

    private static final ThreadFactory namedThreadFactory = Executors.privilegedThreadFactory();
    private static final ExecutorService doubleThreadPool = new ThreadPoolExecutor(2, 2, 0L, TimeUnit.MILLISECONDS,
            new LinkedBlockingQueue<>(1024), namedThreadFactory, new ThreadPoolExecutor.AbortPolicy());

    /*
     * private static List<Integer> list = new CopyOnWriteArrayList<>();
     *
     * ThreadFactory namedThreadFactory = Executors.defaultThreadFactory();
     * ExecutorService singleThreadPool = new ThreadPoolExecutor(1, 1, 0L,
     * TimeUnit.MILLISECONDS, new LinkedBlockingQueue<>(1024), namedThreadFactory,
     * new ThreadPoolExecutor.AbortPolicy());
     *
     * // singleThreadPool.execute(()->
     * System.out.println(Thread.currentThread().getName())); //
     * singleThreadPool.shutdown();
     *
     * private static void producer() { new Thread(() -> { while (true) {
     * list.add(new Random().nextInt(100)); } }).run(); }
     *
     * private static int consumer() { if (list.isEmpty()) { return 0; } return
     * list.get(0); }
     *
     * public static void main(String[] args) { Thread producerThread = new
     * Thread(() -> { while (true) { producer(); System.out.println(consumer()); }
     * });
     *
     * producerThread.run();
     *
     * // try { // Thread.sleep(1000_000_000); // } catch (InterruptedException e) {
     * // e.printStackTrace(); // }
     *
     * }
     *
     */

    private static void saveImage(ImageBuffer imageBuffer, long i) throws IOException {
        if (imageBuffer != null) {
            System.out.println(imageBuffer.getSize());

            /*String projectDir = System.getProperty("user.dir");
            String imageFileName = projectDir + "/images/buffer" + i + ".jpeg";
            File imageFile = new File(imageFileName);
            if (!imageFile.exists()) {
                imageFile.createNewFile();
            }
            FileOutputStream fileOutputStream = new FileOutputStream(imageFile);
            fileOutputStream.write(imageBuffer.getData());
            fileOutputStream.flush();
            fileOutputStream.close();*/

            imageBuffer.finalize();
            imageBuffer = null;
        }
    }

    private void consumer() {
        for (long i = 0; true; i++) {
            try {
                if (!running) {
                    break;
                }
                ImageBuffer imageBuffer = queue.poll();
                saveImage(imageBuffer, i++);
            } catch (Exception e) {
                e.printStackTrace();
            }
        }
    }

    private final static FFmpegJava ffmpeg = new FFmpegJava();
    private volatile boolean running = false;

    private void producer() {
        try {
            String addr = "rtsp://admin:iec123456@192.168.1.72:554/unicast/c1/s0/live";
            // addr = "rtsp://admin:iec123456@192.168.1.71:554/h264/ch1/main/av_stream";

            FFmpegInputParams params = new FFmpegInputParams();
            params.setVideoAddr(addr);
            params.setTimeout(2 * 500000);
            params.setGpuId(0);
            params.setLevel(AvLogLevel.DEBUG);
            params.setNobuffer(false);
            params.setUseGpu(false);
            params.setUseTcp(true);

            VideoConnectedResult result = ffmpeg.connectVideo(params);
            System.out.println("connect: " + result);

            if (result.getCode() == 0) {
                ImageBufferParams imageBufferParams = new ImageBufferParams();
                imageBufferParams.setFrames(1);
                imageBufferParams.setQuality(100);
                imageBufferParams.setType(ImageStreamType.JPEG);
                running = true;

                ImageBuffer imageBuffer;
                AtomicInteger i = new AtomicInteger();
                while ((imageBuffer = ffmpeg.grab(imageBufferParams)) != null) {
                    System.out.println(running);
                    if (!running) break;
                    Thread.sleep(1);
                    // queue.add(imageBuffer);
                    saveImage(imageBuffer, i.getAndIncrement());
                    // imageBuffer = null;
                }

                /*ffmpeg.doGrab(imageBufferParams, new GrabListener() {
                    @Override
                    public void grab(ImageBuffer imageBuffer) throws IOException {
                        System.out.println(imageBuffer);
                        System.out.println(imageBuffer.getSize());
                        saveImage(imageBuffer, i.getAndIncrement());
                    }
                });*/
            }
        } catch (Exception e) {
            e.printStackTrace();
        } finally {
            ffmpeg.close();
        }
    }

    public static void main(String[] args) {

        // producer();

        /* new Thread(new Producer<>(flvHandler)).start();

        new Thread(new Consumer<FlvHandler, ByteArrayOutputStream>(flvHandler, target -> {
            System.out.println(target);
            try {
                if (session.isOpen()) {
                session.getBasicRemote().sendBinary(ByteBuffer.wrap(target.toByteArray()));
                }
            } catch (IOException e) {
                e.printStackTrace();
            }
        })).start();

        */

        // doubleThreadPool.execute(FFmpegJavaTest::producer);
        // doubleThreadPool.execute(FFmpegJavaTest::consumer);

        final FFmpegJavaTest test = new FFmpegJavaTest();

        Thread thread = new Thread(test::producer);
        // thread.setDaemon(true);
        // thread.start();

        // doubleThreadPool.execute(thread);

        test.producer();

        /*for (int i = 0; i <= 1000; i++) {
            System.out.println(i);
            if (i == 1000) {
                try {
                    thread.join();
                    thread.interrupt();
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
            }
        }*/

        /*Thread stop = new Thread(() -> {
            try {
                Thread.sleep(1000);
                test.running = false;
                System.out.println("join");
                thread.join();
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
            try {
                Thread.sleep(1000);
                System.out.println("interrupt");
                thread.interrupt();
                Thread.sleep(1000);
                System.out.println("close");
                ffmpeg.close();
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        });

        doubleThreadPool.execute(stop);
        doubleThreadPool.shutdown();*/

        // producer();
        
         /*String addr = "rtsp://admin:iec123456@192.168.1.72:554/unicast/c1/s0/live";
         String projectDir = System.getProperty("user.dir");
         String outputFilePath = projectDir + "/images/video.mp4";
         FFmpegJava.recordVideo(addr, outputFilePath, 60, false);*/
    }
}