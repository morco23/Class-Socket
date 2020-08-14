#include <cstring>      // memset
#include <iostream>     // io
#include "socket.hpp"

using namespace morcohen;
#define BUFFLEN 100

/*  Run this program and right after that use the utility nc to simulate a client
    this way-> nc localhost 12345 -> type hey from client and you will recieve
    Hello.
*/

int main()
{
    Socket demoSocket(AF_INET, SOCK_STREAM, 0);
    demoSocket.Bind(12345);

    demoSocket.Listen(2);

    std::shared_ptr<Socket> newCon = demoSocket.Accept(0, 0);

    char buffer[BUFFLEN];
    memset(buffer, 0 , BUFFLEN);

    newCon->Recieve(buffer, BUFFLEN, 0);

    std::cout << buffer << std::endl;

    newCon->Send("Hello", 6, 0);

    return 0;
}