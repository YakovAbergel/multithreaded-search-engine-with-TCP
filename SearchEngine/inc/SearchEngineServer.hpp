#ifndef SEARCH_ENGINE_HPP
#define SEARCH_ENGINE_HPP

#include <iostream>
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <string>

#include "crawl.hpp"

namespace h72 {     

class SearchEngine
{
public:
    explicit SearchEngine(std::string const& a_url, size_t a_numOfCrawlThreads, size_t a_numOfPage);
    ~SearchEngine() = default;
    SearchEngine (const SearchEngine&) = delete;
    SearchEngine& operator= (const SearchEngine&) = delete;

    void ThreadRun();

private:
    bool CreatSoket();
    int Recieve(int a_clientSoket, std::vector<std::string>& a_vec);
    void Run (int a_clientSoket, sockaddr_in a_client, int a_result, char* a_host, char* a_svc);
    void CreatServer();

    bool SetSoketReused() const;
    bool Bind() const;
    bool Listen() const;
    int Accept(sockaddr_in& a_client) const;
    int Send (int a_clientSoket, std::vector<std::string>& a_vec) const;
    void CloseSocket (int a_clientSoket, sockaddr_in a_client, int a_result, char* a_host, char* a_svc) const;
    int Wellcome(int a_clientSoket) const;
    bool SendAllData(int a_clientSoket, std::string const& a_msg) const;


private:
    int m_listening; 
    Crawl m_crawler;
};    

}

#endif
