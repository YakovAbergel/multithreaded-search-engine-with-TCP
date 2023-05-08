#include "iostream"
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <string>



int main ()
{
    int listening = socket (AF_INET, SOCK_STREAM, 0);
    if (listening < 0) {
        std::cerr << "Can't creat a socket!\n";
        return -1;
    }

    sockaddr_in hint;
    hint.sin_family = AF_INET;
    hint.sin_port = htons(8080);
    inet_pton(AF_INET,"0.0.0.0", &hint.sin_addr);

    if (bind(listening, (sockaddr*)&hint, sizeof(hint)) < 0){
        std::cerr << "Cant bind to IP/port\n";
        return -2;
    }

    if (listen(listening, SOMAXCONN) == -1){
        std::cerr << "cant listen\n";
        return -3;
    }     

    sockaddr_in client;
    socklen_t clientSize = sizeof(client);
    char host[NI_MAXHOST];
    char svc[NI_MAXSERV];

    int clientSoket = accept(listening, (sockaddr*)&client, &clientSize);
    if (clientSoket == -1){
        std::cout << "Problem with client connecting\n";
        return -4;
    }

    close (listening);

    memset (host,0, NI_MAXHOST);
    memset (svc, 0, NI_MAXSERV);

    int result = getnameinfo ((sockaddr*)&client, sizeof(client), host, NI_MAXHOST, svc, NI_MAXHOST, 0);
    if (result)
    {
        std::cout << host << " connected on " << svc << std::endl;
    } else {
        inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST);
        std::cout << host << " connected on " << ntohs(client.sin_port) << std::endl;
    }

    char buf[4096];
    while (true)
    {
        memset (buf,0,4096);
        int byteRecv = recv (clientSoket, buf, 4096, 0);
        if (byteRecv < 0){
            std::cerr << "There was connection issue" << std::endl;
            break;
            //return -5;
        }
        if (byteRecv == 0){
            std::cout << "The clientd disconected" << std::endl;
            break;
        }

        std::cout << "recieved: " << std::string (buf, 0, byteRecv) << std::endl;
        std::cin >> buf;
        int sentBytes = send (clientSoket, buf, sizeof(buf), 0);
        if (sentBytes != sizeof(buf)){
            std::cerr << "send failed" << std::endl;
            return -5;
        }
    }

    close (clientSoket);

    return 0;
}
