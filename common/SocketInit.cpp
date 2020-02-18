#include "SocketInit.h"

// 全局socket初始化对象
SocketInit socket_init;

SocketInit::SocketInit()
{
    // 初始化socket环境
    WORD wVersionRequested = MAKEWORD(2, 2);
    WSADATA wsaData;
    if (::WSAStartup(wVersionRequested, &wsaData)) {
        return;
    }
}

SocketInit::~SocketInit()
{
    ::WSACleanup();
}
