package com.truthbean.ffmpeg.util;

public class OsUtils {
    private OsUtils() {
    }

    public static boolean is32Os() {
        String osArch = System.getProperty("os.arch");
        return "x86".equalsIgnoreCase(osArch);
    }

    public static boolean isWindowsOs() {
        String osName = System.getProperty("os.name");
        return osName != null && osName.toLowerCase().contains("win");
    }
}