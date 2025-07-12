#pragma once
#include <string>
#include <regex>
#include <iostream>
#include <stdexcept>
#include <format>
#include <functional>
#include "http_get.hpp"
#include <nlohmann/json.hpp>
#include <curl/curl.h>

// Growtopia Authentication Utilities
// This file contains authentication functions and structures for Growtopia login

// Constants
const std::string USER_AGENT = "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36";
const std::string LOGIN_BASE_URL = "https://login.growtopiagame.com";
const std::string VAL_KEY = "40db4045f2d8c572efe8c4a060605726";
    
// HTTP response structure
struct http_response_t {
    std::string body;
    std::map<std::string, std::string> headers;
    int status_code;
};
    
// HTTP client interface
class HttpClient {
    private:
        static size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* userp) {
            userp->append((char*)contents, size * nmemb);
            return size * nmemb;
    }
        
    public:
        std::string urlEncode(const std::string& str) {
            CURL* curl = curl_easy_init();
            if (!curl) {
                std::cerr << "Failed to initialize CURL" << std::endl;
                return str;
            }
            
            char* encoded = curl_easy_escape(curl, str.c_str(), str.length());
            if (!encoded) {
                curl_easy_cleanup(curl);
                return str;
            }
            
            std::string result(encoded);
            curl_free(encoded);
            curl_easy_cleanup(curl);
            return result;
        }
        
        // Synchronous POST method
        http_response_t post_sync(const std::string& url, const std::string& data, 
                                 const std::vector<std::string>& headers) {
            CURL* curl = curl_easy_init();
            if (!curl) {
                return {"CURL initialization failed", {}, 0};
            }
            
            std::string response_string;
            struct curl_slist* header_list = nullptr;
            
            // Set headers
            for (const auto& header : headers) {
                header_list = curl_slist_append(header_list, header.c_str());
            }
            
            curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.c_str());
            curl_easy_setopt(curl, CURLOPT_HTTPHEADER, header_list);
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_string);
            curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
            curl_easy_setopt(curl, CURLOPT_TIMEOUT, 30L);
            
            CURLcode res = curl_easy_perform(curl);
            long response_code;
            curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
            
            curl_slist_free_all(header_list);
            curl_easy_cleanup(curl);
            
            return {response_string, {}, static_cast<int>(response_code)};
        }
        
        // Synchronous GET method
        http_response_t get_sync(const std::string& url, const std::vector<std::string>& headers) {
            CURL* curl = curl_easy_init();
            if (!curl) {
                return {"CURL initialization failed", {}, 0};
            }
            
            std::string response_string;
            struct curl_slist* header_list = nullptr;
            
            // Set headers
            for (const auto& header : headers) {
                header_list = curl_slist_append(header_list, header.c_str());
            }
            
            curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
            curl_easy_setopt(curl, CURLOPT_HTTPHEADER, header_list);
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_string);
            curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
            curl_easy_setopt(curl, CURLOPT_TIMEOUT, 30L);
            
            CURLcode res = curl_easy_perform(curl);
            long response_code;
            curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
            
            curl_slist_free_all(header_list);
            curl_easy_cleanup(curl);
            
            return {response_string, {}, static_cast<int>(response_code)};
        }
};
    
// Account structure
struct Account {
    std::string growid;
    std::string password;
    std::string ltoken;
    std::string Oldltoken;
};
    
// Status enumeration
enum Status {
    GettingToken,
    GotToken,
    ErrorGettingToken,
    WRONGPASS,
    TOOMANYLOGIN
};
    
// Extract CSRF token from HTML response
std::string extract_token_from_html(const std::string& body) {
        std::string token;
        const std::string tokenName = "name=\"_token\"";
        const std::string tokenType = "type=\"hidden\"";
        const std::string valueKey = "value=\"";
        
        size_t pos = body.find(tokenName);
        if (pos != std::string::npos) {
            pos = body.find(tokenType, pos);
            if (pos != std::string::npos) {
                pos = body.find(valueKey, pos);
                if (pos != std::string::npos) {
                    pos += valueKey.length();
                    size_t endPos = body.find("\"", pos);
                    if (endPos != std::string::npos) {
                        token = body.substr(pos, endPos - pos);
                    }
                }
            }
        }
        
        return token;
    }
    
// Extract legacy redirect URL
std::string extractLegacyRedirectURL(const std::string& html) {
        std::regex pattern(R"(href=\"(https:\/\/login\.growtopiagame\.com\/player\/growid\/login\?token=[^\"]+))");
        std::smatch match;
        if (std::regex_search(html, match, pattern)) {
            return match[1].str();
        }
        return "";
    }
    
/**
 * Create standard HTTP headers for web requests
 * @return Vector of header strings for standard web requests
 */
std::vector<std::string> createStandardHeaders() {
    return {
        "User-Agent: " + USER_AGENT,
        "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3;q=0.7",
        "Content-Type: application/x-www-form-urlencoded",
        "Host: login.growtopiagame.com"
    };
}
    
/**
 * Create API-specific headers for checktoken requests
 * @return Vector of header strings for API requests
 */
std::vector<std::string> createApiHeaders() {
    return {
        "User-Agent: UbiServices_SDK_2022.Release.9_PC64_ansi_static",
        "Accept: */*",
        "Content-Type: application/x-www-form-urlencoded",
        "Host: login.growtopiagame.com"
    };
}
    
/**
 * Check if response contains common login error messages
 * @param response HTTP response body to check
 * @return true if error messages are found
 */
bool hasLoginError(const std::string& response) {
    return response.find("Oops, too many people logging at once.") != std::string::npos ||
           response.find("Redirecting") != std::string::npos ||
           response.find("Error") != std::string::npos;
}
    
/**
 * Validate token format and length
 * @param token Token string to validate
 * @return true if token appears to be valid
 */
bool isValidToken(const std::string& token) {
    return !token.empty() && token.length() > 10 && token.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789+/=") == std::string::npos;
}
    
/**
 * Clean token by removing escape characters
 * @param token Raw token string
 * @return Cleaned token string
 */
std::string cleanToken(const std::string& token) {
    std::string cleaned = token;
    size_t pos = 0;
    while ((pos = cleaned.find("\\", pos)) != std::string::npos) {
        cleaned.replace(pos, 1, "");
        pos += 1;
    }
    return cleaned;
}
    
/**
 * Synchronous authentication function
 * Handles multi-step authentication process:
 * 1. GET request to dashboard URL
 * 2. Check if response is 200
 * 3. Extract token from HTML
 * 4. Use token to access login URL
 * 5. Extract _token from response
 * 6. POST to validate endpoint with _token, growId, and password
 * 
 * @param account Reference to account data
 * @param http_client HTTP client for making requests
 * @param CreateLoginData Function to create login data
 * @return Status of the authentication process
 */
Status authenticate_sync(Account& account, HttpClient& http_client,
                        std::function<std::string()> CreateLoginData) {
    
    std::cout << "Requesting dashboard from login.growtopiagame.com" << '\n';
    
    // Step 1: GET request to dashboard URL
    const std::string dashboard_url = LOGIN_BASE_URL + "/player/login/dashboard?valKey=" + VAL_KEY;
    std::cout << "Dashboard URL: " << dashboard_url << '\n';
    
    auto dashboard_response = http_client.get_sync(dashboard_url, createStandardHeaders());
    std::cout << "Dashboard response status: " << dashboard_response.status_code << '\n';
    
    // Step 2: Check if response is 200
    if (dashboard_response.status_code != 200) {
        std::cout << "Dashboard request failed with status: " << dashboard_response.status_code << '\n';
        return ErrorGettingToken;
    }
    
    std::cout << "Dashboard response body: " << dashboard_response.body << '\n';
    
    // Step 3: Extract token from HTML
    std::string redirect_url = extractLegacyRedirectURL(dashboard_response.body);
    if (redirect_url.empty()) {
        std::cout << "Failed to extract redirect URL from dashboard" << '\n';
        return ErrorGettingToken;
    }
    
    std::cout << "Got redirect URL: " << redirect_url << '\n';
    
    // Step 4: Use token to access login URL
    auto login_response = http_client.get_sync(redirect_url, createStandardHeaders());
    std::cout << "Login page response: " << login_response.body << '\n';
    
    // Step 5: Extract _token from response
    std::string csrf_token = extract_token_from_html(login_response.body);
    if (csrf_token.empty()) {
        std::cout << "Failed to extract _token from login page" << '\n';
        return ErrorGettingToken;
    }
    
    std::cout << "Got _token: " << csrf_token << '\n';
    
    // Step 6: POST to validate endpoint
    auto validate_response = http_client.post_sync(
        LOGIN_BASE_URL + "/player/growid/login/validate",
        std::format("_token={}&growId={}&password={}", 
                   http_client.urlEncode(csrf_token), 
                   http_client.urlEncode(account.growid), 
                   http_client.urlEncode(account.password)),
        createStandardHeaders()
    );
    
    std::string resp = validate_response.body;
    std::cout << "Validate response: " << resp << '\n';
    
    if (resp.find("Oops, too many people logging at once.") != std::string::npos) {
        return TOOMANYLOGIN;
    }
    
    if (!nlohmann::json::accept(resp)) {
        std::cout << "Invalid JSON response from validate" << '\n';
        return ErrorGettingToken;
    }
    
    nlohmann::json r = nlohmann::json::parse(resp);
    if (r["token"].is_null()) {
        if (resp.find("Redirecting") != std::string::npos) {
            return WRONGPASS;
        } else {
            std::cout << "Error while getting token" << '\n';
            return ErrorGettingToken;
        }
    }
    
    auto token = r["token"].get<std::string>();
    account.ltoken = token;
    account.Oldltoken = token;
    
    // Refresh token via checktoken endpoint
    std::string debugSTR = std::format("refreshToken={}&clientData={}", 
                                      http_client.urlEncode(account.Oldltoken), 
                                      http_client.urlEncode(CreateLoginData()));
    std::cout << "Checktoken request: " << debugSTR << '\n';
    
    auto checktoken_response = http_client.post_sync(
        LOGIN_BASE_URL + "/player/growid/checktoken?valKey=" + VAL_KEY,
        debugSTR,
        createApiHeaders()
    );
    
    std::string checktoken_resp = checktoken_response.body;
    std::cout << "Checktoken response: " << checktoken_resp << '\n';
    
    if (!nlohmann::json::accept(checktoken_resp)) {
        account.ltoken = "";
        return ErrorGettingToken;
    }
    
    nlohmann::json checktoken_r = nlohmann::json::parse(checktoken_resp);
    if (checktoken_r["token"].is_null() || checktoken_r["token"].get<std::string>() == "") {
        account.ltoken = "";
        return ErrorGettingToken;
    }
    
    auto final_token = cleanToken(checktoken_r["token"].get<std::string>());
    if (!isValidToken(final_token)) {
        account.ltoken = "";
        std::cout << "Invalid token received" << '\n';
        return ErrorGettingToken;
    }
    
    account.Oldltoken = final_token;
    account.ltoken = final_token;
    return GotToken;
}