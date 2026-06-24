// KeyAuth api implementation - init() and license() for KeyAuth 1.3 API.
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <Windows.h>
#include <Wininet.h>
#include "KeyAuthConfig.h"
#include <string>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <cctype>
#include <cstring>
#include <fstream>

#pragma comment(lib, "wininet.lib")

namespace KeyAuth {

bool api::debug = false;

static std::string UrlEncode(const std::string& value) {
    std::ostringstream escaped;
    for (char c : value) {
        if (std::isalnum((unsigned char)c) || c == '-' || c == '_' || c == '.' || c == '~')
            escaped << c;
        else
            escaped << '%' << std::hex << std::uppercase << std::setw(2) << std::setfill('0') << int((unsigned char)c) << std::dec;
    }
    return escaped.str();
}

static std::string TrimKey(const std::string& s) {
    size_t start = s.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = s.find_last_not_of(" \t\r\n");
    return s.substr(start, end - start + 1);
}

static std::string GetMachineHWID() {
    std::string hwid;
    char volName[MAX_PATH] = {}, fsName[MAX_PATH] = {};
    DWORD serial = 0, maxLen = 0, flags = 0;
    if (GetVolumeInformationA("C:\\", volName, sizeof(volName), &serial, &maxLen, &flags, fsName, sizeof(fsName))) {
        char buf[64] = {};
        snprintf(buf, sizeof(buf), "%08X", serial);
        hwid += buf;
    }
    char compName[MAX_COMPUTERNAME_LENGTH + 1] = {};
    DWORD compLen = sizeof(compName);
    if (GetComputerNameA(compName, &compLen))
        hwid += std::string(compName);
    if (hwid.empty())
        hwid = "unknown";
    return hwid;
}

static const char* USER_AGENT = "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36";

static std::string HttpPost(const std::string& url, const std::string& postData) {
    std::string result;
    HINTERNET hInternet = InternetOpenA(USER_AGENT, INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
    if (!hInternet) return result;

    DWORD timeoutMs = 20000;
    InternetSetOptionA(hInternet, INTERNET_OPTION_CONNECT_TIMEOUT, &timeoutMs, sizeof(timeoutMs));
    InternetSetOptionA(hInternet, INTERNET_OPTION_RECEIVE_TIMEOUT, &timeoutMs, sizeof(timeoutMs));
    InternetSetOptionA(hInternet, INTERNET_OPTION_SEND_TIMEOUT, &timeoutMs, sizeof(timeoutMs));

    char host[256] = {}, pathBuf[1024] = {};
    URL_COMPONENTSA uc = { sizeof(uc) };
    uc.dwStructSize = sizeof(uc);
    uc.lpszHostName = host;
    uc.dwHostNameLength = sizeof(host);
    uc.lpszUrlPath = pathBuf;
    uc.dwUrlPathLength = sizeof(pathBuf);
    if (!InternetCrackUrlA(url.c_str(), (DWORD)url.size(), 0, &uc)) {
        InternetCloseHandle(hInternet);
        return result;
    }

    bool useHttps = (uc.nScheme == INTERNET_SCHEME_HTTPS) || (url.size() >= 8 && url.compare(0, 8, "https://") == 0);
    DWORD port = (uc.nPort != 0) ? uc.nPort : (useHttps ? 443u : 80u);
    HINTERNET hConnect = InternetConnectA(hInternet, host, (INTERNET_PORT)port, NULL, NULL, INTERNET_SERVICE_HTTP, 0, 0);
    if (!hConnect) {
        InternetCloseHandle(hInternet);
        return result;
    }

    DWORD flags = useHttps ? INTERNET_FLAG_SECURE : 0;
    HINTERNET hRequest = HttpOpenRequestA(hConnect, "POST", pathBuf, NULL, NULL, NULL, flags, 0);
    if (!hRequest) {
        InternetCloseHandle(hConnect);
        InternetCloseHandle(hInternet);
        return result;
    }

    if (useHttps) {
        DWORD secureFlags = SECURITY_FLAG_IGNORE_UNKNOWN_CA | SECURITY_FLAG_IGNORE_CERT_CN_INVALID | SECURITY_FLAG_IGNORE_CERT_DATE_INVALID;
        InternetSetOptionA(hRequest, INTERNET_OPTION_SECURITY_FLAGS, &secureFlags, sizeof(secureFlags));
    }

    const char* headers = "Content-Type: application/x-www-form-urlencoded\r\n";
    if (!HttpSendRequestA(hRequest, headers, (DWORD)strlen(headers), (LPVOID)postData.c_str(), (DWORD)postData.size())) {
        InternetCloseHandle(hRequest);
        InternetCloseHandle(hConnect);
        InternetCloseHandle(hInternet);
        return result;
    }

    char buffer[4096];
    DWORD bytesRead;
    while (InternetReadFile(hRequest, buffer, sizeof(buffer) - 1, &bytesRead) && bytesRead > 0) {
        buffer[bytesRead] = '\0';
        result += buffer;
    }
    InternetCloseHandle(hRequest);
    InternetCloseHandle(hConnect);
    InternetCloseHandle(hInternet);
    return result;
}

static std::string HttpGet(const std::string& fullUrl) {
    std::string result;
    HINTERNET hInternet = InternetOpenA(USER_AGENT, INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
    if (!hInternet) return result;

    DWORD timeoutMs = 20000;
    InternetSetOptionA(hInternet, INTERNET_OPTION_CONNECT_TIMEOUT, &timeoutMs, sizeof(timeoutMs));
    InternetSetOptionA(hInternet, INTERNET_OPTION_RECEIVE_TIMEOUT, &timeoutMs, sizeof(timeoutMs));

    DWORD flags = INTERNET_FLAG_RELOAD | INTERNET_FLAG_NO_CACHE_WRITE | INTERNET_FLAG_SECURE;
    HINTERNET hUrl = InternetOpenUrlA(hInternet, fullUrl.c_str(), NULL, 0, flags, 0);
    if (!hUrl) {
        InternetCloseHandle(hInternet);
        return result;
    }
    DWORD secureFlags = SECURITY_FLAG_IGNORE_UNKNOWN_CA | SECURITY_FLAG_IGNORE_CERT_CN_INVALID | SECURITY_FLAG_IGNORE_CERT_DATE_INVALID;
    InternetSetOptionA(hUrl, INTERNET_OPTION_SECURITY_FLAGS, &secureFlags, sizeof(secureFlags));

    char buffer[4096];
    DWORD bytesRead;
    while (InternetReadFile(hUrl, buffer, sizeof(buffer) - 1, &bytesRead) && bytesRead > 0) {
        buffer[bytesRead] = '\0';
        result += buffer;
    }
    InternetCloseHandle(hUrl);
    InternetCloseHandle(hInternet);
    return result;
}

static void ParseSessionId(const std::string& response, std::string& outSessionId) {
    const char* patterns[] = {
        "\"sessionid\":\"", "\"session_id\":\"", "\"sessionid\": \"", "\"session_id\": \"",
        "\"token\":\"", "\"token\": \"", "\"session\":\"", "\"session\": \"",
        "\"sessionId\":\"", "\"sessionId\": \"", "\"SessionId\":\"", "\"SessionId\": \""
    };
    for (const char* pat : patterns) {
        size_t len = strlen(pat);
        size_t p = response.find(pat);
        if (p != std::string::npos) {
            p += len;
            size_t end = response.find("\"", p);
            if (end != std::string::npos) {
                outSessionId = response.substr(p, end - p);
                return;
            }
        }
    }
}

static void ParseEnckey(const std::string& response, std::string& outEnckey) {
    const char* patterns[] = {
        "\"enckey\":\"", "\"enckey\": \"", "\"enc_key\":\"", "\"enc_key\": \""
    };
    for (const char* pat : patterns) {
        size_t len = strlen(pat);
        size_t p = response.find(pat);
        if (p != std::string::npos) {
            p += len;
            size_t end = response.find("\"", p);
            if (end != std::string::npos) {
                outEnckey = response.substr(p, end - p);
                return;
            }
        }
    }
}

static std::string GetDebugFilePath() {
    char path[MAX_PATH] = {};
    if (GetModuleFileNameA(NULL, path, MAX_PATH) > 0) {
        std::string s(path);
        size_t last = s.find_last_of("\\/");
        if (last != std::string::npos)
            return s.substr(0, last + 1) + "auth_response.txt";
    }
    return "auth_response.txt";
}

static void WriteDebugResponse(const char* label, const std::string& content) {
    std::string filePath = GetDebugFilePath();
    std::ofstream f(filePath.c_str(), std::ios::out);
    if (f) {
        f << label << "\n\n";
        if (!content.empty())
            f << content << "\n";
        else
            f << "(empty)\n";
        f.close();
    }
}

static bool ParseSuccess(const std::string& response) {
    if (response.find("\"success\":true") != std::string::npos) return true;
    if (response.find("\"success\": true") != std::string::npos) return true;
    if (response.find("\"success\":true ") != std::string::npos) return true;
    if (response.find("\"Success\":true") != std::string::npos) return true;
    if (response.find("\"Success\": true") != std::string::npos) return true;
    if (response.find("\"success\":1") != std::string::npos) return true;
    if (response.find("\"success\": 1") != std::string::npos) return true;
    return false;
}

static std::string ParseMessage(const std::string& response) {
    const char* patterns[] = {
        "\"message\":\"", "\"message\": \"", "\"msg\":\"", "\"msg\": \"",
        "\"info\":\"", "\"info\": \"", "\"error\":\"", "\"error\": \"",
        "\"reason\":\"", "\"reason\": \""
    };
    for (const char* pat : patterns) {
        size_t len = strlen(pat);
        size_t msgStart = response.find(pat);
        if (msgStart != std::string::npos) {
            msgStart += len;
            size_t msgEnd = response.find("\"", msgStart);
            if (msgEnd != std::string::npos)
                return response.substr(msgStart, msgEnd - msgStart);
        }
    }
    return "";
}

static std::string ParseExpiry(const std::string& response) {
    const char* patterns[] = {
        "\"expiry\":", "\"expiry\": ", "\"expiry\":\"", "\"expires\":", "\"expires\": ",
        "\"expires\":\"", "\"expiry_date\":", "\"expiry_date\":\"", "\"subscription_expiry\":",
        "\"subscription_expiry\":\"", "\"expiry_time\":", "\"expiry_time\":\""
    };
    for (const char* pat : patterns) {
        size_t pos = response.find(pat);
        if (pos != std::string::npos) {
            pos += strlen(pat);
            while (pos < response.size() && (response[pos] == ' ' || response[pos] == '\t' || response[pos] == '"'))
                ++pos;
            size_t end = pos;
            while (end < response.size() && (std::isdigit((unsigned char)response[end]) || response[end] == '.'))
                ++end;
            if (end > pos) return response.substr(pos, end - pos);
        }
    }
    return "";
}

void api::init() {
    data.success = false;
    data.message = "";
    sessionid.clear();
    enckey.clear();

    std::string query;
    query += "type=init&ver="; query += UrlEncode(version);
    query += "&name="; query += UrlEncode(name);
    query += "&ownerid="; query += UrlEncode(ownerid);
    query += "&secret="; query += UrlEncode(secret);

    std::string getUrl = url;
    if (getUrl.find('?') == std::string::npos) getUrl += "?";
    else if (!getUrl.empty() && getUrl.back() != '&') getUrl += "&";
    getUrl += query;

    std::string response = HttpPost(url, query);
    if (response.empty())
        response = HttpGet(getUrl);

    if (response.empty()) {
        Sleep(800);
        response = HttpPost(url, query);
        if (response.empty())
            response = HttpGet(getUrl);
    }

    if (!response.empty()) {
        ParseSessionId(response, sessionid);
        ParseEnckey(response, enckey);
        data.success = ParseSuccess(response);
        if (!data.success) {
            data.message = ParseMessage(response);
            WriteDebugResponse("Init failed - server response:", response);
        } else if (sessionid.empty()) {
            WriteDebugResponse("Init success but no sessionid in response:", response);
        }
    } else {
        DWORD err = GetLastError();
        std::string msg = "Init request to: ";
        msg += url;
        msg += "\nResponse: (empty)\nWinInet/GetLastError: ";
        msg += std::to_string((unsigned long long)err);
        if (err == 12002) msg += " (ERROR_INTERNET_TIMEOUT)";
        else if (err == 12029) msg += " (ERROR_INTERNET_CONNECTION_ABORTED)";
        else if (err == 12007) msg += " (ERROR_INTERNET_NAME_NOT_RESOLVED)";
        else if (err == 12009) msg += " (ERROR_INTERNET_INCORRECT_HANDLE_STATE)";
        WriteDebugResponse(msg.c_str(), "");
    }
}

void api::license(std::string key, std::string code) {
    data.success = false;
    data.message = "";

    key = TrimKey(key);
    if (key.empty()) {
        data.message = "Key is empty";
        return;
    }

    if (sessionid.empty())
        init();

    if (sessionid.empty()) {
        data.message = "Cannot reach license server. Check internet, firewall, or try again later.";
        WriteDebugResponse("Init failed - no sessionid. Check URL and app credentials (name, ownerid, secret).", "");
        return;
    }

    std::string hwidStr = GetMachineHWID();
    std::string query;
    query += "type=license&key="; query += UrlEncode(key);
    query += "&ownerid="; query += UrlEncode(ownerid);
    query += "&name="; query += UrlEncode(name);
    query += "&sessionid="; query += UrlEncode(sessionid);
    query += "&hwid="; query += UrlEncode(hwidStr);
    if (!enckey.empty()) { query += "&enckey="; query += UrlEncode(enckey); }
    if (!code.empty()) { query += "&code="; query += UrlEncode(code); }

    std::string response = HttpPost(url, query);
    if (response.empty()) {
        std::string getUrl = url;
        if (getUrl.find('?') == std::string::npos) getUrl += "?";
        else if (!getUrl.empty() && getUrl.back() != '&') getUrl += "&";
        getUrl += query;
        response = HttpGet(getUrl);
    }

    if (response.empty()) {
        data.message = "No response from server. Check internet connection.";
        WriteDebugResponse("License request returned empty response. URL used: ", url);
        return;
    }

    data.success = ParseSuccess(response);
    if (data.success) {
        std::string expiryStr = ParseExpiry(response);
        user_data.subscriptions.clear();
        if (!expiryStr.empty()) {
            user_data.subscriptions.resize(1);
            user_data.subscriptions[0].name = "default";
            user_data.subscriptions[0].expiry = expiryStr;
        }
    } else {
        user_data.subscriptions.clear();
        data.message = ParseMessage(response);
        if (data.message.empty())
            data.message = "Invalid key";
        WriteDebugResponse("License failed - server response:", response);
    }
}

void api::ban(std::string reason) {}
void api::check(bool check_paid) {}
void api::log(std::string msg) {}
std::string api::var(std::string varid) { return ""; }
std::string api::webhook(std::string id, std::string params, std::string body, std::string contenttype) { return ""; }
void api::setvar(std::string var, std::string vardata) {}
std::string api::getvar(std::string var) { return ""; }
bool api::checkblack() { return false; }
void api::web_login() {}
void api::button(std::string value) {}
void api::upgrade(std::string username, std::string key) {}
void api::login(std::string username, std::string password, std::string code) {}
std::vector<unsigned char> api::download(std::string fileid) { return {}; }
void api::regstr(std::string username, std::string password, std::string key, std::string email) {}
void api::chatget(std::string channel) {}
bool api::chatsend(std::string message, std::string channel) { return false; }
void api::changeUsername(std::string newusername) {}
std::string api::fetchonline() { return ""; }
void api::fetchstats() {}
void api::forgot(std::string username, std::string email) {}
void api::logout() {}

}
