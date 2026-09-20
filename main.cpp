#include <jni.h>
#include <pthread.h>
#include <unistd.h>
#include <android/log.h>
#include "antiband.h"
#include "menu.h"

#define LOG_TAG "ModMenu"
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)

static void* MainLoop(void*) {
    Menu_Init();
    while (true) {
        Menu_Render();
        usleep(16000);
    }
    return nullptr;
}

extern "C" JNIEXPORT void JNICALL
Java_com_rustycain_modmenu_ModInjector_nativeStartMenu(JNIEnv*, jclass) {
    LOGD("nativeStartMenu called");
    RunAntiBan();
    pthread_t t;
    pthread_create(&t, nullptr, MainLoop, nullptr);
    pthread_detach(t);
}