// imgui-notify by patrickcjk
// https://github.com/patrickcjk/imgui-notify

#ifndef IMGUI_NOTIFY
#define IMGUI_NOTIFY

// Project treats MSVC security deprecation warnings as errors (/WX).
// imgui-notify uses vsnprintf() internally, so disable C4996.
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif

#pragma once
#include "dependencies/imgui/fa_solid_900.h"
#include "dependencies/imgui/font_awesome_5.h"
#include <string>
#include <vector>

#define NOTIFY_MAX_MSG_LENGTH 4096    // Max message content length
#define NOTIFY_PADDING_X 20.f         // Bottom-left X padding
#define NOTIFY_PADDING_Y 20.f         // Bottom-left Y padding
#define NOTIFY_PADDING_MESSAGE_Y 10.f // Padding Y between each message
#define NOTIFY_FADE_IN_OUT_TIME 150   // Fade in and out duration
#define NOTIFY_DEFAULT_DISMISS 3000   // Auto dismiss after X ms (default, applied only of no data provided in constructors)
#define NOTIFY_OPACITY 1.0f           // 0-1 Toast opacity
#define NOTIFY_TOAST_FLAGS ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoBackground
// Comment out if you don't want any separator between title and content
#define NOTIFY_USE_SEPARATOR
#define NOTIFY_INLINE inline
#define NOTIFY_NULL_OR_EMPTY(str) (!str || !strlen(str))
#define NOTIFY_FORMAT(fn, format, ...) \
    if (format) {                      \
        va_list args;                  \
        va_start(args, format);        \
        fn(format, args, __VA_ARGS__); \
        va_end(args);                  \
    }
typedef int ImGuiToastType;
typedef int ImGuiToastPhase;
typedef int ImGuiToastPos;
enum ImGuiToastType_ {
    ImGuiToastType_None,
    ImGuiToastType_Success,
    ImGuiToastType_Warning,
    ImGuiToastType_Error,
    ImGuiToastType_Info,
    ImGuiToastType_COUNT
};
enum ImGuiToastPhase_ {
    ImGuiToastPhase_FadeIn,
    ImGuiToastPhase_Wait,
    ImGuiToastPhase_FadeOut,
    ImGuiToastPhase_Expired,
    ImGuiToastPhase_COUNT
};
enum ImGuiToastPos_ {
    ImGuiToastPos_TopLeft,
    ImGuiToastPos_TopCenter,
    ImGuiToastPos_TopRight,
    ImGuiToastPos_BottomLeft,
    ImGuiToastPos_BottomCenter,
    ImGuiToastPos_BottomRight,
    ImGuiToastPos_Center,
    ImGuiToastPos_COUNT
};
class ImGuiToast {
private:
    ImGuiToastType type = ImGuiToastType_None;
    char title[NOTIFY_MAX_MSG_LENGTH];
    char content[NOTIFY_MAX_MSG_LENGTH];
    int dismiss_time = NOTIFY_DEFAULT_DISMISS;
    uint64_t creation_time = 0;
private:
    NOTIFY_INLINE auto set_title(const char* format, va_list args) { vsnprintf(this->title, sizeof(this->title), format, args); }
    NOTIFY_INLINE auto set_content(const char* format, va_list args) { vsnprintf(this->content, sizeof(this->content), format, args); }
public:
    NOTIFY_INLINE auto set_title(const char* format, ...) -> void {
        if (format) {
            va_list args;
            va_start(args, format);
            this->set_title(format, args);
            va_end(args);
        }
    }
    NOTIFY_INLINE auto set_content(const char* format, ...) -> void {
        if (format) {
            va_list args;
            va_start(args, format);
            this->set_content(format, args);
            va_end(args);
        }
    }
    NOTIFY_INLINE auto set_type(const ImGuiToastType& type) -> void {
        IM_ASSERT(type < ImGuiToastType_COUNT);
        this->type = type;
    };
public:
    NOTIFY_INLINE auto get_title( ) -> char* { return this->title; };
    NOTIFY_INLINE auto get_dismiss( ) -> float { return this->dismiss_time; };
    NOTIFY_INLINE auto get_default_title( ) -> string 
    {
        if (!strlen(this->title)) 
        {
            switch (this->type) 
            {
                case ImGuiToastType_None:
                    return "";
                case ImGuiToastType_Success:
                    return "Success";
                case ImGuiToastType_Warning:
                    return "Warning";
                case ImGuiToastType_Error:
                    return "Error";
                case ImGuiToastType_Info:
                    return "Info";
            }
        }
        return this->title;
    };
    NOTIFY_INLINE auto get_type( ) -> const ImGuiToastType& { return this->type; };
    NOTIFY_INLINE auto get_color( ) -> const ImVec4& 
    {
        switch (this->type)
        {
            case ImGuiToastType_None:
                return ImVec4(255, 255, 255, 255); // White
            case ImGuiToastType_Success:
                return ImVec4(0, 255, 0, 255); // Green
            case ImGuiToastType_Warning:
                return ImVec4(255, 255, 0, 255); // Yellow
            case ImGuiToastType_Error:
                return ImVec4(255, 0, 0, 255); // Error
            case ImGuiToastType_Info:
                return ImVec4(0, 157, 255, 255); // Blue
        }
    }
    NOTIFY_INLINE auto get_icon( ) -> const char* {
        switch (this->type) {
            case ImGuiToastType_None:
                return NULL;
            case ImGuiToastType_Success:
                return ICON_FA_CHECK_CIRCLE;
            case ImGuiToastType_Warning:
                return ICON_FA_EXCLAMATION_TRIANGLE;
            case ImGuiToastType_Error:
                return ICON_FA_TIMES_CIRCLE;
            case ImGuiToastType_Info:
                return ICON_FA_INFO_CIRCLE;
        }
    }
    NOTIFY_INLINE auto get_content( ) -> char* { return this->content; };
    NOTIFY_INLINE auto get_elapsed_time( ) { return GetTickCount64( ) - this->creation_time; }
    NOTIFY_INLINE auto get_phase( ) -> const ImGuiToastPhase& {
        const auto elapsed = get_elapsed_time( );

        if (elapsed > NOTIFY_FADE_IN_OUT_TIME + this->dismiss_time + NOTIFY_FADE_IN_OUT_TIME) {
            return ImGuiToastPhase_Expired;
        } else if (elapsed > NOTIFY_FADE_IN_OUT_TIME + this->dismiss_time) {
            return ImGuiToastPhase_FadeOut;
        } else if (elapsed > NOTIFY_FADE_IN_OUT_TIME) {
            return ImGuiToastPhase_Wait;
        } else {
            return ImGuiToastPhase_FadeIn;
        }
    }
    NOTIFY_INLINE auto get_fade_percent( ) -> const float {
        const auto phase = get_phase( );
        const auto elapsed = get_elapsed_time( );
        if (phase == ImGuiToastPhase_FadeIn) {
            return ((float)elapsed / (float)NOTIFY_FADE_IN_OUT_TIME) * NOTIFY_OPACITY;
        } else if (phase == ImGuiToastPhase_FadeOut) {
            return (1.f - (((float)elapsed - (float)NOTIFY_FADE_IN_OUT_TIME - (float)this->dismiss_time) / (float)NOTIFY_FADE_IN_OUT_TIME)) * NOTIFY_OPACITY;
        }
        return 1.f * NOTIFY_OPACITY;
    }
public:
    ImGuiToast(ImGuiToastType type, int dismiss_time = NOTIFY_DEFAULT_DISMISS) {
        IM_ASSERT(type < ImGuiToastType_COUNT);
        this->type = type;
        this->dismiss_time = dismiss_time;
        this->creation_time = GetTickCount64( );
        memset(this->title, 0, sizeof(this->title));
        memset(this->content, 0, sizeof(this->content));
    }
    ImGuiToast(ImGuiToastType type, const char* format, ...) : ImGuiToast(type) {
        if (format) {
            va_list args;
            va_start(args, format);
            this->set_content(format, args);
            va_end(args);
        }
    }
    ImGuiToast(ImGuiToastType type, int dismiss_time, const char* format, ...) : ImGuiToast(type, dismiss_time) {
        if (format) {
            va_list args;
            va_start(args, format);
            this->set_content(format, args);
            va_end(args);
        }
    }
};
namespace ImGui {
    static std::vector<ImGuiToast> notifications;
    NOTIFY_INLINE BOOL BufferingBar(const char* label, float value, const ImVec2& size_arg, const ImU32& bg_col, const ImU32& fg_col) {
        ImGuiWindow* window = GetCurrentWindow( );
        if (window->SkipItems)
            return false;
        ImGuiContext& g = *GImGui;
        const ImGuiStyle& style = g.Style;
        const ImGuiID id = window->GetID(label);
        ImVec2 pos = window->DC.CursorPos;
        ImVec2 size = size_arg;
        const ImRect bb(ImVec2(pos.x, pos.y), ImVec2(pos.x + (size.x * 0.75), pos.y + size.y));
        ItemSize(bb, style.FramePadding.y);
        if (!ItemAdd(bb, id))
            return false;
        const float circleStart = size.x * 0.75;
        window->DrawList->AddRectFilled(bb.Min, ImVec2(pos.x + circleStart, bb.Max.y), bg_col);
        if (value >= 1)
            value = 1;
        window->DrawList->AddRectFilled(bb.Min, ImVec2(pos.x + circleStart * value, bb.Max.y), fg_col);
    }
    NOTIFY_INLINE VOID InsertNotification(const ImGuiToast& toast) {
        notifications.push_back(toast);
    }
    NOTIFY_INLINE VOID RemoveNotification(int index) {
        notifications.erase(notifications.begin( ) + index);
    }
    NOTIFY_INLINE VOID RenderNotifications( ) {
        const auto vp_size = ImVec2(ScreenInfo::Width, ScreenInfo::Height);
        float height = 0.f;
        for (auto i = 0; i < notifications.size( ); i++) 
        {
            auto* current_toast = &notifications[i];
            if (current_toast->get_phase( ) == ImGuiToastPhase_Expired) {
                RemoveNotification(i);
                continue;
            }
            const auto title = current_toast->get_title( );
            string content = current_toast->get_content( );
            content.append(".");
            const auto default_title = current_toast->get_default_title( );
            const auto opacity = current_toast->get_fade_percent( );
            float elapsed_time = current_toast->get_elapsed_time( );
            float dissmis_time = current_toast->get_dismiss( );
            ImVec4 text_color;
            if (current_toast->get_type() == ImGuiToastType_None){
                text_color = ImVec4(255, 255, 255, 255); // White
            }
            if (current_toast->get_type( ) == ImGuiToastType_Success) {
                text_color = ImVec4(0, 255, 0, 255);
            }
            if (current_toast->get_type( ) == ImGuiToastType_Warning) {
                text_color = ImVec4(255, 255, 0, 255); // Yellow
            }
            if (current_toast->get_type( ) == ImGuiToastType_Error) {
                text_color = ImVec4(255, 0, 0, 255); // Error
            }
            if (current_toast->get_type( ) == ImGuiToastType_Info) {
                text_color = ImVec4(0, 157, 255, 255); // Blue
            }
            char window_name[50];
            sprintf_s(window_name, "##TOAST%d", i);
            PushStyleVar(ImGuiStyleVar_WindowBorderSize, 1.3f);
            PushStyleVar(ImGuiStyleVar_WindowRounding, 0);
            PushStyleVar(ImGuiStyleVar_FrameRounding, 0);
            PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(12, 8));
            PushStyleColor(ImGuiCol_Border, text_color);
            PushStyleColor(ImGuiCol_WindowBg, ImVec4(43 / 255.f, 43 / 255.f, 43 / 255.f, 255 / 255.f));
            SetNextWindowPos(ImVec2(vp_size.x - NOTIFY_PADDING_X, 100 + height), ImGuiCond_Always, ImVec2(1.0f, 1.0f));
            Begin(window_name, NULL, NOTIFY_TOAST_FLAGS);
            {
                ImDrawList* drawList = GetWindowDrawList();
                ImVec2 pos = GetWindowPos();
                ImVec2 size = GetWindowSize();
                float slant = 12.0f;

                // Hexagon Points (clipped corners style for better text fit)
                // Inset by 1.0f to prevent border clipping
                ImVec2 pts[6] = {
                    ImVec2(pos.x + slant + 1.0f, pos.y + 1.0f),
                    ImVec2(pos.x + size.x - slant - 1.0f, pos.y + 1.0f),
                    ImVec2(pos.x + size.x - 1.0f, pos.y + size.y * 0.5f),
                    ImVec2(pos.x + size.x - slant - 1.0f, pos.y + size.y - 1.0f),
                    ImVec2(pos.x + slant + 1.0f, pos.y + size.y - 1.0f),
                    ImVec2(pos.x + 1.0f, pos.y + size.y * 0.5f)
                };

                // Draw Background
                drawList->AddConvexPolyFilled(pts, 6, ImColor(25, 25, 25, (int)(230 * opacity)));
                // Draw Border
                drawList->AddPolyline(pts, 6, ColorConvertFloat4ToU32(ImVec4(text_color.x, text_color.y, text_color.z, opacity)), ImDrawFlags_Closed, 1.5f);

                PushTextWrapPos(vp_size.x / 3.f);
                SetCursorPosX(GetCursorPosX() + slant * 0.5f); // Adjust for slant
                bool was_title_rendered = false;
                if (!NOTIFY_NULL_OR_EMPTY(current_toast->get_icon( ))) 
                {
                    TextColored(text_color, current_toast->get_icon( ));
                    was_title_rendered = true;
                }
                if (!NOTIFY_NULL_OR_EMPTY(title)) {
                    if (!NOTIFY_NULL_OR_EMPTY(current_toast->get_icon( )))
                        SameLine( );
                    Text(title);
                    was_title_rendered = true;
                } 
                else if (!NOTIFY_NULL_OR_EMPTY(default_title.c_str( ))) 
                {
                    if (!NOTIFY_NULL_OR_EMPTY(current_toast->get_icon( )))
                        SameLine( );
                    Text(default_title.c_str( ));
                    was_title_rendered = true;
                }
                if (was_title_rendered && !NOTIFY_NULL_OR_EMPTY(content.c_str( ))) 
                {
                    SetCursorPosY(GetCursorPosY( ));
                }
                if (!NOTIFY_NULL_OR_EMPTY(content.c_str( ))) 
                {
                    if (was_title_rendered) 
                    {
                        ImVec2 TextSize2 = Fonts::MainFont->CalcTextSizeA(20, FLT_MAX, 0.0f, content.c_str( ));
                        const ImU32 col = ImColor(230, 179, 0, 255);  // đường ở trong
                        const ImU32 bg = ImColor(66, 66, 66, 255); // phần ko chạy
                        float Opacaityy = (float)((elapsed_time) / dissmis_time);
                        ImGui::BufferingBar("##buffer_bar", Opacaityy, ImVec2(TextSize2.x, 2), bg, col);
                    }
                    Text(content.c_str( ));
                }
                PopTextWrapPos( );
            }
            height += GetWindowHeight( ) + NOTIFY_PADDING_MESSAGE_Y;
            PopStyleVar(5);
            PopStyleColor(2);
            End( );
        }
    }
    NOTIFY_INLINE VOID MergeIconsWithLatestFont(float font_size, bool FontDataOwnedByAtlas = false) 
    {
        static const ImWchar icons_ranges[] = {ICON_MIN_FA, ICON_MAX_FA, 0};
        ImFontConfig icons_config;
        icons_config.MergeMode = true;
        icons_config.PixelSnapH = true;
        icons_config.FontDataOwnedByAtlas = FontDataOwnedByAtlas;
        GetIO( ).Fonts->AddFontFromMemoryTTF((void*)fa_solid_900, sizeof(fa_solid_900), font_size, &icons_config, icons_ranges);
    }
}
#endif
