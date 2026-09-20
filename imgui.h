// dear imgui, v1.93.0 WIP
// (headers)

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
// EVERYTHING ELSE should be asked in 'Issues'! We are building a database of cross-linked knowledge there.
// Since 1.92, we encourage font loading questions to also be posted in 'Issues'.

// Library Version
// (Integer encoded as XYYZZ for use in #if preprocessor conditionals, e.g. '#if IMGUI_VERSION_NUM >= 12345')
#define IMGUI_VERSION       "1.93.0 WIP"
#define IMGUI_VERSION_NUM   19297
#define IMGUI_HAS_TABLE             // Added BeginTable() - from IMGUI_VERSION_NUM >= 18000
#define IMGUI_HAS_TEXTURES          // Added ImGuiBackendFlags_RendererHasTextures - from IMGUI_VERSION_NUM >= 19198

/*

Index of this file:
// [SECTION] Header mess
// [SECTION] Forward declarations and basic types
// [SECTION] Texture identifiers (ImTextureID, ImTextureRef)
// [SECTION] Dear ImGui end-user API functions
// [SECTION] Flags & Enumerations
// [SECTION] Tables API flags and structures (ImGuiTableFlags, ImGuiTableColumnFlags, ImGuiTableRowFlags, ImGuiTableBgTarget, ImGuiTableSortSpecs, ImGuiTableColumnSortSpecs)
// [SECTION] Helpers: Debug log, Memory allocations macros, ImVector<>
// [SECTION] ImGuiStyle
// [SECTION] ImGuiIO
// [SECTION] Misc data structures (ImGuiInputTextCallbackData, ImGuiSizeCallbackData, ImGuiPayload)
// [SECTION] Helpers (ImGuiOnceUponAFrame, ImGuiTextFilter, ImGuiTextBuffer, ImGuiStorage, ImGuiListClipper, Math Operators, ImColor)
// [SECTION] Multi-Select API flags and structures (ImGuiMultiSelectFlags, ImGuiMultiSelectIO, ImGuiSelectionRequest, ImGuiSelectionBasicStorage, ImGuiSelectionExternalStorage)
// [SECTION] Drawing API (ImDrawCallback, ImDrawCmd, ImDrawIdx, ImDrawVert, ImDrawChannel, ImDrawListSplitter, ImDrawFlags, ImDrawListFlags, ImDrawList, ImDrawData)
// [SECTION] Texture API (ImTextureFormat, ImTextureStatus, ImTextureRect, ImTextureData)
// [SECTION] Font API (ImFontConfig, ImFontGlyph, ImFontGlyphRangesBuilder, ImFontAtlasFlags, ImFontAtlas, ImFontBaked, ImFont)
// [SECTION] Viewports (ImGuiViewportFlags, ImGuiViewport)
// [SECTION] ImGuiPlatformIO + other Platform Dependent Interfaces (ImGuiPlatformImeData)
// [SECTION] Obsolete functions and types

*/

#pragma once

// Configuration file with compile-time options
// (edit imconfig.h or '#define IMGUI_USER_CONFIG "myfilename.h" from your build system)
#ifdef IMGUI_USER_CONFIG
#include IMGUI_USER_CONFIG
#endif
#include "imconfig.h"

#ifndef IMGUI_DISABLE

//-----------------------------------------------------------------------------
// [SECTION] Header mess
//-----------------------------------------------------------------------------

// Includes
#include <float.h>                  // FLT_MIN, FLT_MAX
#include <stdarg.h>                 // va_list, va_start, va_end
#include <stddef.h>                 // ptrdiff_t, NULL
#include <string.h>                 // memset, memmove, memcpy, strlen, strchr, strcpy, strcmp

// Define attributes of all API symbols declarations (e.g. for DLL under Windows)
// IMGUI_API is used for core imgui functions, IMGUI_IMPL_API is used for the default backends files (imgui_impl_xxx.h)
// Using dear imgui via a shared library is not recommended: we don't guarantee backward nor forward ABI compatibility + this is a call-heavy library and function call overhead adds up.
#ifndef IMGUI_API
#define IMGUI_API
#endif
#ifndef IMGUI_IMPL_API
#define IMGUI_IMPL_API              IMGUI_API
#endif

// Helper Macros
// (note: compiling with NDEBUG will usually strip out assert() to nothing, which is NOT recommended because we use asserts to notify of programmer mistakes.)
#ifndef IM_ASSERT
#include <assert.h>
#define IM_ASSERT(_EXPR)            assert(_EXPR)                               // You can override the default assert handler by editing imconfig.h
#endif
#define IM_COUNTOF(_ARR)            ((int)(sizeof(_ARR) / sizeof(*(_ARR))))     // Size of a static C-style array. Don't use on pointers!
#define IM_UNUSED(_VAR)             ((void)(_VAR))                              // Used to silence "unused variable warnings". Often useful as asserts may be stripped out from final builds.
#define IM_STRINGIFY_HELPER(_EXPR)  #_EXPR
#define IM_STRINGIFY(_EXPR)         IM_STRINGIFY_HELPER(_EXPR)                  // Preprocessor idiom to stringify e.g. an integer or a macro.

// Check that version and structures layouts are matching between compiled imgui code and caller. Read comments above DebugCheckVersionAndDataLayout() for details.
#define IMGUI_CHECKVERSION()        ImGui::DebugCheckVersionAndDataLayout(IMGUI_VERSION, sizeof(ImGuiIO), sizeof(ImGuiStyle), sizeof(ImVec2), sizeof(ImVec4), sizeof(ImDrawVert), sizeof(ImDrawIdx))

// Helper Macros - IM_FMTARGS, IM_FMTLIST: Apply printf-style warnings to our formatting functions.
// (MSVC provides an equivalent mechanism via SAL Annotations but it requires the macros in a different
//  location. e.g. #include <sal.h> + void myprintf(_Printf_format_string_ const char* format, ...),
//  and only works when using Code Analysis, rather than just normal compiling).
// (see https://github.com/ocornut/imgui/issues/8871 for a patch to enable this for MSVC's Code Analysis)
#if !defined(IMGUI_USE_STB_SPRINTF) && defined(__MINGW32__) && !defined(__clang__)
#define IM_FMTARGS(FMT)             __attribute__((format(gnu_printf, FMT, FMT+1)))
#define IM_FMTLIST(FMT)             __attribute__((format(gnu_printf, FMT, 0)))
#elif !defined(IMGUI_USE_STB_SPRINTF) && (defined(__clang__) || defined(__GNUC__))
#define IM_FMTARGS(FMT)             __attribute__((format(printf, FMT, FMT+1)))
#define IM_FMTLIST(FMT)             __attribute__((format(printf, FMT, 0)))
#else
#define IM_FMTARGS(FMT)
#define IM_FMTLIST(FMT)
#endif

// Disable some of MSVC most aggressive Debug runtime checks in function header/footer (used in some simple/low-level functions)
#if defined(_MSC_VER) && !defined(__clang__)  && !defined(__INTEL_COMPILER) && !defined(IMGUI_DEBUG_PARANOID)
#define IM_MSVC_RUNTIME_CHECKS_OFF      __pragma(runtime_checks("",off))     __pragma(check_stack(off)) __pragma(strict_gs_check(push,off))
#define IM_MSVC_RUNTIME_CHECKS_RESTORE  __pragma(runtime_checks("",restore)) __pragma(check_stack())    __pragma(strict_gs_check(pop))
#else
#define IM_MSVC_RUNTIME_CHECKS_OFF
#define IM_MSVC_RUNTIME_CHECKS_RESTORE
#endif

// Alternative to using a .natstepfilter file or other scripts in misc/debuggers/ to skip debug-stepping selected trivial functions.
// If you get a compiler error or warning related to use, please report it to us!
#if (defined(__clang__) && (__clang_major__ >= 7)) || (defined(__GNUC__) && (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 8)))
#define IM_NODEBUGSTEP      [[gnu::artificial]]
#elif defined(_MSC_VER) && (_MSC_VER >= 1915)
#define IM_NODEBUGSTEP      __declspec(non_user_code)
#else
#define IM_NODEBUGSTEP
#endif

// Warnings
#ifdef _MSC_VER
#pragma warning (push)
#pragma warning (disable: 26495)    // [Static Analyzer] Variable 'XXX' is uninitialized. Always initialize a member variable (type.6).
#endif
#if defined(__clang__)
#pragma clang diagnostic push
#if __has_warning("-Wunknown-warning-option")
#pragma clang diagnostic ignored "-Wunknown-warning-option"         // warning: unknown warning group 'xxx'
#endif
#pragma clang diagnostic ignored "-Wunknown-pragmas"                // warning: unknown warning group 'xxx'
#pragma clang diagnostic ignored "-Wold-style-cast"                 // warning: use of old-style cast
#pragma clang diagnostic ignored "-Wfloat-equal"                    // warning: comparing floating point with == or != is unsafe
#pragma clang diagnostic ignored "-Wzero-as-null-pointer-constant"  // warning: zero as null pointer constant
#pragma clang diagnostic ignored "-Wreserved-identifier"            // warning: identifier '_Xxx' is reserved because it starts with '_' followed by a capital letter
#pragma clang diagnostic ignored "-Wunsafe-buffer-usage"            // warning: 'xxx' is an unsafe pointer used for buffer access
#pragma clang diagnostic ignored "-Wnontrivial-memaccess"           // warning: first argument in call to 'memset' is a pointer to non-trivially copyable type
#elif defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpragmas"                          // warning: unknown option after '#pragma GCC diagnostic' kind
#pragma GCC diagnostic ignored "-Wfloat-equal"                      // warning: comparing floating-point with '==' or '!=' is unsafe
#pragma GCC diagnostic ignored "-Wclass-memaccess"                  // [__GNUC__ >= 8] warning: 'memset/memcpy' clearing/writing an object of type 'xxxx' with no trivial copy-assignment; use assignment or value-initialization instead
#endif

//-----------------------------------------------------------------------------
// [SECTION] Forward declarations and basic types
//-----------------------------------------------------------------------------

// Scalar data types
typedef unsigned int        ImGuiID;// A unique ID used by widgets (typically the result of hashing a stack of string)
typedef signed char         ImS8;   // 8-bit signed integer
typedef unsigned char       ImU8;   // 8-bit unsigned integer
typedef signed short        ImS16;  // 16-bit signed integer
typedef unsigned short      ImU16;  // 16-bit unsigned integer
typedef signed int          ImS32;  // 32-bit signed integer == int
typedef unsigned int        ImU32;  // 32-bit unsigned integer (often used to store packed colors)
typedef signed   long long  ImS64;  // 64-bit signed integer
typedef unsigned long long  ImU64;  // 64-bit unsigned integer

// Forward declarations: ImDrawList, ImFontAtlas layer
struct ImDrawChannel;               // Temporary storage to output draw commands out of order, used by ImDrawListSplitter and ImDrawList::ChannelsSplit()
struct ImDrawCmd;                   // A single draw command within a parent ImDrawList (generally maps to 1 GPU draw call, unless it is a callback)
struct ImDrawData;                  // All draw command lists required to render the frame + pos/size coordinates to use for the projection matrix.
struct ImDrawList;                  // A single draw command list (generally one per window, conceptually you may see this as a dynamic "mesh" builder)
struct ImDrawListSharedData;        // Data shared among multiple draw lists (typically owned by parent ImGui context, but you may create one yourself)
struct ImDrawListSplitter;          // Helper to split a draw list into different layers which can be drawn into out of order, then flattened back.
struct ImDrawVert;                  // A single vertex (pos + uv + col = 20 bytes by default. Override layout with IMGUI_OVERRIDE_DRAWVERT_STRUCT_LAYOUT)
struct ImFont;                      // Runtime data for a single font within a parent ImFontAtlas
struct ImFontAtlas;                 // Runtime data for multiple fonts, bake multiple fonts into a single texture, TTF/OTF font loader
struct ImFontAtlasBuilder;          // Opaque storage for building a ImFontAtlas
struct ImFontAtlasRect;             // Output of ImFontAtlas::GetCustomRect() when using custom rectangles.
struct ImFontBaked;                 // Baked data for a ImFont at a given size.
struct ImFontConfig;                // Configuration data when adding a font or merging fonts
struct ImFontGlyph;                 // A single font glyph (code point + coordinates within in ImFontAtlas + offset)
struct ImFontGlyphRangesBuilder;    // Helper to build glyph ranges from text/string data
struct ImFontLoader;                // Opaque interface to a font loading backend (stb_truetype, FreeType etc.).
struct ImTextureData;               // Specs and pixel storage for a texture used by Dear ImGui.
struct ImTextureRect;               // Coordinates of a rectangle within a texture.
struct ImColor;                     // Helper functions to create a color that can be converted to either u32 or float4 (*OBSOLETE* please avoid using)

// Forward declarations: ImGui layer
struct ImGuiContext;                // Dear ImGui context (opaque structure, unless including imgui_internal.h)
struct ImGuiIO;                     // Main configuration and I/O between your application and ImGui (also see: ImGuiPlatformIO)
struct ImGuiInputTextCallbackData;  // Shared state of InputText() when using custom ImGuiInputTextCallback (rare/advanced use)
struct ImGuiKeyData;                // Storage for ImGuiIO and IsKeyDown(), IsKeyPressed() etc functions.
struct ImGuiListClipper;            // Helper to manually clip large list of items
struct ImGuiMultiSelectIO;          // Structure to interact with a BeginMultiSelect()/EndMultiSelect() block
struct ImGuiOnceUponAFrame;         // Helper for running a block of code not more than once a frame
struct ImGuiPayload;                // User data payload for drag and drop operations
struct ImGuiPlatformIO;             // Interface between platform/renderer backends and ImGui (e.g. Clipboard, IME hooks). Extends ImGuiIO. In docking branch, this gets extended to support multi-viewports.
struct ImGuiPlatformImeData;        // Platform IME data for io.PlatformSetImeDataFn() function.
struct ImGuiSelectionBasicStorage;  // Optional helper to store multi-selection state + apply multi-selection requests.
struct ImGuiSelectionExternalStorage;//Optional helper to apply multi-selection requests to existing randomly accessible storage.
struct ImGuiSelectionRequest;       // A selection request (stored in ImGuiMultiSelectIO)
struct ImGuiSizeCallbackData;       // Callback data when using SetNextWindowSizeConstraints() (rare/advanced use)
struct ImGuiStorage;                // Helper for key->value storage (container sorted by key)
struct ImGuiStoragePair;            // Helper for key->value storage (pair)
struct ImGuiStyle;                  // Runtime data for styling/colors
struct ImGuiTableSortSpecs;         // Sorting specifications for a table (often handling sort specs for a single column, occasionally more)
struct ImGuiTableColumnSortSpecs;   // Sorting specification for one column of a table
struct ImGuiTextBuffer;             // Helper to hold and append into a text buffer (~string builder)
struct ImGuiTextFilter;             // Helper to parse and apply text filters (e.g. "aaaaa[,bbbbb][,ccccc]")
struct ImGuiViewport;               // A Platform Window (always only one in 'master' branch), in the future may represent Platform Monitor

// Enumerations
// - We don't use strongly typed enums much because they add constraints (can't extend in private code, can't store typed in bit fields, extra casting on iteration)
// - Tip: Use your programming IDE navigation facilities on the names in the _central column_ below to find the actual flags/enum lists!
//   - In Visual Studio: Ctrl+Comma ("Edit.GoToAll") can follow symbols inside comments, whereas Ctrl+F12 ("Edit.GoToImplementation") cannot.
//   - In Visual Studio w/ Visual Assist installed: Alt+G ("VAssistX.GoToImplementation") can also follow symbols inside comments.
//   - In VS Code, CLion, etc.: Ctrl+Click can follow symbols inside comments.
enum ImGuiDir : int;                // -> enum ImGuiDir              // Enum: A cardinal direction (Left, Right, Up, Down)
enum ImGuiKey : int;                // -> enum ImGuiKey              // Enum: A key identifier (ImGuiKey_XXX or ImGuiMod_XXX value)
enum ImGuiMouseSource : int;        // -> enum ImGuiMouseSource      // Enum; A mouse input source identifier (Mouse, TouchScreen, Pen)
enum ImGuiSortDirection : ImU8;     // -> enum ImGuiSortDirection    // Enum: A sorting direction (ascending or descending)
typedef int ImGuiCol;               // -> enum ImGuiCol_             // Enum: A color identifier for styling
typedef int ImGuiCond;              // -> enum ImGuiCond_            // Enum: A condition for many Set*() functions
typedef int ImGuiDataType;          // -> enum ImGuiDataType_        // Enum: A primary data type
typedef int ImGuiMouseButton;       // -> enum ImGuiMouseButton_     // Enum: A mouse button identifier (0=left, 1=right, 2=middle)
typedef int ImGuiMouseCursor;       // -> enum ImGuiMouseCursor_     // Enum: A mouse cursor shape
typedef int ImGuiStyleVar;          // -> enum ImGuiStyleVar_        // Enum: A variable identifier for styling
typedef int ImGuiTableBgTarget;     // -> enum ImGuiTableBgTarget_   // Enum: A color target for TableSetBgColor()

// Flags (declared as int to allow using as flags without overhead, and to not pollute the top of this file)
// - Tip: Use your programming IDE navigation facilities on the names in the _central column_ below to find the actual flags/enum lists!
//   - In Visual Studio: Ctrl+Comma ("Edit.GoToAll") can follow symbols inside comments, whereas Ctrl+F12 ("Edit.GoToImplementation") cannot.
//   - In Visual Studio w/ Visual Assist installed: Alt+G ("VAssistX.GoToImplementation") can also follow symbols inside comments.
//   - In VS Code, CLion, etc.: Ctrl+Click can follow symbols inside comments.
typedef int ImDrawFlags;            // -> enum ImDrawFlags_          // Flags: for ImDrawList functions
typedef int ImDrawListFlags;        // -> enum ImDrawListFlags_      // Flags: for ImDrawList instance
typedef int ImDrawTextFlags;        // -> enum ImDrawTextFlags_      // Internal, do not use!
typedef int ImFontFlags;            // -> enum ImFontFlags_          // Flags: for ImFont
typedef int ImFontAtlasFlags;       // -> enum ImFontAtlasFlags_     // Flags: for ImFontAtlas
typedef int ImGuiBackendFlags;      // -> enum ImGuiBackendFlags_    // Flags: for io.BackendFlags
typedef int ImGuiButtonFlags;       // -> enum ImGuiButtonFlags_     // Flags: for InvisibleButton()
typedef int ImGuiChildFlags;        // -> enum ImGuiChildFlags_      // Flags: for BeginChild()
typedef int ImGuiColorEditFlags;    // -> enum ImGuiColorEditFlags_  // Flags: for ColorEdit4(), ColorPicker4(), io.ConfigColorEditFlags etc.
typedef int ImGuiConfigFlags;       // -> enum ImGuiConfigFlags_     // Flags: for io.ConfigFlags
typedef int ImGuiComboFlags;        //