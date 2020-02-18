#include "SocketUnit.h"

void SocketUnit::report_error()
{
    LPWSTR buf;
    ::FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL,
        get_last_error(),
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
        (LPWSTR)&buf,
        0,
        NULL
    );
    // 打印
    ::OutputDebugString((LPWSTR)buf);

    ::LocalFree(buf);
}

void SocketUnit::report_error(const char *filename, const char *funname, const int linenum)
{
    char buf[4096] { 0 };
    sprintf(buf, "file: %s, func: %s, num: %d", filename, funname, linenum);
    OutputDebugStringA(buf);
    report_error();
}

int SocketUnit::get_last_error()
{
    return ::WSAGetLastError();
}

std::string SocketUnit::get_ip(sockaddr_in &addr_in)
{
    char ip[INET_ADDRSTRLEN] { 0 };
    inet_ntop(AF_INET, &addr_in.sin_addr, ip, sizeof(ip));
    return std::string(ip);
}

unsigned SocketUnit::get_port(sockaddr_in &addr_in)
{
    unsigned int port = ntohs(addr_in.sin_port);
    return port;
}

std::string SocketUnit::get_ip_port(sockaddr_in &addr_in)
{
    char ip[INET_ADDRSTRLEN] { 0 };
    inet_ntop(AF_INET, &addr_in.sin_addr, ip, sizeof(ip));
    return std::string(ip) + ":" + std::to_string(ntohs(addr_in.sin_port));
}
