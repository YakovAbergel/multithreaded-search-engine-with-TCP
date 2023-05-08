#include <curl/curl.h>

#include <iostream>
#include <regex>
#include <sstream>
#include <algorithm>

#include "crawl.hpp"

namespace h72 {

static size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp);

Crawl::Crawl(std::string const& a_seedUrl, size_t a_numOfThreads, size_t a_numOfPage)
: m_linksInPage(a_numOfPage)
, m_wordsInPage(1000000)
{
    m_visitedUrls.Insert(a_seedUrl);
    m_urlQueue.PushBack(a_seedUrl);

    for (size_t i = 0; i < a_numOfThreads; ++i) {
        m_threads.emplace_back(&Crawl::CrawlThread, this);
    }
}

Crawl::~Crawl() 
{
    for(auto& thread : m_threads) {
        thread.join();
    }
}

void Crawl::CrawlThread() 
{
    while (true) {
        std::string url;

        m_urlQueue.PopFront(url);

        std::string html = DownloadHTML(url);
        if(html.empty()) {
            continue;
        }

        try {
            ExtractLinks(html);
            ExtractWords(url, html);
        } catch(std::out_of_range const& e) {
            break;
        }
    }

    m_scanFinished = true;
}

bool Crawl::IsScanFinished() const
{
    return m_scanFinished;
}

std::vector<std::string> Crawl::GetTopTenURLs(const std::string& a_words) 
{    
    std::unordered_map<std::string, size_t> mapUrlCounts;
    std::unordered_set<std::string> excludeWords;
    std::vector<std::pair<std::string, size_t>> vecUrlCounts;

    auto searchWords = SplitStringToWords(a_words);

    if(!ProcessSearchWords(searchWords, mapUrlCounts, excludeWords)){
        std::vector<std::string> vec = {"no result found!\n"};
        return vec;
    }

    RemoveExcludedWords(mapUrlCounts, excludeWords);

    SortAndFilterResults(vecUrlCounts, mapUrlCounts);

    return TopTenUrls(vecUrlCounts);
}

std::vector<std::pair<std::string, size_t>> Crawl::WordSearch(std::string const& a_word) 
{
    std::vector<std::pair<std::string, size_t>> vec;

    auto v = m_wordsInPage.Get(a_word);
    if(v.has_value()) {
        auto value = v.value();
        for(auto& e : value) {
            vec.emplace_back(std::make_pair(e.first, e.second));
        }

    }

    return vec;
}

std::string Crawl::DownloadHTML(const std::string& a_url) const
{
    CURL *curl = nullptr;
    std::string html;

    curl_global_init(CURL_GLOBAL_ALL);
    curl = curl_easy_init();

    if(curl) {
        curl_easy_setopt(curl, CURLOPT_URL, a_url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&html);

        CURLcode res = curl_easy_perform(curl);

        if (res != CURLE_OK) {
            // std::cerr << "cURL error: " << curl_easy_strerror(res) << std::endl;
            html.clear();
        }
    }

    curl_easy_cleanup(curl);
    curl_global_cleanup();
    return html;
}

void Crawl::ExtractLinks(const std::string& a_html) 
{
    std::regex linkRegex(R"!!(<\s*a\s+[^>]*href\s*=\s*"((http|https)://[^"]*)"[^>]*>)!!", std::regex_constants::icase);

    for (std::sregex_iterator i = std::sregex_iterator(a_html.begin(), a_html.end(), linkRegex); i != std::sregex_iterator(); ++i) {
        std::smatch match = *i;
        std::string url = match[1];

        UpsertPagsMap(a_html, url);

        if(!m_visitedUrls.Count(url)) {
            m_visitedUrls.Insert(url);
            m_urlQueue.PushBack(url);
        }
    }
}

void Crawl::ExtractWords(const std::string& a_url ,const std::string& a_html) 
{
    GumboOutput* output = gumbo_parse(a_html.c_str());
    std::string text = GetWordsFromNode(output->root);

    std::transform(text.begin(), text.end(), text.begin(),
    [](unsigned char c) -> unsigned char {
        return isalpha(c) ? tolower(c) : ' ';
    });

    std::istringstream iss(text);
    std::string word;
    while(iss >> word) {
        if(word.length() > 3) {
            UpsertWordsMap(word, a_url);
        }
    }
}

void Crawl::UpsertPagsMap(const std::string &a_page, const std::string &a_url) 
{
    m_linksInPage.Upsert(a_page, 
        [&](std::unordered_map<std::string, size_t>& value) {
            if(value.find(a_url) != value.end()) {
                value[a_url] += 1;
            } else {
                value.insert({a_url, 1});
            }
        },
        [&]() { 
            std::unordered_map<std::string, size_t> value;
            value.insert({a_url, 1});
            return value;
        }
    );
}

void Crawl::UpsertWordsMap(const std::string &a_wors, const std::string &a_url) 
{
    m_wordsInPage.Upsert(a_wors, 
        [&](std::unordered_map<std::string, size_t>& value) {
            if(value.find(a_url) != value.end()) {
                value[a_url] += 1;
            } else {
                value.insert({a_url, 1});
            }
        },
        [&]() { 
            std::unordered_map<std::string, size_t> value;
            value.insert({a_url, 1});
            return value;
        }
    );
}

std::string Crawl::GetWordsFromNode(GumboNode* a_node) const
{
    if(a_node->type == GUMBO_NODE_TEXT) {
        return std::string(a_node->v.text.text);
    } else if(a_node->type == GUMBO_NODE_ELEMENT && a_node->v.element.tag != GUMBO_TAG_SCRIPT && a_node->v.element.tag != GUMBO_TAG_STYLE) {
        std::string content = "";
        GumboVector* children = &a_node->v.element.children;
        for(size_t i = 0; i < children->length; ++i) {
            auto words = GetWordsFromNode(static_cast<GumboNode*>(children->data[i]));
            if(i != 0 && !words.empty()) {
                content.append(" ");
            }
            content.append(words);
        }
        return content;
    } else {
        return "";
    }
}

std::vector<std::string> Crawl::SplitStringToWords(const std::string& a_words) const
{
    std::vector<std::string> searchWords;
    std::stringstream stringStream(a_words);
    std::string word;

    while (stringStream >> word) {
        searchWords.push_back(word);
    }

    return searchWords;
}

bool Crawl::ProcessSearchWords(std::vector<std::string> const& a_searchWords, std::unordered_map<std::string, size_t>& a_urlCounts, std::unordered_set<std::string>& a_excludeWords)
{
    for (auto& word : a_searchWords) {
        if(word[0] == '-') {
            a_excludeWords.insert(word.substr(1));
        } else {
            auto vecOfUrlsForWord = WordSearch(word);

            if(vecOfUrlsForWord.empty()){
                return false;
            }

            for(auto& urlAndCount : vecOfUrlsForWord){
                a_urlCounts[urlAndCount.first] += urlAndCount.second;
            }
        }
    }

    return true;
}

void Crawl::RemoveExcludedWords(std::unordered_map<std::string, size_t>& a_urlCounts, std::unordered_set<std::string>& a_excludeWords)
{
    for(auto& word : a_excludeWords) {
        auto vecOfUrlsForWord = WordSearch(word);
        for(auto& urlAndCount: vecOfUrlsForWord) {
            a_urlCounts[urlAndCount.first] = 0;
        }
    }
}

void Crawl::SortAndFilterResults(std::vector<std::pair<std::string, size_t>>& a_vecUrlCounts, std::unordered_map<std::string, size_t> const& a_urlCounts) const
{
    for(auto& urlAndCount : a_urlCounts) {
        if(urlAndCount.second > 0) {
            a_vecUrlCounts.emplace_back(std::make_pair(urlAndCount.first, urlAndCount.second));
        }
    }

    std::sort(a_vecUrlCounts.begin(), a_vecUrlCounts.end(), [](const auto& a, const auto& b) { 
        return a.second > b.second; 
    });
}

std::vector<std::string> Crawl::TopTenUrls(std::vector<std::pair<std::string, size_t>> const& a_vecUrlCounts) const
{
    std::vector<std::string> topURLs;

    for(const auto& url : a_vecUrlCounts) {
        if(topURLs.size() == 10) {
            break;
        }
        topURLs.push_back(url.first);
    }

    if(topURLs.empty()) {
        topURLs.emplace_back("no result found!\n");
    }

    return topURLs;
}

static size_t WriteMemoryCallback(void *a_contents, size_t a_size, size_t a_nmemb, void *a_userp)
{
    size_t realsize = a_size * a_nmemb;
    std::string *a_mem = (std::string*)a_userp;
    a_mem->append((char*)a_contents, realsize);
    return realsize;
}

}
