package com.jldevelopers.mediainfosource;

public class MediaInfoHelper {
    static {
        System.loadLibrary("mediainfo-android");
    }

    // Stream kind constants
    public static final int STREAM_GENERAL = 0;
    public static final int STREAM_VIDEO = 1;
    public static final int STREAM_AUDIO = 2;
    public static final int STREAM_TEXT = 3;
    public static final int STREAM_OTHER = 4;
    public static final int STREAM_IMAGE = 5;
    public static final int STREAM_MENU = 6;

    // Native methods
    public native boolean initMediaInfo();
    public native int openFile(String filePath);
    public native String getInform(int fileIndex);
    public native String getParameter(int fileIndex, String parameter);
    public native String getParameterWithStream(int fileIndex, int streamKind, int streamNumber, String parameter);
    public native int getStreamCount(int fileIndex, int streamKind);
    public native String getAvailableParameters(int fileIndex);
    public native String getFullInfo(int fileIndex);
    public native void closeAll();
    public native void cleanupMediaInfo();

    // Helper methods
    public String getGeneralInfo(String filePath) {
        if (openFile(filePath) >= 0) {
            String info = getFullInfo(0);
            closeAll();
            return info;
        }
        return "Failed to open file";
    }

    public String getDuration(String filePath) {
        if (openFile(filePath) >= 0) {
            String duration = getParameter(0, "Duration");
            closeAll();
            return duration;
        }
        return "";
    }

    public String getFormat(String filePath) {
        if (openFile(filePath) >= 0) {
            String format = getParameter(0, "Format");
            closeAll();
            return format;
        }
        return "";
    }

    public String getVideoFormat(String filePath) {
        if (openFile(filePath) >= 0) {
            String format = getParameterWithStream(0, STREAM_VIDEO, 0, "Format");
            closeAll();
            return format;
        }
        return "";
    }

    public String getAudioFormat(String filePath) {
        if (openFile(filePath) >= 0) {
            String format = getParameterWithStream(0, STREAM_AUDIO, 0, "Format");
            closeAll();
            return format;
        }
        return "";
    }

    public String getResolution(String filePath) {
        if (openFile(filePath) >= 0) {
            String width = getParameterWithStream(0, STREAM_VIDEO, 0, "Width");
            String height = getParameterWithStream(0, STREAM_VIDEO, 0, "Height");
            closeAll();
            if (!width.isEmpty() && !height.isEmpty()) {
                return width + "x" + height;
            }
        }
        return "";
    }
}