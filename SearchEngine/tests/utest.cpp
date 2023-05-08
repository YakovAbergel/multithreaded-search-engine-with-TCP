// #include "../../inc/mu_test.h"

#include <iostream>
#include <string>
#include <ctime>
#include <thread>

#include "crawl.hpp"

using namespace h72;

int main() 
{    
    Crawl crawl("https://stanfordnlp.github.io/CoreNLP/download.html", 12);

    // std::thread t1{[&crawl](){
    //     crawl.WordSearch("code");
    // }};
    // t1.join();

    // auto linksInPage = crawl.GetLinksInPage();
    while(!crawl.IsScanFinished()) {

    }

    // auto wordsInPage = crawl.WordSearch("stanford");

    // std::vector<std::pair<std::string, size_t>> vec;

    // for(auto& e : wordsInPage) {
    //     vec.emplace_back(std::make_pair(e.first, e.second));
    // }

    // std::sort(vec.begin(), vec.end(), [](const auto &a, const auto &b){return a.second > b.second;});

    // for(auto& v : vec) {
    //     std::cout << v.first << " " << v.second << '\n';
    // }

    return 0;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////

// #include <curl/curl.h>
// #include <iostream>
// #include <string>
// #include <vector>
// #include <sstream>
// #include "rapidxml.hpp"


// using namespace std;
// using namespace rapidxml;

// static size_t WriteMemoryCallback(void *a_contents, size_t a_size, size_t a_nmemb, void *a_userp)
// {
//     size_t realsize = a_size * a_nmemb;
//     std::string *a_mem = (std::string*)a_userp;
//     a_mem->append((char*)a_contents, realsize);
//     return realsize;
// }

// // Function to download the HTML content of a web page using curl
// std::string DownloadHTML(const std::string& a_url) 
// {
//     CURL *curl;
//     std::string html;

//     curl_global_init(CURL_GLOBAL_ALL);
//     curl = curl_easy_init();

//     if(curl) {
//         curl_easy_setopt(curl, CURLOPT_URL, a_url.c_str());
//         curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
//         curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&html);

//         CURLcode res = curl_easy_perform(curl);

//         if (res != CURLE_OK) {
//             std::cerr << "cURL error: " << curl_easy_strerror(res) << std::endl;
//             html.clear();
//         }
//     }

//     curl_easy_cleanup(curl);
//     curl_global_cleanup();
    
//     std::cout << html << '\n';
//     return html;
// }

// vector<string> extractWords(const string& html) {
//     vector<string> words;
//     xml_document<> doc;
//     doc.parse<0>(const_cast<char*>(&html[0]));

//     for (xml_node<>* node = doc.first_node(); node; node = node->next_sibling()) {
//         for (xml_node<>* child = node->first_node(); child; child = child->next_sibling()) {
//             for (xml_node<>* text = child->first_node(); text; text = text->next_sibling()) {
//                 stringstream ss(text->value());
//                 string word;
//                 while (ss >> word) {
//                     words.push_back(word);
//                 }
//             }
//         }
//     }

//     return words;
// }

// int main() {
//     // string html = "<html><body><p>Hello World!</p></body></html>";
//     string url = "https://www.yahoo.com/";
//     string html = DownloadHTML(url);
//     vector<string> words = extractWords(html);
//     cout << "Words extracted from HTML: ";
//     for (const string& word : words) {
//         cout << word << " ";
//     }
//     cout << endl;
//     return 0;
// }