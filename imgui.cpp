// dear imgui, v1.93.0 WIP
// (main code and documentation)

// Help:
// - Call and read ImGui::ShowDemoWindow() in imgui_demo.cpp. All applications in examples/ are doing that.
// - Read top of imgui.cpp for more details, links and comments.
// - Add '#define IMGUI_DEFINE_MATH_OPERATORS' before including imgui.h (or in imconfig.h) to access courtesy maths operators for ImVec2 and ImVec4.

// Resources:
// - FAQ ........................ https://dearimgui.com/faq (in repository as docs/FAQ.md)
// - Homepage ................... https://github.com/ocornut/imgui
// - Releases & Changelog ....... https://github.com/ocornut/imgui/releases
// - Gallery .................... https://github.com/ocornut/imgui/issues?q=label%3Agallery (please post your screenshots/video there!)
// - Wiki ....................... https://github.com/ocornut/imgui/wiki (lots of good stuff there)
//   - Getting Started            https://github.com/ocornut/imgui/wiki/Getting-Started (how to integrate in an existing app by adding ~25 lines of code)
//   - Third-party Extensions     https://github.com/ocornut/imgui/wiki/Useful-Extensions (ImPlot & many more)
//   - Bindings/Backends          https://github.com/ocornut/imgui/wiki/Bindings (language bindings + backends for various tech/engines)
//   - Debug Tools                https://github.com/ocornut/imgui/wiki/Debug-Tools
//   - Glossary                   https://github.com/ocornut/imgui/wiki/Glossary
//   - Software using Dear ImGui  https://github.com/ocornut/imgui/wiki/Software-using-dear-imgui
// - Issues & support ........... https://github.com/ocornut/imgui/issues
// - Test Engine & Automation ... https://github.com/ocornut/imgui_test_engine (test suite, test engine to automate your apps)
// - Web version of the Demo .... https://pthom.github.io/imgui_explorer (w/ source code browser)

// For FIRST-TIME users having issues compiling/linking/running:
// please post in https://github.com/ocornut/imgui/discussions if you cannot find a solution in resources above.
// Everything else should be asked in 'Issues'! We are building a database of cross-linked knowledge there.
// Since 1.92, we encourage font loading questions to also be posted in 'Issues'.

// Copyright (c) 2014-2026 Omar Cornut
// Developed by Omar Cornut and every direct or indirect contributors to the GitHub.
// See LICENSE.txt for copyright and licensing details (standard MIT License).
// This library is free but needs your support to sustain development and maintenance.
// Businesses: you can support continued development via B2B invoiced technical support, maintenance and sponsoring contracts.
// PLEASE reach out at omar AT discohello DOT com. See https://github.com/ocornut/imgui/wiki/Funding
// Businesses: you can also purchase licenses for the Dear ImGui Automation/Test Engine.

// It is recommended that you don't modify imgui.cpp! It will become difficult for you to update the library.
// Note that 'ImGui::' being a namespace, you can add functions into the namespace from your own source files, without
// modifying imgui.h or imgui.cpp. You may include imgui_internal.h to access internal data structures, but it doesn't
// come with any guarantee of forward compatibility. Discussing your changes on the GitHub Issue Tracker may lead you
// to a better solution or official support for them.

/*

Index of this file:

DOCUMENTATION

- MISSION STATEMENT
- CONTROLS GUIDE
- PROGRAMMER GUIDE
  - READ FIRST
  - HOW TO UPDATE TO A NEWER VERSION OF DEAR IMGUI
  - GETTING STARTED WITH INTEGRATING DEAR IMGUI IN YOUR CODE/ENGINE
  - HOW A SIMPLE APPLICATION MAY LOOK LIKE
  - USING CUSTOM BACKEND / CUSTOM ENGINE
- API BREAKING CHANGES (read me when you update!)
- FREQUENTLY ASKED QUESTIONS (FAQ)
  - Read all answers online: https://www.dearimgui.com/faq, or in docs/FAQ.md (with a Markdown viewer)

CODE
(search for "[SECTION]" in the code to find them)

// [SECTION] INCLUDES
// [SECTION] FORWARD DECLARATIONS
// [SECTION] CONTEXT AND MEMORY ALLOCATORS
// [SECTION] USER FACING STRUCTURES (ImGuiStyle, ImGuiIO, ImGuiPlatformIO)
// [SECTION] MISC HELPERS/UTILITIES (Geometry functions)
// [SECTION] MISC HELPERS/UTILITIES (String, Format, Hash functions)
// [SECTION] MISC HELPERS/UTILITIES (File functions)
// [SECTION] MISC HELPERS/UTILITIES (ImText* functions)
// [SECTION] MISC HELPERS/UTILITIES (Color functions)
// [SECTION] ImGuiStorage
// [SECTION] ImGuiTextFilter
// [SECTION] ImGuiTextBuffer, ImGuiTextIndex
// [SECTION] ImGuiListClipper
// [SECTION] STYLING
// [SECTION] RENDER HELPERS
// [SECTION] INITIALIZATION, SHUTDOWN
// [SECTION] MAIN CODE (most of the code! lots of stuff, needs tidying up!)
// [SECTION] FONTS, TEXTURES
// [SECTION] ID STACK
// [SECTION] INPUTS
// [SECTION] ERROR CHECKING, STATE RECOVERY
// [SECTION] ITEM SUBMISSION
// [SECTION] LAYOUT
// [SECTION] SCROLLING
// [SECTION] TOOLTIPS
// [SECTION] POPUPS
// [SECTION] WINDOW FOCUS
// [SECTION] KEYBOARD/GAMEPAD NAVIGATION
// [SECTION] DRAG AND DROP
// [SECTION] LOGGING/CAPTURING
// [SECTION] SETTINGS
// [SECTION] LOCALIZATION
// [SECTION] VIEWPORTS, PLATFORM WINDOWS
// [SECTION] PLATFORM DEPENDENT HELPERS
// [SECTION] METRICS/DEBUGGER WINDOW
// [SECTION] DEBUG LOG WINDOW
// [SECTION] OTHER DEBUG TOOLS (ITEM PICKER, ID STACK TOOL)

*/

//-----------------------------------------------------------------------------
// DOCUMENTATION
//-----------------------------------------------------------------------------

/*

 MISSION STATEMENT
 =================

 - Easy to use to create code-driven and data-driven tools.
 - Easy to use to create ad hoc short-lived tools and long-lived, more elaborate tools.
 - Easy to hack and improve.
 - Minimize setup and maintenance.
 - Minimize state storage on user side.
 - Minimize state synchronization.
 - Portable, minimize dependencies, run on target (consoles, phones, etc.).
 - Efficient runtime and memory consumption.

 Designed primarily for developers and content-creators, not the typical end-user!
 Some of the current weaknesses (which we aim to address in the future) includes:

 - Doesn't look fancy by default.
 - Limited layout features, intricate layouts are typically crafted in code.


 CONTROLS GUIDE
 ==============

 - MOUSE CONTROLS
   - Mouse wheel:                   Scroll vertically.
   - Shift+Mouse wheel:             Scroll horizontally.
   - Click [X]:                     Close a window, available when 'bool* p_open' is passed to ImGui::Begin().
   - Click ^, Double-Click title:   Collapse window.
   - Drag on corner/border:         Resize window (double-click to auto fit window to its contents).
   - Drag on any empty space:       Move window (unless io.ConfigWindowsMoveFromTitleBarOnly = true).
   - Left-click outside popup:      Close popup stack (right-click over underlying popup: Partially close popup stack).

 - TEXT EDITOR
   - Hold Shift or Drag Mouse:      Select text.
   - Ctrl+Left/Right:               Word jump.
   - Ctrl+Shift+Left/Right:         Select words.
   - Ctrl+A or Double-Click:        Select All.
   - Ctrl+X, Ctrl+C, Ctrl+V:        Use OS clipboard.
   - Ctrl+Z                         Undo.
   - Ctrl+Y or Ctrl+Shift+Z:        Redo.
   - ESCAPE:                        Revert text to its original value.
   - On macOS, controls are automatically adjusted to match standard macOS text editing and behaviors.
     (for 99% of shortcuts, Ctrl is replaced by Cmd on macOS).

 - KEYBOARD CONTROLS
   - Basic:
     - Tab, Shift+Tab               Cycle through text editable fields.
     - Ctrl+Tab, Ctrl+Shift+Tab     Cycle through windows.
     - Ctrl+Click                   Input text into a Slider or Drag widget.
   - Extended features with `io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard`:
     - Tab, Shift+Tab:              Cycle through every items.
     - Arrow keys                   Move through items using directional navigation. Tweak value.
     - Arrow keys + Alt, Shift      Tweak slower, tweak faster (when using arrow keys).
     - Enter                        Activate item (prefer text input when possible).
     - Space                        Activate item (prefer tweaking with arrows when possible).
     - Escape                       Deactivate item, leave child window, close popup.
     - Page Up, Page Down           Previous page, next page.
     - Home, End                    Scroll to top, scroll to bottom.
     - Alt                          Toggle between scrolling layer and menu layer.
     - Ctrl+Tab then Ctrl+Arrows    Move window. Hold Shift to resize instead of moving.
     - Menu or Shift+F10            Open context menu.
   - Output when ImGuiConfigFlags_NavEnableKeyboard set,
     - io.WantCaptureKeyboard flag is set when keyboard is claimed.
     - io.NavActive: true when a window is focused and it doesn't have the ImGuiWindowFlags_NoNavInputs flag set.
     - io.NavVisible: true when the navigation cursor is visible (usually goes to back false when mouse is used).

 - GAMEPAD CONTROLS
   - Enable with 'io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad'.
   - Particularly useful to use Dear ImGui on a console system (e.g. PlayStation, Switch, Xbox) without a mouse!
   - Download controller mapping PNG/PSD at http://dearimgui.com/controls_sheets
   - Backend support: backend needs to:
      - Set 'io.BackendFlags |= ImGuiBackendFlags_HasGamepad' + call io.AddKeyEvent/AddKeyAnalogEvent() with ImGuiKey_Gamepad_XXX keys.
      - For analog values (0.0f to 1.0f), backend is responsible to handling a dead-zone and rescaling inputs accordingly.
        Backend code will probably need to transform your raw inputs (such as e.g. remapping your 0.2..0.9 raw input range to 0.0..1.0 imgui range, etc.).
   - If you need to share inputs between your game and the Dear ImGui interface, the easiest approach is to go all-or-nothing,
     with a buttons combo to toggle the target. Please reach out if you think the game vs navigation input sharing could be improved.

 - REMOTE INPUTS SHARING & MOUSE EMULATION
   - PS4/PS5 users: Consider emulating a mouse cursor with DualShock touch pad or a spare analog stick as a mouse-emulation fallback.
   - Consoles/Tablet/Phone users: Consider using a Synergy 1.x server (on your PC) + run examples/libs/synergy/uSynergy.c (on your console/tablet/phone app)
     in order to share your PC mouse/keyboard.
   - See https://github.com/ocornut/imgui/wiki/Useful-Extensions#remoting for other remoting solutions.
   - On a TV/console system where readability may be lower or mouse inputs may be awkward, you may want to set the io.ConfigNavMoveSetMousePos flag.
     Enabling io.ConfigNavMoveSetMousePos + ImGuiBackendFlags_HasSetMousePos instructs Dear ImGui to move your mouse cursor along with navigation movements.
     When enabled, the NewFrame() function may alter 'io.MousePos' and set 'io.WantSetMousePos' to notify you that it wants the mouse cursor to be moved.
     When that happens your backend NEEDS to move the OS or underlying mouse cursor on the next frame. Some of the backends in examples/ do that.
     (If you set the NavEnableSetMousePos flag but don't honor 'io.WantSetMousePos' properly, Dear ImGui will misbehave as it will see your mouse moving back & forth!)
     (In a setup when you may not have easy control over the mouse cursor, e.g. uSynergy.c doesn't expose moving remote mouse cursor, you may want
     to set a boolean to ignore your other external mouse positions until the external source is moved again.)


 PROGRAMMER GUIDE
 ================

 READ FIRST
 ----------
 - Remember to check the wonderful Wiki: https://github.com/ocornut/imgui/wiki
 - Your code creates the UI every frame of your application loop, if your code doesn't run the UI is gone!
   The UI can be highly dynamic, there are no construction or destruction steps, less superfluous
   data retention on your side, less state duplication, less state synchronization, fewer bugs.
 - Call and read ImGui::ShowDemoWindow() for demo code demonstrating most features.
   Or browse pthom's online imgui_explorer: https://pthom.github.io/imgui_explorer for a web version w/ source code browser.
 - The library is designed to be built from sources. Avoid pre-compiled binaries and packaged versions. See imconfig.h to configure your build.
 - Dear ImGui is an implementation of the IMGUI paradigm (immediate-mode graphical user interface, a term coined by Casey Muratori).
   You can learn about IMGUI principles at http://www.johno.se/book/imgui.html, http://mollyrocket.com/861 & more links in Wiki.
 - Dear ImGui is a "single pass" rasterizing implementation of the IMGUI paradigm, aimed at ease of use and high-performances.
   For every application frame, your UI code will be called only once. This is in contrast to e.g. Unity's implementation of an IMGUI,
   where the UI code is called multiple times ("multiple passes") from a single entry point. There are pros and cons to both approaches.
 - Our origin is on the top-left. In axis aligned bounding boxes, Min = top-left, Max = bottom-right.
 - Please make sure you have asserts enabled (IM_ASSERT redirects to assert() by default, but can be redirected).
   If you get an assert, read the messages and comments around the assert.
 - This codebase aims to be highly optimized:
   - A typical idle frame should never call malloc/free.
   - We rely on a maximum of constant-time or O(N) algorithms. Limiting searches/scans as much as possible.
   - We put particular energy in making sure performances are decent with typical "Debug" build settings as well.
     Which mean we tend to avoid over-relying on "zero-cost abstraction" as they aren't zero-cost at all.
 - This codebase aims to be both highly opinionated and highly flexible:
   - This code works because of the things it choose to solve or not solve.
   - C++: this is a pragmatic C-ish codebase: we don't use fancy C++ features, we don't include C++ headers,
     and ImGui:: is a namespace. We rarely use member functions (and when we did, I am mostly regretting it now).
     This is to increase compatibility, increase maintainability and facilitate use from other languages.
   - C++: ImVec2/ImVec4 do not expose math operators by default, because it is expected that you use your own math types.
     See FAQ "How can I use my own math types instead of ImVec2/ImVec4?" for details about setting up imconfig.h for that.
     We can can optionally export math operators for ImVec2/ImVec4 using IMGUI_DEFINE_MATH_OPERATORS, which we use internally.
   - C++: pay attention that ImVector<> manipulates plain-old-data and does not honor construction/destruction
     (so don't use ImVector in your code or at our own risk!).
   - Building: We don't use nor mandate a build system for the main library.
     This is in an effort to ensure that it works in the real world aka with any esoteric build setup.
     This is also because providing a build system for the main library would be of little-value.
     The build problems are almost never coming from the main library but from specific backends.


 HOW TO UPDATE TO A NEWER VERSION OF DEAR IMGUI
 ----------------------------------------------
 - Update submodule or copy/overwrite every file.
 - About imconfig.h:
   - You may modify your copy of imconfig.h, in this case don't overwrite it.
   - or you may locally branch to modify imconfig.h and merge/rebase latest.
   - or you may '#define IMGUI_USER_CONFIG "my_config_file.h"' globally from your build system to
     specify a custom path for your imconfig.h file and instead not have to modify the default one.

 - Overwrite all the sources files except for imconfig.h (if you have modified your copy of imconfig.h)
 - Or maintain your own branch where you have imconfig.h modified as a top-most commit which you can regularly rebase over "master".
 - You can also use '#define IMGUI_USER_CONFIG "my_config_file.h" to redirect configuration to your own file.
 - Read the "API BREAKING CHANGES" section (below). This is where we list occasional API breaking changes.
   If a function/type has been renamed / or marked obsolete, try to fix the name in your code before it is permanently removed
   from the public API. If you have a problem with a missing function/symbols, search for its name in the code, there will
   likely be a comment about it. Please report any issue to the GitHub page!
 - To find out usage of old API, you can add '#define IMGUI_DISABLE_OBSOLETE_FUNCTIONS' in your configuration file.
 - Try to keep your copy of Dear ImGui reasonably up to date!


 GETTING STARTED WITH INTEGRATING DEAR IMGUI IN YOUR CODE/ENGINE
 ---------------------------------------------------------------
 - See https://github.com/ocornut/imgui/wiki/Getting-Started.
 - Run and study the examples and demo in imgui_demo.cpp to get acquainted with the library.
 - In the majority of cases you should be able to use unmodified backends files available in the backends/ folder.
 - Add the Dear ImGui source files + selected backend source files to your projects or using your preferred build system.
   It is recommended you build and statically link the .cpp files as part of your project and NOT as a shared library (DLL).
 - You can later customize the imconfig.h file to tweak some compile-time behavior, such as integrating Dear ImGui types with your own maths types.
 - When using Dear ImGui, your programming IDE is your friend: follow the declaration of variables, functions and types to find comments about them.
 - Dear ImGui never touches or knows about your GPU state. The only function that knows about GPU is the draw function that you provide.
   Effectively it means you can create widgets at any time in your code, regardless of considerations of being in "update" vs "render"
   phases of your own application. All rendering information is stored into command-lists that you will retrieve after calling ImGui::Render().
 - Refer to the backends and demo applications in the examples/ folder for instruction on how to setup your code.
 - If you are running over a standard OS with a common graphics API, you should be able to use unmodified imgui_impl_*** files from the examples/ folder.


 HOW A SIMPLE APPLICATION MAY LOOK LIKE
 --------------------------------------

 USING THE EXAMPLE BACKENDS (= imgui_impl_XXX.cpp files from the backends/ folder).
 The sub-folders in examples/ contain examples applications following this structure.

     // Application init: create a dear imgui context, setup some options, load fonts
     ImGui::CreateContext();
     ImGuiIO& io = ImGui::GetIO();
     // TODO: Set optional io.ConfigFlags values, e.g. 'io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard' to enable keyboard controls.
     // TODO: Fill optional fields of the io structure later.
     // TODO: Load TTF/OTF fonts if you don't want to use the default font.

     // Initialize helper Platform and Renderer backends (here we are using imgui_impl_win32.cpp and imgui_impl_dx11.cpp)
     ImGui_ImplWin32_Init(hwnd);
     ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);

     // Application main loop
     while (true)
     {
         // Feed inputs to dear imgui, start new frame
         ImGui_ImplDX11_NewFrame();
         ImGui_ImplWin32_NewFrame();
         ImGui::NewFrame();

         // Any application code here
         ImGui::Text("Hello, world!");

         // Render dear imgui into framebuffer
         ImGui::Render();
         ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
         g_pSwapChain->Present(1, 0);
     }

     // Shutdown
     ImGui_ImplDX11_Shutdown();
     ImGui_ImplWin32_Shutdown();
     ImGui::DestroyContext();

 To decide whether to dispatch mouse/keyboard inputs to Dear ImGui to the rest of your application,
 you should read the 'io.WantCaptureMouse', 'io.WantCaptureKeyboard' and 'io.WantTextInput' flags!
 Please read the FAQ entry "How can I tell whether to dispatch mouse/keyboard to Dear ImGui or my application?" about this.


USING CUSTOM BAC