#ifndef _SOCKET_ADDRESS_H_
#define _SOCKET_ADDRESS_H_

#include <string>
#include <WS2tcpip.h>

class SocketAddress {
public:
    SocketAddress();
    explicit SocketAddress(sockaddr_in &addr);
    SocketAddress(const char *ip, unsigned int port);
    ~SocketAddress();

    std::string get_ip();
    unsigned int get_port();
    inline uint32_t get_address() { return htonl(sock_addr_.sin_addr.S_un.S_addr); }

    //inline sockaddr * socketaddr() { return (sockaddr *)&sock_addr_; }
    inline sockaddr * get_sockaddr() const { return (sockaddr *)&sock_addr_; }
    inline int get_sockaddr_size() const { return sizeof(sock_addr_); }

    friend bool operator==(SocketAddress &myself, SocketAddress &other)
    {
        return myself == other.sock_addr_;
    }
    friend bool operator==(SocketAddress &myself, sockaddr_in &addr)
    {
        return memcmp(&myself.sock_addr_, &addr, sizeof(sockaddr_in)) == 0;
    }

private:
    sockaddr_in sock_addr_ { 0 };
};

#endif