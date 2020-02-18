#ifndef SERVERCORE_H
#define SERVERCORE_H

#include <QObject>
#include <QStandardItemModel>
#include <vector>
#include <ctime>
#include "../common/TCPSocket.h"
#include "../common/CriticalSection.h"
#include "../common/protocol.h"

struct ClientItem;

class ServerCore : public QObject
{
    Q_OBJECT
public:
    explicit ServerCore(QObject *parent = nullptr);
    ~ServerCore();

    TCPSocket * get_tcp_socket() { return tcp_socket_; }
    inline QStandardItemModel * get_table_model() { return &table_model_; }

    void run(std::string ip, unsigned int port);
    void stop();

    // 请求处理
    void deal_user_login(ClientItem *client, ProtocolHead &head, char *buf);
    void deal_user_logout(ClientItem *client, ProtocolHead &head, char *buf);
    void deal_heart(ClientItem *client, ProtocolHead &head, char *buf);
    void deal_say_all(ClientItem *client, ProtocolHead &head, char *buf);
    void deal_say_to(ClientItem *client, ProtocolHead &head, char *buf);
    void deal_ctrl_screen_request(ProtocolHead &head, char *buf);
    void deal_ctrl_screen_data(ProtocolHead &head, char *buf);
    void deal_ctrl_screen_close(ProtocolHead &head, char *buf);
    void deal_ctrl_mouse(ProtocolHead &head, char *buf);
    void deal_ctrl_keyboard(ProtocolHead &head, char *buf);
    void deal_ctrl_cmd_request(ProtocolHead &head, char *buf);
    void deal_ctrl_cmd_data(ProtocolHead &head, char *buf);
    void deal_ctrl_cmd_exec(ProtocolHead &head, char *buf);
    void deal_ctrl_driver_request(ProtocolHead &head, char *buf);
    void deal_ctrl_driver_data(ProtocolHead &head, char *buf);
    void deal_ctrl_files_request(ProtocolHead &head, char *buf);
    void deal_ctrl_files_data(ProtocolHead &head, char *buf);
    void deal_ctrl_download_file_request(ProtocolHead &head, char *buf);
    void deal_ctrl_download_file_data(ProtocolHead &head, char *buf);
    void deal_ctrl_download_file_complete(ProtocolHead &head, char *buf);
    void deal_ctrl_download_file_cancel(ProtocolHead &head, char *buf);
    void deal_ctrl_upload_file_request(ProtocolHead &head, char *buf);
    void deal_ctrl_upload_file_data(ProtocolHead &head, char *buf);
    void deal_ctrl_upload_file_complete(ProtocolHead &head, char *buf);
    //void deal_ctrl_upload_file_cancel(ProtocolHead &head, char *buf);
    


    ClientItem * remove_client_item(ClientItem *client_item);
    ClientItem * find_client_by_name(std::string name);

signals:
    void append_item(QStringList strings);
    void remove_item(QString name);

public slots:

private:
    TCPSocket *tcp_socket_ = nullptr;
    std::vector<ClientItem *> client_items_;
    CriticalSection critical_section_;  // 用于访问 client_items_ 的关键段

    QStandardItemModel table_model_;

    HANDLE accept_thread_handle = NULL;
    HANDLE heart_thread_handle = NULL;
    bool is_run = false;

protected:
    static unsigned __stdcall accept(void *arg);    // accept线程
    static unsigned __stdcall client_thread(void *arg); // 通信线程
    static unsigned __stdcall heart_thread(void *arg);  // 心跳线程
};


struct ClientItem {
    TCPSocket *tcp_socket;
    clock_t clock_time;
    std::string name;
    HANDLE thread_handle = nullptr;

    ServerCore *server_core = nullptr;

    ClientItem(TCPSocket *sock, clock_t time, std::string user_name = "")
        : tcp_socket(sock), clock_time(time), name(user_name) {}
    ~ClientItem() { close_thread();  delete tcp_socket; ::CloseHandle(thread_handle); }

    inline void close_thread() { ::TerminateThread(thread_handle, 0); }

    // 收包
    int recv_pack(char *buf, int length)
    {
        int current_bytes = 0;
        int total = length;

        while (current_bytes < total) {
            int recv_bytes = tcp_socket->recv(buf + current_bytes, total - current_bytes);
            if (recv_bytes <= 0)
                break;
            current_bytes += recv_bytes;
        }
        return current_bytes;
    }

    // 发包
    int send_pack(const char *buf, int length)
    {
        return tcp_socket->send(buf, length);
    }

    // 解析命令
    void parse_command(ProtocolHead &head, char *buf)
    {
        switch (head.command) {
        case ChatCommand::CHAT_USER_LOGIN:
            server_core->deal_user_login(this, head, buf);
            break;
        case ChatCommand::CHAT_USER_LOGOUT:
            server_core->deal_user_logout(this, head, buf);
            break;
        case ChatCommand::CHAT_HEART:
            server_core->deal_heart(this, head, buf);
            break;
        case ChatCommand::CHAT_SAY_ALL:
            server_core->deal_say_all(this, head, buf);
            break;
        case ChatCommand::CHAT_SAY_TO:
            server_core->deal_say_to(this, head, buf);
            break;
        case ChatCommand::CONTROL_SCREEN_REQUEST:
            server_core->deal_ctrl_screen_request(head, buf);
            break;
        case ChatCommand::CONTROL_SCREEN_DATA:
            server_core->deal_ctrl_screen_data(head, buf);
            break;
        case ChatCommand::CONTROL_SCREEN_CLOSE:
            server_core->deal_ctrl_screen_close(head, buf);
            break;
        case ChatCommand::CONTROL_MOUSE_CLICK:
        case ChatCommand::CONTROL_MOUSE_UPLIFT:
        case ChatCommand::CONTROL_MOUSE_DOUBLE_CLICK:
            server_core->deal_ctrl_mouse(head, buf);
            break;
        case ChatCommand::CONTROL_KEYBOARD_DOWN:
        case ChatCommand::CONTROL_KEYBOARD_UP:
            server_core->deal_ctrl_keyboard(head, buf);
            break;
        case ChatCommand::CONTROL_CMD_REQUEST:
            server_core->deal_ctrl_cmd_request(head, buf);
            break;
        case ChatCommand::CONTROL_CMD_DATA:
            server_core->deal_ctrl_cmd_data(head, buf);
            break;
        case ChatCommand::CONTROL_CMD_EXEC:
            server_core->deal_ctrl_cmd_exec(head, buf);
            break;
        case ChatCommand::CONTROL_DRIVER_REQUEST:
            server_core->deal_ctrl_driver_request(head, buf);
            break;
        case ChatCommand::CONTROL_DRIVER_DATA:
            server_core->deal_ctrl_driver_data(head, buf);
            break;
        case ChatCommand::CONTROL_FILES_REQUEST:
            server_core->deal_ctrl_files_request(head, buf);
            break;
        case ChatCommand::CONTROL_FILES_DATA:
            server_core->deal_ctrl_files_data(head, buf);
            break;
        case ChatCommand::CONTROL_DOWNLOAD_FILE_REQUEST:
            server_core->deal_ctrl_download_file_request(head, buf);
            break;
        case ChatCommand::CONTROL_DOWNLOAD_FILE_DATA:
            server_core->deal_ctrl_download_file_data(head, buf);
            break;
        case ChatCommand::CONTROL_DOWNLOAD_FILE_COMPLETE:
            server_core->deal_ctrl_download_file_complete(head, buf);
            break;
        case ChatCommand::CONTROL_DOWNLOAD_FILE_CANCEL:
            server_core->deal_ctrl_download_file_cancel(head, buf);
            break;
        case ChatCommand::CONTROL_UPLOAD_FILE_REQUEST:
            server_core->deal_ctrl_upload_file_request(head, buf);
            break;
        case ChatCommand::CONTROL_UPLOAD_FILE_DATA:
            server_core->deal_ctrl_upload_file_data(head, buf);
            break;
        case ChatCommand::CONTROL_UPLOAD_FILE_COMPLETE:
            server_core->deal_ctrl_upload_file_complete(head, buf);
            break;
        /*case ChatCommand::CONTROL_UPLOAD_FILE_CANCEL:
            server_core->deal_ctrl_upload_file_cancel(head, buf);
            break;*/
        default:
            break;
        }
    }
};


#endif // SERVERCORE_H
