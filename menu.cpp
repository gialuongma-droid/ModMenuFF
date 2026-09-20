#include "menu.h"
#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_android.h"
#include <android/log.h>

#define LOG_TAG "Menu"
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)

static bool showMenu = true;

static bool aimlock = false, aimHead = false, aimBody = false;
static bool prediction = false, autoFire = false, wallCheck = false;
static bool teamCheck = false, visibleOnly = false, silentAim = false;
static bool aimKey = false, aimBone = false;
static float fov = 90.0f, smoothAim = 15.0f;
static float predictionFactor = 50.0f, fireDelay = 0.0f;

static bool espMaster = false, boxEsp = false, lineEsp = false;
static bool nameEsp = false, distanceEsp = false, healthEsp = false;
static bool skeletonEsp = false, chams = false, weaponEsp = false;
static float maxRange = 500.0f;

static bool infiniteAmmo = true, noReload = false, noRecoil = false;
static bool noSpread = false, throughWall = false, instantHit = false;
static bool magicBullet = false, oneShotKill = false;
static float damageMultiplier = 1.0f, fireRateBoost = 1.0f;

static bool spinWhileRun = false, speedHack = false, superJump = false;
static bool flyMode = false, godMode = false, noFallDamage = false;
static bool fastHeal = false, noClip = false;
static float spinSpeed = 5.0f, speedValue = 2.0f;

static bool antiBan = true, hideMaps = true;

static ImVec4 accent() {
    return ImVec4(1.0f, 0.13f, 0.13f, 1.0f);
}

void Menu_Init() {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.IniFilename = nullptr;

    ImGui_ImplAndroid_Init(nullptr);
    ImGui_ImplOpenGL3_Init("#version 300 es");

    ImGuiStyle& s = ImGui::GetStyle();
    s.WindowRounding = 10.0f;
    s.FrameRounding = 6.0f;
    s.GrabRounding = 6.0f;
    s.WindowBorderSize = 0.0f;
    s.Colors[ImGuiCol_WindowBg] = ImVec4(0.06f, 0.06f, 0.08f, 0.98f);
    s.Colors[ImGuiCol_FrameBg] = ImVec4(0.15f, 0.15f, 0.20f, 1.0f);
    s.Colors[ImGuiCol_CheckMark] = accent();
    s.Colors[ImGuiCol_SliderGrab] = accent();
    s.Colors[ImGuiCol_Header] = ImVec4(0.15f, 0.05f, 0.05f, 1.0f);
    s.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.25f, 0.08f, 0.08f, 1.0f);
    s.Colors[ImGuiCol_HeaderActive] = accent();
    s.Colors[ImGuiCol_Tab] = ImVec4(0.10f, 0.10f, 0.14f, 1.0f);
    s.Colors[ImGuiCol_TabHovered] = ImVec4(0.25f, 0.08f, 0.08f, 1.0f);
    s.Colors[ImGuiCol_TabActive] = accent();

    LOGD("Menu_Init done");
}

static void DrawAimbot() {
    ImGui::Checkbox("Aimlock", &aimlock);
    ImGui::Checkbox("Aim Head", &aimHead);
    ImGui::Checkbox("Aim Body", &aimBody);
    ImGui::Checkbox("Prediction", &prediction);
    ImGui::Checkbox("Auto Fire", &autoFire);
    ImGui::Checkbox("Wall Check", &wallCheck);
    ImGui::Checkbox("Team Check", &teamCheck);
    ImGui::Checkbox("Visible Only", &visibleOnly);
    ImGui::Checkbox("Silent Aim", &silentAim);
    ImGui::Checkbox("Aim Key", &aimKey);
    ImGui::Checkbox("Aim Bone", &aimBone);
    ImGui::SliderFloat("FOV", &fov, 0.0f, 360.0f, "%.0f");
    ImGui::SliderFloat("Smooth Aim", &smoothAim, 1.0f, 100.0f, "%.0f");
    ImGui::SliderFloat("Prediction Factor", &predictionFactor, 0.0f, 200.0f, "%.0f");
    ImGui::SliderFloat("Fire Delay", &fireDelay, 0.0f, 500.0f, "%.0f ms");
}

static void DrawEsp() {
    ImGui::Checkbox("ESP Master", &espMaster);
    ImGui::Checkbox("Box ESP", &boxEsp);
    ImGui::Checkbox("Line ESP", &lineEsp);
    ImGui::Checkbox("Name ESP", &nameEsp);
    ImGui::Checkbox("Distance ESP", &distanceEsp);
    ImGui::Checkbox("Health ESP", &healthEsp);
    ImGui::Checkbox("Skeleton ESP", &skeletonEsp);
    ImGui::Checkbox("Chams", &chams);
    ImGui::Checkbox("Weapon ESP", &weaponEsp);
    ImGui::SliderFloat("Max Range", &maxRange, 50.0f, 2000.0f, "%.0f m");
}

static void DrawBullet() {
    ImGui::Checkbox("Infinite Ammo", &infiniteAmmo);
    ImGui::Checkbox("No Reload", &noReload);
    ImGui::Checkbox("No Recoil", &noRecoil);
    ImGui::Checkbox("No Spread", &noSpread);
    ImGui::Checkbox("Through Wall", &throughWall);
    ImGui::Checkbox("Instant Hit", &instantHit);
    ImGui::Checkbox("Magic Bullet", &magicBullet);
    ImGui::Checkbox("One Shot Kill", &oneShotKill);
    ImGui::SliderFloat("Damage Multiplier", &damageMultiplier, 1.0f, 10.0f, "%.1fx");
    ImGui::SliderFloat("Fire Rate Boost", &fireRateBoost, 1.0f, 5.0f, "%.1fx");
}

static void DrawPlayer() {
    ImGui::Checkbox("Spin While Run", &spinWhileRun);
    ImGui::SliderFloat("Spin Speed", &spinSpeed, 1.0f, 20.0f, "%.0fx");
    ImGui::Checkbox("Speed Hack", &speedHack);
    ImGui::SliderFloat("Speed Value", &speedValue, 1.0f, 10.0f, "%.1fx");
    ImGui::Checkbox("Super Jump", &superJump);
    ImGui::Checkbox("Fly Mode", &flyMode);
    ImGui::Checkbox("God Mode", &godMode);
    ImGui::Checkbox("No Fall Damage", &noFallDamage);
    ImGui::Checkbox("Fast Heal", &fastHeal);
    ImGui::Checkbox("No Clip", &noClip);
}

static void DrawAntiBan() {
    ImGui::TextColored(accent(), "ANTI-BAN 12 LAYERS");
    ImGui::Separator();
    ImGui::Checkbox("Anti-Ban Master", &antiBan);
    ImGui::Checkbox("Hide from /proc/maps", &hideMaps);
    ImGui::TextDisabled("Ptrace: %s", antiBan ? "Blocked" : "Exposed");
    ImGui::TextDisabled("Debugger: %s", antiBan ? "Blocked" : "Exposed");
    ImGui::TextDisabled("Frida: %s", antiBan ? "Blocked" : "Exposed");
    ImGui::TextDisabled("Xposed: %s", antiBan ? "Blocked" : "Exposed");
    ImGui::TextDisabled("Hook: %s", antiBan ? "Blocked" : "Exposed");
    ImGui::TextDisabled("Emulator: %s", antiBan ? "Blocked" : "Exposed");
}

void Menu_Render() {
    if (!showMenu) return;

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplAndroid_NewFrame();
    ImGui::NewFrame();

    ImGui::SetNextWindowSize(ImVec2(500, 650), ImGuiCond_FirstUseEver);
    ImGui::Begin("RUSTYCAIN", &showMenu, ImGuiWindowFlags_NoCollapse);

    ImGui::TextColored(accent(), "RUSTYCAIN");
    ImGui::SameLine();
    ImGui::TextDisabled("FULL MOD MENU");
    ImGui::Separator();

    if (ImGui::BeginTabBar("MainTabs")) {
        if (ImGui::BeginTabItem("AIMBOT")) { DrawAimbot(); ImGui::EndTabItem(); }
        if (ImGui::BeginTabItem("ESP")) { DrawEsp(); ImGui::EndTabItem(); }
        if (ImGui::BeginTabItem("BULLET")) { DrawBullet(); ImGui::EndTabItem(); }
        if (ImGui::BeginTabItem("PLAYER")) { DrawPlayer(); ImGui::EndTabItem(); }
        if (ImGui::BeginTabItem("ANTI-BAN")) { DrawAntiBan(); ImGui::EndTabItem(); }
        ImGui::EndTabBar();
    }

    ImGui::End();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Menu_Shutdown() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplAndroid_Shutdown();
    ImGui::DestroyContext();
}