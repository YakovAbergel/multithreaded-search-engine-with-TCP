#include "SearchEngineServer.hpp"
#include <exception>
#include <thread>
#include <algorithm>
		

namespace h72 {

SearchEngine::SearchEngine(std::string const& a_url, size_t a_numOfCrawlThreads, size_t a_numOfPage)
: m_crawler(a_url, a_numOfCrawlThreads, a_numOfPage)
{
}

bool SearchEngine::CreatSoket ()
{
    m_listening = socket (AF_INET, SOCK_STREAM, 0);
    if (m_listening < 0){
       return false;        
    }
    return true;
}

bool SearchEngine::SetSoketReused() const
{
    int optval = 1;
    if (setsockopt(m_listening, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) < 0){
        std::cout<< strerror(errno)<<'\n';
        close(m_listening);
        return false;
    }
    return true;
}

bool SearchEngine::Bind() const
{
    sockaddr_in hint;
    memset(&hint, 0, sizeof(hint));
    hint.sin_family = AF_INET;
    hint.sin_addr.s_addr = INADDR_ANY;
    hint.sin_port = htons(5403);
    int bindResult = bind(m_listening, (sockaddr*)&hint, sizeof(hint));
    if (bindResult < 0){
        std::cout << bindResult <<'\n';
        std::cout<< strerror(errno)<<'\n';
        close (m_listening);
        return false;
    }
    return true;
}
   
bool SearchEngine::Listen() const
{
    if (listen(m_listening, SOMAXCONN) == -1){
        std::cout<< strerror(errno)<<'\n';
        close (m_listening);
        return false;
    }
    return true;
}

int SearchEngine::Accept(sockaddr_in& a_client) const
{
    // sockaddr_in client;
    socklen_t clientSize = sizeof (a_client);
    int clientSoket = accept(m_listening, (sockaddr*)&a_client, &clientSize);
    return clientSoket;
}

int SearchEngine::Recieve(int a_clientSoket, std::vector<std::string>& a_vec)
{
    std::string url;
    char buffer[4096];
    memset(buffer,0,4096);

    int byteRecv = 0;
    while (true) {
        byteRecv = recv(a_clientSoket, buffer, sizeof(buffer), 0);
        if (byteRecv < 0) {
            return -1;
        }
        url += buffer;
        if (url.back() == '\n') {
            break;
        }
    }

    url.erase(std::find_if(url.rbegin(), url.rend(), [](unsigned char ch) {
        return !std::isspace(ch);
    }).base(), url.end());

    if(url == "q" || url == "Q") {
        return 0;
    }

    a_vec = m_crawler.GetTopTenURLs(url);

    return 1;
}

void SearchEngine::Run (int a_clientSoket, sockaddr_in a_client, int a_result, char* a_host, char* a_svc)
{
    std::vector<std::string> vec;
    while (true){
        if(Wellcome(a_clientSoket) == -1){
            break;
        }
        if(Recieve(a_clientSoket, vec) != 1){
            break;
        }
        if(Send(a_clientSoket, vec) == -1) {
            break;
        } 
    }

    CloseSocket (a_clientSoket, a_client, a_result, a_host, a_svc);
}

bool SearchEngine::SendAllData(int a_clientSoket, std::string const& a_msg) const
{
    size_t totalSend = 0;
    while (totalSend < a_msg.size()){
        int sentBytes = send(a_clientSoket, a_msg.data() + totalSend, a_msg.size() - totalSend, 0);
        if (sentBytes < 0) {
            std::cout << "sending error\n";
            std::cout<< strerror(errno)<<'\n';
            return false;
        }     
        totalSend += sentBytes;
    }    
    return true;
}

int SearchEngine::Send(int a_clientSoket, std::vector<std::string>& a_vec) const
{        
    for(auto& link : a_vec) {
        link.append("\n");
        if(!SendAllData(a_clientSoket, link)) {
            return -1;
        }
    }

    if(!SendAllData(a_clientSoket, "\n")) {
        return -1;
    }

    return 1;
}

int SearchEngine::Wellcome(int a_clientSoket) const
{
    if(!SendAllData(a_clientSoket, "Search with Y&A:\n")) {
        return -1;
    }

    return 1;
}

void SearchEngine::CloseSocket(int a_clientSoket, sockaddr_in a_client, int a_result, char* a_host, char* a_svc) const
{
    if (a_result) {
        std::cout << a_host << " disconected from " << a_svc << std::endl;
    } else {
        inet_ntop(AF_INET, &a_client.sin_addr, a_host, NI_MAXHOST);
        std::cout << a_host << " disconected from " << ntohs(a_client.sin_port) << std::endl;
    }    
    close(a_clientSoket);      
}
    
void SearchEngine::CreatServer()
{    
    if (CreatSoket() == false){
        std::cout<< "Can't creat a socket!\n";
        std::exit(0);
    }
    if (SetSoketReused() == false){
        std::cout<< "reuse failed\n";
        std::exit(0); 
    }
    if (Bind() == false){
        std::cout<< "Cant bind to IP/port\n";
        std::exit(0);
    }
    if (Listen() == false){
        std::cout<< "cant listen\n";
        std::exit(0);
    }
    std::cout << "Server is ready to use!\n";
}

    
void SearchEngine::ThreadRun()
{
    while(!m_crawler.IsScanFinished()) {

    }
    std::cout << "crawl finished\n";

    CreatServer();

    while (true){
        sockaddr_in client;
        int clientSoket = Accept(client);
        if (clientSoket > 0){
            char host[NI_MAXHOST];
            char svc[NI_MAXSERV];
            memset (host,0, NI_MAXHOST);
            memset (svc, 0, NI_MAXSERV);
            int result = getnameinfo ((sockaddr*)&client, sizeof(client), host, NI_MAXHOST, svc, NI_MAXHOST, 0);
            if (result) {
                std::cout << host << " connected on " << svc << std::endl;
            } else {
                inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST);
                std::cout << host << " connected on " << ntohs(client.sin_port) << std::endl;
            }
            std::thread temp {&SearchEngine::Run, this, clientSoket, client, result, host, svc};
            temp.detach();
        }
    }
    // CloseSocket(m_listening);
}

}
