#ifndef _SOCKET_INIT_H_
#define _SOCKET_INIT_H_

#include <winsock2.h>
#include <Windows.h>

#pragma comment(lib, "Ws2_32.lib")

class SocketInit {
public:
    SocketInit();
    ~SocketInit();
};

#endif