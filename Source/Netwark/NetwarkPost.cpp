#include "NetwarkPost.h"

#include <iostream>
#include <ws2tcpip.h>
#pragma comment(lib,"ws2_32.lib")

__fastcall NetwarkPost::~NetwarkPost()
{
    // �\�P�b�g�I��
    if (closesocket(multicastSock) != 0)
    {
        std::cout << "error_code:" << WSAGetLastError();
    }
    if (closesocket(sock) != 0)
    {
        std::cout << "error_code:" << WSAGetLastError();
    }

    // WSA�I��
    WSACleanup();
}

