#include "antiband.h"
#include <sys/ptrace.h>
#include <sys/prctl.h>
#include <sys/mman.h>
#include <sys/system_properties.h>
#include <unistd.h>
#include <fcntl.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cstdint>
#include <dirent.h>
#include <string>
#include <fstream>
#include <thread>
#include <atomic>
#include <android/log.h>

#define LOG_TAG "AntiBan"
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)

static std::atomic<bool> running(true);

static void L1_AntiPtrace() {
    if (ptrace(PTRACE_TRACEME, 0, 0, 0) == -1) _exit(0);
}

static void L2_AntiDebugger() {
    FILE* fp = fopen("/proc/self/status", "r");
    if (!fp) return;
    char line[256];
    while (fgets(line, sizeof(line), fp)) {
        if (strncmp(line, "TracerPid:", 10) == 0) {
            if (atoi(line + 10) != 0) { fclose(fp); _exit(0); }
        }
    }
    fclose(fp);
}

static void L3_AntiFridaFiles() {
    const char* paths[] = {
        "/data/local/tmp/frida-server",
        "/data/local/tmp/re.frida.server",
        "/data/local/tmp/frida",
        "/sdcard/frida-server",
        "/system/bin/frida-server"
    };
    for (const char* p : paths) {
        if (access(p, F_OK) != -1) _exit(0);
    }
}

static void L4_AntiFridaMaps() {
    std::ifstream maps("/proc/self/maps");
    std::string line;
    while (std::getline(maps, line)) {
        if (line.find("frida") != std::string::npos ||
            line.find("gum-js-loop") != std::string::npos ||
            line.find("gmain") != std::string::npos ||
            line.find("gdbus") != std::string::npos) {
            _exit(0);
        }
    }
}

static void L5_AntiFridaPort() {
    FILE* fp = popen("netstat -an 2>/dev/null | grep 27042", "r");
    if (fp) {
        char buf[256];
        if (fgets(buf, sizeof(buf), fp)) { pclose(fp); _exit(0); }
        pclose(fp);
    }
}

static void L6_AntiXposed() {
    const char* paths[] = {
        "/system/framework/XposedBridge.jar",
        "/system/lib/libxposed_art.so",
        "/system/lib64/libxposed_art.so",
        "/data/data/de.robv.android.xposed.installer",
        "/data/data/com.saurik.substrate"
    };
    for (const char* p : paths) {
        if (access(p, F_OK) != -1) _exit(0);
    }

    std::ifstream maps("/proc/self/maps");
    std::string line;
    while (std::getline(maps, line)) {
        if (line.find("Xposed") != std::string::npos ||
            line.find("xposed") != std::string::npos) {
            _exit(0);
        }
    }
}

static void L7_AntiHook() {
    std::ifstream maps("/proc/self/maps");
    std::string line;
    while (std::getline(maps, line)) {
        if (line.find("substrate") != std::string::npos ||
            line.find("dobby") != std::string::npos ||
            line.find("epic") != std::string::npos ||
            line.find("sandhook") != std::string::npos ||
            line.find("yahfa") != std::string::npos ||
            line.find("pine") != std::string::npos) {
            _exit(0);
        }
    }
}

static void L8_AntiEmulator() {
    char value[92] = {0};

    __system_property_get("ro.build.fingerprint", value);
    if (strstr(value, "generic") || strstr(value, "emulator") ||
        strstr(value, "vbox") || strstr(value, "nox")) {
        _exit(0);
    }

    __system_property_get("ro.kernel.qemu", value);
    if (strcmp(value, "1") == 0) _exit(0);

    if (access("/dev/socket/qemud", F_OK) != -1) _exit(0);
    if (access("/dev/qemu_pipe", F_OK) != -1) _exit(0);
}

static void L9_HideProcess() {
    prctl(PR_SET_NAME, "system_server", 0, 0, 0);

    int fd = open("/proc/self/cmdline", O_WRONLY);
    if (fd >= 0) {
        const char* name = "com.android.systemui";
        write(fd, name, strlen(name));
        close(fd);
    }
}

static void L10_HideMaps() {
    std::ifstream maps("/proc/self/maps");
    std::string line;
    while (std::getline(maps, line)) {
        if (line.find("libModMenu") != std::string::npos ||
            line.find("libmenu") != std::string::npos) {
            uintptr_t start, end;
            char perms[8];
            sscanf(line.c_str(), "%lx-%lx %s", &start, &end, perms);
            mprotect((void*)start, end - start, PROT_READ | PROT_WRITE);
            memset((void*)start, 0, 1);
        }
    }
}

static void L11_AntiDump() {
    ptrace(PTRACE_TRACEME, 0, 0, 0);
}

static void L12_MonitorThread() {
    while (running) {
        L1_AntiPtrace();
        L2_AntiDebugger();
        L3_AntiFridaFiles();
        L4_AntiFridaMaps();
        L5_AntiFridaPort();
        L6_AntiXposed();
        L7_AntiHook();
        L8_AntiEmulator();
        L9_HideProcess();
        L10_HideMaps();
        usleep(3000000);
    }
}

void RunAntiBan() {
    LOGD("Anti-ban 12 layers starting...");

    L1_AntiPtrace();
    L2_AntiDebugger();
    L3_AntiFridaFiles();
    L4_AntiFridaMaps();
    L5_AntiFridaPort();
    L6_AntiXposed();
    L7_AntiHook();
    L8_AntiEmulator();
    L9_HideProcess();
    L10_HideMaps();
    L11_AntiDump();

    std::thread monitor(L12_MonitorThread);
    monitor.detach();

    LOGD("Anti-ban 12 layers started");
}