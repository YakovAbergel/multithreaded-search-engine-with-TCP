#ifndef CRAWL_HPP
#define CRAWL_HPP

#include <gumbo.h>

#include <string>
#include <thread>
#include <vector>
#include <unordered_set>
#include <map>
#include <atomic>
#include <condition_variable>
#include <mutex>

#include "threadSafeQueue.hpp"
#include "threadSafeHashTable.hpp"
#include "threadSafeSet.hpp"

namespace h72 {

class Crawl {
public:
    explicit Crawl(std::string const& a_seedUrl, size_t a_numOfThreads = 4, size_t a_numOfPage = 10);
    ~Crawl();
    Crawl(Crawl const&) = delete;
    Crawl& operator=(Crawl const&) = delete;

    std::vector<std::string> GetTopTenURLs(std::string const& a_words);
    bool IsScanFinished() const;

private:
    void CrawlThread();
    void ExtractLinks(std::string const& a_html);
    void ExtractWords(std::string const& a_url ,std::string const& a_html);
    void UpsertPagsMap(std::string const& a_page, std::string const& a_url);
    void UpsertWordsMap(std::string const& a_wors, std::string const& a_url);
    bool ProcessSearchWords(std::vector<std::string> const& a_searchWords, std::unordered_map<std::string, size_t>& a_urlCounts, std::unordered_set<std::string>& a_excludeWords);
    void RemoveExcludedWords(std::unordered_map<std::string, size_t>& a_urlCounts, std::unordered_set<std::string>& a_excludeWords);
    std::vector<std::pair<std::string, size_t>> WordSearch(std::string const& a_word);

    void SortAndFilterResults(std::vector<std::pair<std::string, size_t>>& a_vecUrlCounts, std::unordered_map<std::string, size_t> const& a_urlCounts) const;
    std::string DownloadHTML(std::string const& a_url) const;
    std::string GetWordsFromNode(GumboNode* a_node) const;
    std::vector<std::string> SplitStringToWords(std::string const& a_words) const;
    std::vector<std::string> TopTenUrls(std::vector<std::pair<std::string, size_t>> const& a_vecUrlCounts) const;

private:
    ThreadSafeQueue<std::string> m_urlQueue;
    ThreadSafeSet<std::string> m_visitedUrls;
    ThreadSafeHashTable<std::string, std::unordered_map<std::string, size_t>> m_linksInPage;
    ThreadSafeHashTable<std::string, std::unordered_map<std::string, size_t>> m_wordsInPage;
    std::vector<std::thread> m_threads;
    std::atomic<bool> m_scanFinished = false;
};

}

#endif
