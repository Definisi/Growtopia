#pragma once
#include <iostream>
#include <string>
#include <curl/curl.h>
#include <sstream>
#include <vector>
#include <algorithm>
#include <nlohmann/json.hpp>
#include <client/structs/login_info.hpp>

// Compile with: -lcurl

enum class LoginStatus {
    Success,
    Failed,
    InvalidResponse,
    WrongCreds,
    Timeout,
    GenericHttpError,
    CaptchaRequested,
    UnsafeBrowser,
    CreateAccountRequested
};

inline size_t writeCallback(void* contents, size_t size, size_t nmemb, std::string* output) {
    size_t totalSize = size * nmemb;
    output->append((char*)contents, totalSize);
    return totalSize;
}

// Remove getServerMeta since we'll use LoginInfo's meta

inline std::string getLoginFormUrl(LoginInfo& loginInfo) {
    if (loginInfo.m_meta.empty()) {
        std::cout << "Meta is empty, requesting server data..." << std::endl;
        if (!loginInfo.request_server_data()) {
            std::cout << "Failed to get server data" << std::endl;
            return "";
        }
    }

    // URL encode the mac address (replace : with %3A)
    std::string encodedMac = loginInfo.m_mac;
    size_t pos = 0;
    while ((pos = encodedMac.find(':', pos)) != std::string::npos) {
        encodedMac.replace(pos, 1, "%3A");
        pos += 3;
    }

    // Create proper login data using dynamic variables from LoginInfo
    std::string loginData = "tankIDName=%7C%0AtankIDPass=%7C%0ArequestedName=%7C%0Af%7C1%0Aprotocol%7C" + std::to_string(loginInfo.m_protocol) + "%0Agame_version%7C" + loginInfo.m_game_version + "%0Afz%7C21905432%0Acbits%7C0%0Aplayer_age%7C25%0AGDPR%7C2%0Acategory%7C_-5100%0AtotalPlaytime%7C0%0Aklv%7C" + loginInfo.m_klv + "%0Ahash2%7C" + std::to_string(loginInfo.m_hash2) + "%0Ameta%7C" + loginInfo.m_meta + "%0Afhash%7C-716928004%0Arid%7C" + loginInfo.m_rid + "%0AplatformID%7C" + loginInfo.m_platform_id + "%0AdeviceVersion%7C0%0Acountry%7Cus%0Ahash%7C" + std::to_string(loginInfo.m_hash) + "%0Amac%7C" + encodedMac + "%0Awk%7C" + loginInfo.m_wk + "%0Azf%7C-1623530258";

    CURL* curl = curl_easy_init();
    if (!curl) return "";

    std::string response;
    curl_easy_setopt(curl, CURLOPT_URL, "https://login.growtopiagame.com/player/login/dashboard");
    curl_easy_setopt(curl, CURLOPT_POST, 1L);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, loginData.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 25L);
    struct curl_slist* headers = nullptr;
    headers = curl_slist_append(headers, "Content-Type: application/x-www-form-urlencoded");
    headers = curl_slist_append(headers, "User-Agent: UbiServices_SDK_2022.Release.9_PC64_ansi_static");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    CURLcode res = curl_easy_perform(curl);
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);

    if (res != CURLE_OK) return "";
    std::cout << "Dashboard response: " << response << std::endl;
    
    std::istringstream stream(response);
    std::string line;
    while (std::getline(stream, line)) {
        size_t pos = line.find("href=\"");
        if (pos != std::string::npos && line.find("/growid/", pos) != std::string::npos) {
            size_t start = line.find("\"", pos) + 1;
            size_t end = line.find("\"", start);
            std::cout << "Login URL: " << line.substr(start, end - start) << std::endl;
            return line.substr(start, end - start);
        }
    }

    return "";
}

inline LoginStatus growidLogin(const std::string& loginUrl, const std::string& growid, const std::string& password, std::string& outToken) {
    CURL* curl = curl_easy_init();
    if (!curl) return LoginStatus::Failed;

    // Enable cookie handling
    curl_easy_setopt(curl, CURLOPT_COOKIEFILE, "");
    curl_easy_setopt(curl, CURLOPT_COOKIEJAR, "");

    std::string tokenPage;
    curl_easy_setopt(curl, CURLOPT_URL, loginUrl.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &tokenPage);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1L);
    struct curl_slist* headers = nullptr;
    headers = curl_slist_append(headers, "Content-Type: application/x-www-form-urlencoded");
    headers = curl_slist_append(headers, "User-Agent: UbiServices_SDK_2022.Release.9_PC64_ansi_static");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    CURLcode res = curl_easy_perform(curl);
    
    if (res != CURLE_OK) {
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
        return LoginStatus::Failed;
    }

    std::cout << "Token page content (first 500 chars): " << tokenPage.substr(0, 500) << std::endl;
    
    // Try multiple token patterns like Python version
    size_t tokenPos = std::string::npos;
    std::string token;
    
    // Pattern 1: name="_token" value="..."
    tokenPos = tokenPage.find("name=\"_token\" value=\"");
    if (tokenPos != std::string::npos) {
        size_t valueStart = tokenPos + 20; // length of "name=\"_token\" value=\""
        size_t valueEnd = tokenPage.find("\"", valueStart);
        if (valueEnd != std::string::npos) {
            token = tokenPage.substr(valueStart, valueEnd - valueStart);
        }
    }
    
    // Pattern 2: <input name="_token" value="..." (more flexible)
    if (token.empty()) {
        tokenPos = tokenPage.find("name=\"_token\"");
        if (tokenPos != std::string::npos) {
            size_t valuePos = tokenPage.find("value=\"", tokenPos);
            if (valuePos != std::string::npos) {
                size_t valueStart = valuePos + 7; // length of "value=\""
                size_t valueEnd = tokenPage.find("\"", valueStart);
                if (valueEnd != std::string::npos) {
                    token = tokenPage.substr(valueStart, valueEnd - valueStart);
                }
            }
        }
    }
    
    if (token.empty()) {
        std::cout << "Could not find _token in page. Full page content:" << std::endl;
        std::cout << tokenPage << std::endl;
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
        return LoginStatus::InvalidResponse;
    }
    
    std::cout << "_token found: " << token << std::endl;

    // Prepare validation POST with same curl handle to maintain cookies
    std::string validationResponse;
    char* escapedGrowid = curl_easy_escape(curl, growid.c_str(), 0);
    char* escapedPassword = curl_easy_escape(curl, password.c_str(), 0);
    std::string postFields = "_token=" + token + "&growId=" + std::string(escapedGrowid) + "&password=" + std::string(escapedPassword);
    curl_free(escapedGrowid);
    curl_free(escapedPassword);
    
    std::cout << "POST data being sent: " << postFields << std::endl;
    
    curl_easy_setopt(curl, CURLOPT_URL, "https://login.growtopiagame.com/player/growid/login/validate");
    curl_easy_setopt(curl, CURLOPT_POST, 1L);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postFields.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &validationResponse);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 0L);
    
    // Update headers for validation request
    curl_slist_free_all(headers);
    headers = nullptr;
    headers = curl_slist_append(headers, "Content-Type: application/x-www-form-urlencoded");
    headers = curl_slist_append(headers, "User-Agent: UbiServices_SDK_2022.Release.9_PC64_ansi_static");
    headers = curl_slist_append(headers, "Accept: application/json, text/plain, */*");
    headers = curl_slist_append(headers, "X-Requested-With: XMLHttpRequest");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    
    std::cout << "Sending validation request to: https://login.growtopiagame.com/player/growid/login/validate" << std::endl;
    
    res = curl_easy_perform(curl);
    long httpCode = 0;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &httpCode);
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);

    if (res != CURLE_OK) {
        std::cout << "CURL Error: " << curl_easy_strerror(res) << std::endl;
        return LoginStatus::Failed;
    }
    
    std::cout << "Validation Response (HTTP " << httpCode << "): " << validationResponse << std::endl;
    
    if (httpCode == 302) {
        std::cout << "Wrong credentials detected (HTTP 302 redirect)" << std::endl;
        return LoginStatus::WrongCreds;
    } else if (httpCode == 200) {
        // Check if response is empty
        if (validationResponse.empty()) {
            std::cout << "Empty response from server" << std::endl;
            return LoginStatus::InvalidResponse;
        }
        
        // Check if response looks like JSON
        if (validationResponse[0] != '{' && validationResponse.find('{') == std::string::npos) {
            std::cout << "Response is not JSON format. Response: " << validationResponse << std::endl;
            return LoginStatus::InvalidResponse;
        }
        
        try {
            auto json = nlohmann::json::parse(validationResponse);
            std::cout << "Parsed JSON successfully" << std::endl;
            
            if (json.contains("status")) {
                std::cout << "Status field found: " << json["status"].get<std::string>() << std::endl;
                
                if (json["status"] == "success" && json.contains("token")) {
                    outToken = json["token"].get<std::string>();
                    std::cout << "Login successful! Token: " << outToken << std::endl;
                    return LoginStatus::Success;
                } else {
                    std::cout << "Login failed - status: " << json["status"].get<std::string>() << std::endl;
                    if (json.contains("message")) {
                        std::cout << "Error message: " << json["message"].get<std::string>() << std::endl;
                    }
                    return LoginStatus::Failed;
                }
            } else {
                std::cout << "No 'status' field in JSON response" << std::endl;
                return LoginStatus::InvalidResponse;
            }
        } catch (const std::exception& e) {
            std::cout << "JSON Parse Error: " << e.what() << std::endl;
            std::cout << "Raw response that failed to parse: " << validationResponse << std::endl;
            return LoginStatus::InvalidResponse;
        }
    } else {
        std::cout << "Unexpected HTTP code: " << httpCode << std::endl;
        return LoginStatus::GenericHttpError;
    }
}

inline LoginStatus performGrowidLogin(LoginInfo& loginInfo, const std::string& growid, const std::string& password, std::string& outToken) {
    std::cout << "[GROWID/LEGACY LOGIN MODE]" << std::endl;
    std::cout << "Getting login form URL..." << std::endl;
    
    std::string loginUrl = getLoginFormUrl(loginInfo);
    if (loginUrl.empty()) {
        std::cout << "LOGIN DASHBOARD TIMEOUT or FAILED" << std::endl;
        return LoginStatus::Timeout;
    }
    
    std::cout << "Attempting login with GrowID: " << growid << std::endl;
    LoginStatus status = growidLogin(loginUrl, growid, password, outToken);
    
    std::cout << "Login result: ";
    switch (status) {
        case LoginStatus::Success:
            std::cout << "SUCCESS! Token obtained." << std::endl;
            break;
        case LoginStatus::WrongCreds:
            std::cout << "WRONG CREDENTIALS" << std::endl;
            break;
        case LoginStatus::InvalidResponse:
            std::cout << "INVALID RESPONSE from server" << std::endl;
            break;
        case LoginStatus::Timeout:
            std::cout << "TIMEOUT" << std::endl;
            break;
        case LoginStatus::GenericHttpError:
            std::cout << "HTTP ERROR" << std::endl;
            break;
        default:
            std::cout << "LOGIN FAILED" << std::endl;
            break;
    }
    
    return status;
}

inline void runGrowidLoginInteractive() {
    LoginInfo loginInfo;
    std::string growid, password, token;
    std::cout << "Input your GrowID: ";
    std::getline(std::cin, growid);
    std::cout << "Input your password: ";
    std::getline(std::cin, password);
    
    LoginStatus status = performGrowidLogin(loginInfo, growid, password, token);
    
    if (status == LoginStatus::Success) {
        std::cout << "Final Token: " << token << std::endl;
    }
}

// Function for integration with client.cpp
inline LoginStatus growidLoginWithLoginInfo(LoginInfo& loginInfo, const std::string& tankIdName, const std::string& tankIdPass, std::string& outToken) {
    return performGrowidLogin(loginInfo, tankIdName, tankIdPass, outToken);
}