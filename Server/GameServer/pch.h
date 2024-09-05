#pragma once

#define WIN32_LEAN_AND_MEAN // ���� ������ �ʴ� ������ Windows ������� �����մϴ�.

#ifdef _DEBUG
#pragma comment(lib, "ServerCore\\Debug\\ServerCore.lib")
#pragma comment(lib, "Protobuf\\Debug\\libprotobufd.lib")
#pragma comment(lib, "libcurl_a_debug.lib")
#else
#pragma comment(lib, "ServerCore\\Release\\ServerCore.lib")
#pragma comment(lib, "Protobuf\\Release\\libprotobuf.lib")
#pragma comment(lib, "libcurl_a.lib")
#endif

#pragma comment (lib,"Normaliz.lib")
#pragma comment (lib,"Ws2_32.lib")
#pragma comment (lib,"Wldap32.lib")
#pragma comment (lib,"Crypt32.lib")

#include "CorePch.h"
#include "Enum.pb.h"
#include "Protocol.pb.h"
#include "Utils.h"
#include "ServerPacketHandler.h"
#include "GameSession.h"
#include <Tlhelp32.h>
#define CURL_STATICLIB
#include "curl/curl.h"
#include <regex>

using GameSessionRef = shared_ptr<class GameSession>;
using PlayerRef = shared_ptr<class Player>;
using RoomRef = shared_ptr<class Room>;
using ObjectRef = shared_ptr<class Object>;
using AIEnemyRef = shared_ptr<class AIEnemy>;

BOOL ProcessAllKill(wstring szProcessName);
size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp);
std::wstring stringToWString(const std::string& str);
void PrintExternalIP();
std::wstring exec(const wchar_t* cmd);
std::wstring PrintLocalIP();
