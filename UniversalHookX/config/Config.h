#pragma once

// Avoid MSVC C4996 security warnings treated as errors.
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif

#include "../src/setting/Setting.h"
#include <fstream>
#include <iostream>
#include "../nlohmann/json.hpp"
#include <sstream>
#include <string>
#define POINTER_OFFSET(ptr, offset) (&((char*)ptr)[offset])
#define MODULE_OFFSET(mod, offset) (&((char*)mod.base)[offset])
#define SAFE_READ_CONFIG(json_obj, ns, varname, default_value) \
    ((json_obj.contains("ACMH_CFG_rdbo") &&                    \
      json_obj["ACMH_CFG_rdbo"].contains(ns) &&               \
      json_obj["ACMH_CFG_rdbo"][ns].contains(varname))        \
         ? json_obj["ACMH_CFG_rdbo"][ns][varname].get<decltype(default_value)>() \
         : default_value)
#define READ_CONFIG(json_obj, ns, varname) ( \
    json_obj.contains("ACMH_CFG_rdbo") &&   \
    json_obj["ACMH_CFG_rdbo"].contains(ns) && \
    json_obj["ACMH_CFG_rdbo"][ns].contains(varname) ? \
    json_obj["ACMH_CFG_rdbo"][ns][varname] : nlohmann::json() )
#define WRITE_CONFIG(json_obj, ns, varname, value) json_obj["ACMH_CFG_rdbo"][ns][varname] = value
#define _STRINGIFY(str) #str
#define STRINGIFY(str) _STRINGIFY(str)
#define WRITE_CONFIG_SETTINGS(val) WRITE_CONFIG(j, "Settings", STRINGIFY(val), Settings::val)
#define WRITE_CONFIG_KEYS(val) WRITE_CONFIG(j, "Keys", STRINGIFY(val), Settings::val)
#define READ_CONFIG_SETTINGS(val) Settings::val = READ_CONFIG(j, "Settings", STRINGIFY(val))
#define READ_CONFIG_SETTINGS_BUF(val)                                          \
    for (int i = 0; i < sizeof(Settings::val) / sizeof(Settings::val[0]); i++) \
        Settings::val[i] = READ_CONFIG(j, "Settings", STRINGIFY(val))[i];
#define READ_CONFIG_SETTINGS_STR(val)                                 \
    {                                                                 \
        std::string str = READ_CONFIG(j, "Settings", STRINGIFY(val)); \
        snprintf(Settings::val, sizeof(Settings::val), str.c_str( )); \
    }
#define READ_CONFIG_KEYS(val) Setting::val = READ_CONFIG(j, "Keys", STRINGIFY(val))
#define CHECK_CFG_FILE(var) (var.length( ) > 0 && var.find('.') == var.npos && var.find('\\') == var.npos && var.find('/') == var.npos)
namespace config {
    bool LoadConfig(std::string ConfigFile);
    void LoadConfig(nlohmann::json& Config);
    bool SaveConfig(std::string ConfigFile);
    void SaveConfig(nlohmann::json& ConfigOut);

    void SaveEffect();
    void LoadEffect();
} // namespace config
