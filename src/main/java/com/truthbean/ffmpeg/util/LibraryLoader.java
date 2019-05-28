package com.truthbean.ffmpeg.util;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.nio.ByteBuffer;
import java.nio.channels.Channels;
import java.nio.channels.FileChannel;
import java.nio.channels.ReadableByteChannel;
import java.nio.file.Files;
import java.util.UUID;

public class LibraryLoader {
    private LibraryLoader() {
    }

    private static final String DETECTOR_JNI_PATH = getOrCreateDetectorJniPath();
    private static String osType;

    public static void load() {
        try {
            String suffix = ".dll";
            String preffix = "jni/win32/";
            if (OsUtils.isWindowsOs()) {
                if (OsUtils.is32Os()) {
                    // win32
                    osType = "win32";
                } else {
                    // win64
                    preffix = "jni/win64/";
                    osType = "win64";
                }
            } else {
                // unix
                preffix = "jni/unix/";
                suffix = ".so";
                osType = "unix";
            }
            writeAndLoadLibrary(preffix + "avutil-56" + suffix);
            writeAndLoadLibrary(preffix + "swresample-3" + suffix);
            writeAndLoadLibrary(preffix + "avcodec-58" + suffix);
            // writeAndLoadLibrary(preffix + "libpostproc" + suffix);
            writeAndLoadLibrary(preffix + "swscale-5" + suffix);
            writeAndLoadLibrary(preffix + "avformat-58" + suffix);
            writeAndLoadLibrary(preffix + "avfilter-7" + suffix);
            writeAndLoadLibrary(preffix + "avdevice-58" + suffix);
            writeAndLoadLibrary(preffix + "jpeg62" + suffix);
            writeAndLoadLibrary(preffix + "turbojpeg" + suffix);
            writeAndLoadLibrary(preffix + "ffmpeg-java" + suffix);
        } catch (Exception e) {
            e.printStackTrace();
        }

    }

    private static void writeAndLoadLibrary(String libraryFileName) throws IOException {
        File librarayFile = getOrCreateOutFile(libraryFileName);

        if (librarayFile.length() == 0) {
            // 文件如果不存在则拷贝
            InputStream libraryStream = LibraryLoader.class.getClassLoader().getResourceAsStream(libraryFileName);

            ReadableByteChannel inChannel = Channels.newChannel(libraryStream);
            FileOutputStream fileOutputStream = new FileOutputStream(librarayFile);
            FileChannel outChannel = fileOutputStream.getChannel();
    
            ByteBuffer buffer = ByteBuffer.allocateDirect(4096);
            while (inChannel.read(buffer) >= 0 || buffer.position() > 0) {
                buffer.flip();
                outChannel.write(buffer);
                buffer.compact();
            }
    
            outChannel.close();
            inChannel.close();
            fileOutputStream.close();
            libraryStream.close();
        }

        System.load(librarayFile.getAbsolutePath());
    }

    private static File getOrCreateOutFile(String libraryFileName) throws IOException {
        String detectorJniPath = DETECTOR_JNI_PATH;
        File libraryDir = new File(detectorJniPath, "jni");
        if (!libraryDir.exists()) {
            Files.createDirectories(libraryDir.toPath());
        }
        libraryDir = new File(detectorJniPath + "/jni/" + osType);
        if (!libraryDir.exists()) {
            Files.createDirectories(libraryDir.toPath());
        }
        File libraryFile = new File(detectorJniPath, libraryFileName);
        if (!libraryFile.exists()) {
            libraryFile.createNewFile();
        }
        return libraryFile;
    }

    private static String getOrCreateDetectorJniPath() {
        String ffmpegJniPath = "";
        try {
            String userHome = System.getProperty("user.home");
            String javaJniDirPath = userHome + "/.java_jni";
            File javaJniDir = new File(javaJniDirPath);
            if (!javaJniDir.exists()) {
                Files.createDirectories(javaJniDir.toPath());
            }
            if (!javaJniDir.isDirectory()) {
                javaJniDirPath += UUID.randomUUID();
                javaJniDir = new File(javaJniDirPath);
                Files.createDirectories(javaJniDir.toPath());
            }

            ffmpegJniPath = javaJniDirPath + "/com.truthbean.ffmpeg";
            File detectorJni = new File(ffmpegJniPath);
            if (!detectorJni.exists()) {
                Files.createDirectories(detectorJni.toPath());
            }
            if (!detectorJni.isDirectory()) {
                ffmpegJniPath += UUID.randomUUID();
                detectorJni = new File(ffmpegJniPath);
                Files.createDirectories(detectorJni.toPath());
            }

            ffmpegJniPath = ffmpegJniPath + "/0.0.1";
            detectorJni = new File(ffmpegJniPath);
            if (!detectorJni.exists()) {
                Files.createDirectories(detectorJni.toPath());
            }
            if (!detectorJni.isDirectory()) {
                ffmpegJniPath += UUID.randomUUID();
                detectorJni = new File(ffmpegJniPath);
                Files.createDirectories(detectorJni.toPath());
            }
        } catch (IOException e) {
            e.printStackTrace();
        }

        return ffmpegJniPath;
    }
}