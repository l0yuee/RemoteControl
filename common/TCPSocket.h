#ifndef _TCP_SOCKET_H_
#define _TCP_SOCKET_H_

#include "SocketAddress.h"

class TCPSocket {
public:
    TCPSocket();
    explicit TCPSocket(int sd);
    TCPSocket(int sd, SocketAddress &addr);
    ~TCPSocket();

    inline int get_socket() const { return socket_; }
    inline SocketAddress get_address() const { return address_; }
    inline void set_socket(int sd) { socket_ = sd; }
    inline void set_address(SocketAddress &addr) { address_ = addr; }

    int create();   // ����socket
    int close();    // �ر�socket
    int bind();     // ��
    int listen(int backlog);    // ����
    TCPSocket * accept();       // ����
    int connect(const SocketAddress &addr);  // ����
    int send(const char *buf, int length);  // ����
    int recv(char *buf, int length);  // ����

private:
    int socket_ { -1 };
    SocketAddress address_;
};

#endif