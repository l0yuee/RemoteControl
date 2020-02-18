#include "TCPSocket.h"
#include "SocketUnit.h"

TCPSocket::TCPSocket()
{
}

TCPSocket::TCPSocket(int sd) : socket_(sd)
{
}

TCPSocket::TCPSocket(int sd, SocketAddress &addr) : socket_(sd), address_(addr)
{
}


TCPSocket::~TCPSocket()
{
    close();
}

int TCPSocket::create()
{
    socket_ = (int)::socket(AF_INET, SOCK_STREAM, 0);
    if (socket_ < 0) {
        SocketUnit::report_error(__FILE__, __FUNCTION__, __LINE__);
        return SocketUnit::get_last_error();
    }
    return 0;
}

int TCPSocket::close()
{
    int ret = 0;
    if (socket_ != -1) {
        ret = ::closesocket(socket_);
        socket_ = -1;
    }
    return ret;
}

int TCPSocket::bind()
{
    if (::bind(socket_, address_.get_sockaddr(), address_.get_sockaddr_size()) != 0) {
        SocketUnit::report_error(__FILE__, __FUNCTION__, __LINE__);
        return SocketUnit::get_last_error();
    }
    return 0;
}

int TCPSocket::listen(int backlog)
{
    if (::listen(socket_, backlog) != 0) {
        SocketUnit::report_error(__FILE__, __FUNCTION__, __LINE__);
        return SocketUnit::get_last_error();
    }
    return 0;
}

TCPSocket * TCPSocket::accept()
{
    SocketAddress address;
    int addr_len = address.get_sockaddr_size();
    int sd = (int)::accept(socket_, address.get_sockaddr(), &addr_len);
    if (sd < 0) {
        SocketUnit::report_error(__FILE__, __FUNCTION__, __LINE__);
        return nullptr;
    }
    return new TCPSocket(sd, address);
}

int TCPSocket::connect(const SocketAddress &addr)
{
    if (::connect(socket_, addr.get_sockaddr(), addr.get_sockaddr_size()) != 0) {
        SocketUnit::report_error(__FILE__, __FUNCTION__, __LINE__);
        return SocketUnit::get_last_error();
    }
    return 0;
}

int TCPSocket::send(const char *buf, int length)
{
    int sent_bytes = ::send(socket_, buf, length, 0);
    if (sent_bytes < 0) {
        SocketUnit::report_error(__FILE__, __FUNCTION__, __LINE__);
    }
    return sent_bytes;
}

int TCPSocket::recv(char *buf, int length)
{
    int recv_bytes = ::recv(socket_, buf, length, 0);
    if (recv_bytes < 0) {
        SocketUnit::report_error(__FILE__, __FUNCTION__, __LINE__);
    }
    return recv_bytes;
}
