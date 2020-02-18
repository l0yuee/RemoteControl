#include "SocketAddress.h"
#include "SocketUnit.h"

SocketAddress::SocketAddress()
{
}

SocketAddress::SocketAddress(sockaddr_in &addr) : sock_addr_(addr)
{
}

SocketAddress::SocketAddress(const char *ip, unsigned int port)
{
    ::inet_pton(AF_INET, ip, &sock_addr_.sin_addr);
    sock_addr_.sin_port = ::htons(port);
    sock_addr_.sin_family = AF_INET;
}

SocketAddress::~SocketAddress()
{
}

std::string SocketAddress::get_ip()
{
    return SocketUnit::get_ip(sock_addr_);
}

unsigned int SocketAddress::get_port()
{
    return SocketUnit::get_port(sock_addr_);
}