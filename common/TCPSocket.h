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

    int create();   // 创建socket
    int close();    // 关闭socket
    int bind();     // 绑定
    int listen(int backlog);    // 监听
    TCPSocket * accept();       // 接受
    int connect(const SocketAddress &addr);  // 连接
    int send(const char *buf, int length);  // 发送
    int recv(char *buf, int length);  // 接收

private:
    int socket_ { -1 };
    SocketAddress address_;
};

#endif