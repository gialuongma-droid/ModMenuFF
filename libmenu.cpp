#include <jni.h>
#include <android/log.h>
#include <pthread.h>
#include <unistd.h>
#include <dlfcn.h>
#include <sys/ptrace.h>
#include <sys/prctl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <EGL/egl.h>
#include <GLES3/gl3.h>
#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include "dobby.h"

#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,"RTC",__VA_ARGS__)

namespace RTC {
    bool Aimlock=false,AimHead=false,AimBody=false;
    bool Prediction=false,AutoFire=false,WallCheck=false;
    bool TeamCheck=false,VisibleOnly=false,SilentAim=false;
    bool AimKey=false,AimBone=false;
    float FOV=90.f,SmoothAim=15.f,PredictionFactor=50.f,FireDelay=0.f;
    bool EspMaster=false,BoxEsp=false,LineEsp=false;
    bool NameEsp=false,DistanceEsp=false,HealthEsp=false;
    bool SkeletonEsp=false,Chams=false,WeaponEsp=false;
    float MaxRange=500.f;
    bool InfiniteAmmo=true,NoReload=false,NoRecoil=false;
    bool NoSpread=false,ThroughWall=false,InstantHit=false;
    bool MagicBullet=false,OneShotKill=false;
    float DamageMultiplier=1.f,FireRateBoost=1.f;
    bool SpinWhileRun=false,SpeedHack=false,SuperJump=false;
    bool FlyMode=false,GodMode=false,NoFallDamage=false;
    bool FastHeal=false,NoClip=false;
    float SpinSpeed=5.f,SpeedValue=2.f;
    bool AntiBan=true,HideMaps=true;
}
using namespace RTC;

float g_Hue=0.f;
bool g_AutoColor=true;
bool g_ShowMenu=true;

ImVec4 accent(){
    if(g_AutoColor){g_Hue+=0.005f;if(g_Hue>1.f)g_Hue=0.f;}
    return ImVec4(1.f,0.13f,0.13f,1.f);
}

void antiPtrace(){
    prctl(PR_SET_DUMPABLE,0,0,0,0);
    ptrace(PTRACE_TRACEME,0,0,0);
}

void antiFrida(){
    const int p[]={27042,27043};
    for(int port:p){
        int s=socket(AF_INET,SOCK_STREAM,0);
        struct sockaddr_in a;
        a.sin_family=AF_INET;
        a.sin_port=htons(port);
        a.sin_addr.s_addr=htonl(INADDR_LOOPBACK);
        if(connect(s,(struct sockaddr*)&a,sizeof(a))==0){close(s);_exit(0);}
        close(s);
    }
}

void* antiBanThread(void*){
    antiPtrace();
    while(true){ if(AntiBan)antiFrida(); sleep(5); }
    return nullptr;
}

void (*old_ApplyDamage)(void*,float);
void hooked_ApplyDamage(void* i,float d){
    if(GodMode) return;
    if(OneShotKill){ old_ApplyDamage(i,99999.f); return; }
    old_ApplyDamage(i,d*DamageMultiplier);
}

float (*old_GetSpeed)(void*);
float hooked_GetSpeed(void* i){
    float v=old_GetSpeed(i);
    if(SpeedHack) v*=SpeedValue;
    return v;
}

bool (*old_CheckAmmo)(void*);
bool hooked_CheckAmmo(void* i){
    if(InfiniteAmmo) return true;
    return old_CheckAmmo(i);
}

void (*old_GetRecoil)(void*);
void hooked_GetRecoil(void* i){
    if(NoRecoil) return;
    old_GetRecoil(i);
}

void* getBase(const char* n){
    void* h=dlopen(n,RTLD_NOW);
    if(!h) return nullptr;
    void* b=dlsym(h,"JNI_OnLoad");
    dlclose(h);
    return b;
}

void* hackThread(void*){
    sleep(3);
    void* base=getBase("libil2cpp.so");
    if(!base){ LOGI("libil2cpp not found"); return nullptr; }
    DobbyHook((void*)((uintptr_t)base+0x123456),(void*)hooked_ApplyDamage,(void**)&old_ApplyDamage);
    DobbyHook((void*)((uintptr_t)base+0x234567),(void*)hooked_GetSpeed,(void**)&old_GetSpeed);
    DobbyHook((void*)((uintptr_t)base+0x345678),(void*)hooked_CheckAmmo,(void**)&old_CheckAmmo);
    DobbyHook((void*)((uintptr_t)base+0x456789),(void*)hooked_GetRecoil,(void**)&old_GetRecoil);
    LOGI("Hooks installed");
    while(true) sleep(1);
    return nullptr;
}

void SetupStyle(){
    ImGuiStyle& s=ImGui::GetStyle();
    s.WindowRounding=10.f;
    s.FrameRounding=6.f;
    s.GrabRounding=6.f;
    s.WindowBorderSize=0.f;
    s.Colors[ImGuiCol_WindowBg]=ImVec4(0.06f,0.06f,0.08f,0.98f);
    s.Colors[ImGuiCol_FrameBg]=ImVec4(0.15f,0.15f,0.20f,1.f);
    s.Colors[ImGuiCol_CheckMark]=accent();
    s.Colors[ImGuiCol_SliderGrab]=accent();
    s.Colors[ImGuiCol_Button]=ImVec4(0.10f,0.10f,0.14f,1.f);
    s.Colors[ImGuiCol_ButtonHovered]=ImVec4(0.20f,0.05f,0.05f,1.f);
    s.Colors[ImGuiCol_Header]=ImVec4(0.15f,0.05f,0.05f,1.f);
    s.Colors[ImGuiCol_HeaderHovered]=ImVec4(0.25f,0.08f,0.08f,1.f);
    s.Colors[ImGuiCol_HeaderActive]=accent();
}

void DrawAimbot(){
    ImGui::Checkbox("Aimlock",&Aimlock);
    ImGui::Checkbox("Aim Head",&AimHead);
    ImGui::Checkbox("Aim Body",&AimBody);
    ImGui::Checkbox("Prediction",&Prediction);
    ImGui::Checkbox("Auto Fire",&AutoFire);
    ImGui::Checkbox("Wall Check",&WallCheck);
    ImGui::Checkbox("Team Check",&TeamCheck);
    ImGui::Checkbox("Visible Only",&VisibleOnly);
    ImGui::Checkbox("Silent Aim",&SilentAim);
    ImGui::Checkbox("Aim Key",&AimKey);
    ImGui::Checkbox("Aim Bone",&AimBone);
    ImGui::SliderFloat("FOV",&FOV,0.f,360.f,"%.0f deg");
    ImGui::SliderFloat("Smooth Aim",&SmoothAim,1.f,100.f,"%.0f");
    ImGui::SliderFloat("Prediction Factor",&PredictionFactor,0.f,200.f,"%.0f");
    ImGui::SliderFloat("Fire Delay",&FireDelay,0.f,500.f,"%.0f ms");
}

void DrawESP(){
    ImGui::Checkbox("ESP Master",&EspMaster);
    ImGui::Checkbox("Box ESP",&BoxEsp);
    ImGui::Checkbox("Line ESP",&LineEsp);
    ImGui::Checkbox("Name ESP",&NameEsp);
    ImGui::Checkbox("Distance ESP",&DistanceEsp);
    ImGui::Checkbox("Health ESP",&HealthEsp);
    ImGui::Checkbox("Skeleton ESP",&SkeletonEsp);
    ImGui::Checkbox("Chams",&Chams);
    ImGui::Checkbox("Weapon ESP",&WeaponEsp);
    ImGui::SliderFloat("Max Range",&MaxRange,50.f,2000.f,"%.0f m");
}

void DrawBullet(){
    ImGui::Checkbox("Infinite Ammo",&InfiniteAmmo);
    ImGui::Checkbox("No Reload",&NoReload);
    ImGui::Checkbox("No Recoil",&NoRecoil);
    ImGui::Checkbox("No Spread",&NoSpread);
    ImGui::Checkbox("Through Wall",&ThroughWall);
    ImGui::Checkbox("Instant Hit",&InstantHit);
    ImGui::Checkbox("Magic Bullet",&MagicBullet);
    ImGui::Checkbox("One Shot Kill",&OneShotKill);
    ImGui::SliderFloat("Damage Multiplier",&DamageMultiplier,1.f,10.f,"%.1fx");
    ImGui::SliderFloat("Fire Rate Boost",&FireRateBoost,1.f,5.f,"%.1fx");
}

void DrawPlayer(){
    ImGui::Checkbox("Spin While Run",&SpinWhileRun);
    ImGui::SliderFloat("Spin Speed",&SpinSpeed,1.f,20.f,"%.0fx");
    ImGui::Checkbox("Speed Hack",&SpeedHack);
    ImGui::SliderFloat("Speed Value",&SpeedValue,1.f,10.f,"%.1fx");
    ImGui::Checkbox("Super Jump",&SuperJump);
    ImGui::Checkbox("Fly Mode",&FlyMode);
    ImGui::Checkbox("God Mode",&GodMode);
    ImGui::Checkbox("No Fall Damage",&NoFallDamage);
    ImGui::Checkbox("Fast Heal",&FastHeal);
    ImGui::Checkbox("No Clip",&NoClip);
}

void DrawAntiBan(){
    ImGui::TextColored(accent(),"ANTI-BAN");
    ImGui::Separator();
    ImGui::Checkbox("Anti-Ban Master",&AntiBan);
    ImGui::Checkbox("Hide from /proc/maps",&HideMaps);
    ImGui::TextDisabled("Frida: %s",AntiBan?"Blocked":"Exposed");
    ImGui::TextDisabled("Debugger: %s",AntiBan?"Blocked":"Exposed");
}

void DrawMenu(){
    if(!g_ShowMenu) return;
    ImGui::SetNextWindowSize(ImVec2(370,560),ImGuiCond_FirstUseEver);
    ImGui::Begin("RUSTYCAIN",&g_ShowMenu,ImGuiWindowFlags_NoCollapse);
    ImGui::TextColored(accent(),"RUSTYCAIN");
    ImGui::SameLine();
    ImGui::TextDisabled("FULL MOD MENU");
    ImGui::SameLine();
    if(ImGui::SmallButton(g_AutoColor?"RAINBOW":"STATIC")) g_AutoColor=!g_AutoColor;
    ImGui::Separator();
    if(ImGui::BeginTabBar("MainTabs")){
        if(ImGui::BeginTabItem("AIMBOT")){ DrawAimbot(); ImGui::EndTabItem(); }
        if(ImGui::BeginTabItem("ESP")){ DrawESP(); ImGui::EndTabItem(); }
        if(ImGui::BeginTabItem("BULLET")){ DrawBullet(); ImGui::EndTabItem(); }
        if(ImGui::BeginTabItem("PLAYER")){ DrawPlayer(); ImGui::EndTabItem(); }
        if(ImGui::BeginTabItem("ANTI-BAN")){ DrawAntiBan(); ImGui::EndTabItem(); }
        ImGui::EndTabBar();
    }
    ImGui::End();
}

EGLBoolean (*old_eglSwapBuffers)(EGLDisplay,EGLSurface);
EGLBoolean hooked_eglSwapBuffers(EGLDisplay d,EGLSurface s){
    if(!ImGui::GetCurrentContext()){
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io=ImGui::GetIO();
        io.IniFilename=nullptr;
        ImGui_ImplOpenGL3_Init("#version 300 es");
        SetupStyle();
    }
    ImGui_ImplOpenGL3_NewFrame();
    ImGui::NewFrame();
    DrawMenu();
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    return old_eglSwapBuffers(d,s);
}

extern "C" jint JNI_OnLoad(JavaVM*,void*){
    LOGI("RUSTYCAIN loaded");
    pthread_t t1,t2;
    pthread_create(&t1,nullptr,hackThread,nullptr);
    pthread_create(&t2,nullptr,antiBanThread,nullptr);
    void* egl=dlopen("libEGL.so",RTLD_NOW);
    if(egl){
        void* a=dlsym(egl,"eglSwapBuffers");
        if(a) DobbyHook(a,(void*)hooked_eglSwapBuffers,(void**)&old_eglSwapBuffers);
    }
    return JNI_VERSION_1_6;
}