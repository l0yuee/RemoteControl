#ifndef _SOCKET_UNIT_H_
#define _SOCKET_UNIT_H_

#include <WS2tcpip.h>
#include <string>

class SocketUnit {
public:
    static void report_error();
    static void report_error(const char *filename, const char *funname, const int linenum);
    static int get_last_error();
    static std::string get_ip(sockaddr_in &addr_in);
    static unsigned int get_port(sockaddr_in &addr_in);
    static std::string get_ip_port(sockaddr_in &addr_in);
};

#endif
