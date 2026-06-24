#include "KeyAuthConfig.h"
#include "xorstr.hpp"

using namespace KeyAuth;

std::string name = XorStr("NAME").str();
std::string ownerid = XorStr("3JTXaaS3Mb").str();
std::string secret = XorStr("e54ab19e7fca26bbd70bc7f866f5594d5f983788652c9c9c1f12c410cee3dd26").str();
std::string version = XorStr("1.0").str();
std::string url = XorStr("https://keyauth.win/api/1.3/").str();
std::string path = XorStr("").str();

KeyAuth::api KeyAuthApp(name, ownerid, version, url, path);
