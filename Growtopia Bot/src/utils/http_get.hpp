#include <string>
#include <curl/curl.h>
static size_t write_callback(void* contents, size_t size, size_t nmemb, void* userp)
{
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

size_t write_callback_file(void* contents, size_t size, size_t nmemb, void* userp) {
    std::ofstream* outfile = static_cast<std::ofstream*>(userp);
    size_t totalSize = size * nmemb;
    outfile->write(static_cast<char*>(contents), totalSize);
    return totalSize;
}


std::string http_get(std::string uri) {
    CURL* curl;
    CURLcode res;
    std::string readBuffer;

    curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, uri.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
        res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);

        return readBuffer;
    }
    return "";
}

void download_image_from_url(const std::string& url, const std::string& filename) {
    CURL* curl = curl_easy_init();
    if (curl) {
        std::ofstream outfile(filename, std::ios::binary);
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback_file);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &outfile);
        curl_easy_perform(curl);
        curl_easy_cleanup(curl);
        outfile.close();
    }
}