#pragma once
enum ImGuiNotiType {
    ImGuiNotiType_None,
    ImGuiNotiType_Success,
    ImGuiNotiType_Warning,
    ImGuiNotiType_Error,
    ImGuiNotiType_Info,
    ImGuiNotiType_COUNT
};
namespace DX11 {
    void Hook(HWND hwnd);
    void Hook2(HWND hwnd);
	void Unhook( );
    void ModSkinThread( );
    void InsertNoti(ImGuiNotiType Type, int TimeShow, std::string Text = "", std::string Title = "");
}
