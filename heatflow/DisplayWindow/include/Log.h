#ifndef LOG_H
#define LOG_H

#ifdef __ANDROID__
    #include <android/log.h>
    #define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "SimpleRay", __VA_ARGS__))
    #define LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN, "SimpleRay", __VA_ARGS__))
#else
    #include <stdio.h>
    #define LOGI(...) printf(__VA_ARGS__)
#endif

#endif
