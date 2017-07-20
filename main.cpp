#include <iostream>
#include <curl/curl.h>
#include <vector>
#include <future>

using downloadCbPtr = void (const char *url, const char *errorText, const long resSize);
using taskFuture = std::future<long>;

long addChunkSize(char *ptr, size_t size, size_t nmemb, void *data);
void downloadCb(const char *url, const char *errorText, const long resSize);
long downloadTask(const char *url, downloadCbPtr cb);

int main(int argc, const char *argv[])
{
    if (argc == 1)
    {
        std::cerr << "Usage: " << argv[0] << " url1 url2 ..." << std::endl;
        return 1;
    }

    std::vector<taskFuture> tasks = std::vector<taskFuture>();

    for (int i = 1; i < argc; i++)
    {
        const char *url = argv[i];
        tasks.push_back(std::async(std::launch::async, downloadTask, url, downloadCb));
    }

    long sumResSize = 0;
    for (taskFuture &task : tasks)
    {
        sumResSize += task.get();
    }

    std::cout << "--" << std::endl
              << "Sum size: " << sumResSize << std::endl;

    return 0;
}

long addChunkSize(char *ptr, size_t size, size_t nmemb, void *data)
{
    long *acc = static_cast<long *>(data);
    long chunkSize = size * nmemb;
    *acc += chunkSize;

    return chunkSize;
}


void downloadCb(const char *url, const char *errorText, const long resSize)
{
    if (errorText)
    {
        std::cerr << "Download error (" << url << "): " << errorText << std::endl;
    }
    else
    {
        std::cout << "Downloaded (" << url << "): " << resSize << std::endl;
    }
}

long downloadTask(const char *url, downloadCbPtr *cb)
{
    CURL *curl;
    CURLcode resCode;
    long resSize = 0;

    curl = curl_easy_init();

    if (curl)
    {
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &resSize);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, addChunkSize);

        resCode = curl_easy_perform(curl);

        const char *errorText = resCode != CURLE_OK ? curl_easy_strerror(resCode) : nullptr;
        cb(url, errorText, resSize);

        curl_easy_cleanup(curl);
    }

    return resSize;
}
