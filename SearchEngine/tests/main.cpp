#include "crawl.hpp"
#include "SearchEngineServer.hpp"

using namespace h72;

int main() 
{   
    std::cout << "Please enter the initial link from which you want the program to start crawling:\n";
    std::string link;
    std::cin >> link;

    std::cout << "\nEnter the number of threads you would like to have in the program:\n";
    size_t numOfThreads;
    std::cin >> numOfThreads;
    
    std::cout << "\nEnter the maximum number of pages you want the program to scan in each scan:\n";
    size_t numOfPages;
    std::cin >> numOfPages;

    std::cout << "\nNow please wait until the crawl is finished and the server is ready to use, then you can activate the user and start searching\n";
    SearchEngine srv(link, numOfThreads, numOfPages);
    srv.ThreadRun();

    return 0;
}