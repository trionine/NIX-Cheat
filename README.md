# NIX Cheat

Internal cheat DLL for NIX (Unreal Engine 4).

## Build

Open `0x0.sln` in Visual Studio 2022 and build. Available configurations:

| Config | Platform |
|---|---|
| Debug | Win32 |
| Release | Win32 |
| Debug | x64 |
| Release | x64 |

Output: `UniversalHookX/bin/module.dll`

## Features

- Aimbot
- ESP
- ImGui overlay (DX9/10/11/12/OpenGL)
- KeyAuth license system
- Config save/load
- Discord webhooks

## Dependencies

All vendored in-repo:
- [Dear ImGui](https://github.com/ocornut/imgui)
- [MinHook](https://github.com/TsudaKageyu/minhook)
- [Detours](https://github.com/microsoft/Detours)
- [nlohmann/json](https://github.com/nlohmann/json)
- [stb_image](https://github.com/nothings/stb)
