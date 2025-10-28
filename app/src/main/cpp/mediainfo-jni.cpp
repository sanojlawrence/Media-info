#include <jni.h>
#include <string>
#include <android/log.h>

// Include MediaInfo headers
#include "MediaInfo/MediaInfo.h"
#include "MediaInfo/MediaInfoList.h"

#define LOG_TAG "MediaInfoJNI"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

#ifdef __cplusplus
extern "C" {
#endif

// Global MediaInfoList instance
MediaInfoLib::MediaInfoList* g_mediaInfoList = nullptr;

// Initialize MediaInfo
JNIEXPORT jboolean JNICALL
Java_com_jldevelopers_mediainfosource_MediaInfoHelper_initMediaInfo(JNIEnv *env, jobject thiz) {
    try {
        if (g_mediaInfoList == nullptr) {
            g_mediaInfoList = new MediaInfoLib::MediaInfoList();
            LOGI("MediaInfo initialized successfully");
            return JNI_TRUE;
        }
        return JNI_TRUE;
    } catch (...) {
        LOGE("Failed to initialize MediaInfo");
        return JNI_FALSE;
    }
}

// Open a file for analysis
JNIEXPORT jint JNICALL
Java_com_jldevelopers_mediainfosource_MediaInfoHelper_openFile(JNIEnv *env, jobject thiz, jstring file_path) {
    if (g_mediaInfoList == nullptr) {
        LOGE("MediaInfo not initialized");
        return -1;
    }

    try {
        const char *nativeFilePath = env->GetStringUTFChars(file_path, nullptr);
        if (nativeFilePath == nullptr) {
            return -1;
        }

        // Simple conversion
        std::string filePathStr(nativeFilePath);
        MediaInfoLib::String mediaInfoPath;
        for (char c : filePathStr) {
            mediaInfoPath += (MediaInfoLib::Char)c;
        }

        size_t result = g_mediaInfoList->Open(mediaInfoPath);

        env->ReleaseStringUTFChars(file_path, nativeFilePath);

        LOGI("Open file result: %d", (int)result);
        return (jint)result;
    } catch (...) {
        LOGE("Exception opening file");
        return -1;
    }
}

// Get media information as string
JNIEXPORT jstring JNICALL
Java_com_jldevelopers_mediainfosource_MediaInfoHelper_getInform(JNIEnv *env, jobject thiz, jint file_index) {
    if (g_mediaInfoList == nullptr) {
        LOGE("MediaInfo not initialized");
        return env->NewStringUTF("MediaInfo not initialized");
    }

    try {
        MediaInfoLib::String inform = g_mediaInfoList->Inform((size_t)file_index);

        // Convert MediaInfoLib::String to std::string
        std::string result;
        for (size_t i = 0; i < inform.size(); ++i) {
            result += (char)inform[i];
        }

        return env->NewStringUTF(result.c_str());
    } catch (...) {
        LOGE("Exception getting inform");
        return env->NewStringUTF("Error getting media information");
    }
}

// Get specific parameter value - FIXED VERSION with correct parameters
JNIEXPORT jstring JNICALL
Java_com_jldevelopers_mediainfosource_MediaInfoHelper_getParameter(JNIEnv *env, jobject thiz, jint file_index, jstring parameter) {
    if (g_mediaInfoList == nullptr) {
        return env->NewStringUTF("");
    }

    try {
        const char *nativeParam = env->GetStringUTFChars(parameter, nullptr);
        if (nativeParam == nullptr) {
            return env->NewStringUTF("");
        }

        // Convert parameter to MediaInfoLib::String
        std::string paramStr(nativeParam);
        MediaInfoLib::String mediaInfoParam;
        for (char c : paramStr) {
            mediaInfoParam += (MediaInfoLib::Char)c;
        }

        MediaInfoLib::String value;

        // FIX: Use the correct Get() overload with 4 parameters
        // Try different stream types with stream number 0 (first stream of each type)
        value = g_mediaInfoList->Get((size_t)file_index, MediaInfoLib::Stream_General, 0, mediaInfoParam, MediaInfoLib::Info_Text, MediaInfoLib::Info_Name);

        // If empty, try other stream types
        if (value.empty()) {
            value = g_mediaInfoList->Get((size_t)file_index, MediaInfoLib::Stream_Video, 0, mediaInfoParam, MediaInfoLib::Info_Text, MediaInfoLib::Info_Name);
        }
        if (value.empty()) {
            value = g_mediaInfoList->Get((size_t)file_index, MediaInfoLib::Stream_Audio, 0, mediaInfoParam, MediaInfoLib::Info_Text, MediaInfoLib::Info_Name);
        }

        env->ReleaseStringUTFChars(parameter, nativeParam);

        // Convert result back to std::string
        std::string result;
        for (size_t i = 0; i < value.size(); ++i) {
            result += (char)value[i];
        }

        return env->NewStringUTF(result.c_str());
    } catch (...) {
        return env->NewStringUTF("");
    }
}

// Alternative: Get parameter with stream kind and stream number specified
JNIEXPORT jstring JNICALL
Java_com_jldevelopers_mediainfosource_MediaInfoHelper_getParameterWithStream(JNIEnv *env, jobject thiz, jint file_index, jint stream_kind, jint stream_number, jstring parameter) {
    if (g_mediaInfoList == nullptr) {
        return env->NewStringUTF("");
    }

    try {
        const char *nativeParam = env->GetStringUTFChars(parameter, nullptr);
        if (nativeParam == nullptr) {
            return env->NewStringUTF("");
        }

        // Convert parameter to MediaInfoLib::String
        std::string paramStr(nativeParam);
        MediaInfoLib::String mediaInfoParam;
        for (char c : paramStr) {
            mediaInfoParam += (MediaInfoLib::Char)c;
        }

        // Use the stream kind and stream number specified
        MediaInfoLib::String value = g_mediaInfoList->Get(
                (size_t)file_index,
                (MediaInfoLib::stream_t)stream_kind,
                (size_t)stream_number,
                mediaInfoParam,
                MediaInfoLib::Info_Text,
                MediaInfoLib::Info_Name
        );

        env->ReleaseStringUTFChars(parameter, nativeParam);

        // Convert result back to std::string
        std::string result;
        for (size_t i = 0; i < value.size(); ++i) {
            result += (char)value[i];
        }

        return env->NewStringUTF(result.c_str());
    } catch (...) {
        return env->NewStringUTF("");
    }
}

// Get count of streams for a specific type
JNIEXPORT jint JNICALL
Java_com_jldevelopers_mediainfosource_MediaInfoHelper_getStreamCount(JNIEnv *env, jobject thiz, jint file_index, jint stream_kind) {
    if (g_mediaInfoList == nullptr) {
        return -1;
    }

    try {
        size_t count = g_mediaInfoList->Count_Get((size_t)file_index, (MediaInfoLib::stream_t)stream_kind);
        return (jint)count;
    } catch (...) {
        return -1;
    }
}

// Get available parameters list
JNIEXPORT jstring JNICALL
Java_com_jldevelopers_mediainfosource_MediaInfoHelper_getAvailableParameters(JNIEnv *env, jobject thiz, jint file_index) {
    if (g_mediaInfoList == nullptr) {
        return env->NewStringUTF("");
    }

    try {
        std::string result;

        // Get basic information from different stream types
        MediaInfoLib::String generalInfo = g_mediaInfoList->Get((size_t)file_index, MediaInfoLib::Stream_General, 0, MediaInfoLib::String(__T("Format")), MediaInfoLib::Info_Text, MediaInfoLib::Info_Name);
        MediaInfoLib::String videoInfo = g_mediaInfoList->Get((size_t)file_index, MediaInfoLib::Stream_Video, 0, MediaInfoLib::String(__T("Format")), MediaInfoLib::Info_Text, MediaInfoLib::Info_Name);
        MediaInfoLib::String audioInfo = g_mediaInfoList->Get((size_t)file_index, MediaInfoLib::Stream_Audio, 0, MediaInfoLib::String(__T("Format")), MediaInfoLib::Info_Text, MediaInfoLib::Info_Name);

        // Convert to string
        auto convertToString = [](const MediaInfoLib::String& str) -> std::string {
            std::string result;
            for (size_t i = 0; i < str.size(); ++i) {
                result += (char)str[i];
            }
            return result;
        };

        result = "General Format: " + convertToString(generalInfo) + "\n" +
                 "Video Format: " + convertToString(videoInfo) + "\n" +
                 "Audio Format: " + convertToString(audioInfo);

        return env->NewStringUTF(result.c_str());
    } catch (...) {
        return env->NewStringUTF("");
    }
}

// Get all information as a formatted string (similar to mediainfo command line)
JNIEXPORT jstring JNICALL
Java_com_jldevelopers_mediainfosource_MediaInfoHelper_getFullInfo(JNIEnv *env, jobject thiz, jint file_index) {
    if (g_mediaInfoList == nullptr) {
        return env->NewStringUTF("MediaInfo not initialized");
    }

    try {
        MediaInfoLib::String inform = g_mediaInfoList->Inform((size_t)file_index);

        // Convert MediaInfoLib::String to std::string
        std::string result;
        for (size_t i = 0; i < inform.size(); ++i) {
            result += (char)inform[i];
        }

        return env->NewStringUTF(result.c_str());
    } catch (...) {
        return env->NewStringUTF("Error getting full information");
    }
}

// Close all files
JNIEXPORT void JNICALL
Java_com_jldevelopers_mediainfosource_MediaInfoHelper_closeAll(JNIEnv *env, jobject thiz) {
    if (g_mediaInfoList != nullptr) {
        g_mediaInfoList->Close();
    }
}

// Cleanup MediaInfo
JNIEXPORT void JNICALL
Java_com_jldevelopers_mediainfosource_MediaInfoHelper_cleanupMediaInfo(JNIEnv *env, jobject thiz) {
    if (g_mediaInfoList != nullptr) {
        delete g_mediaInfoList;
        g_mediaInfoList = nullptr;
        LOGI("MediaInfo cleaned up");
    }
}

#ifdef __cplusplus
}
#endif