#include "servercore.h"
#include <process.h>

ServerCore::ServerCore(QObject *parent) : QObject(parent)
{
    // 设置table_model表头
    table_model_.setHorizontalHeaderLabels(QStringList() << "IP" << "Port" << "Name");

    connect(this, static_cast<void (ServerCore::*)(QStringList)>(&ServerCore::append_item),
        this, [=](QStringList strings) {
        QList<QStandardItem *> labels;
        for (auto &item : strings) {
            labels.append(new QStandardItem(item));
        }
        table_model_.appendRow(labels);
    });

    connect(this, static_cast<void (ServerCore::*)(QString)>(&ServerCore::remove_item),
        this, [=](QString name) {
        for (int i = 0; i < table_model_.rowCount(); i++) {
            if (table_model_.index(i, 2).data().toString() == name) {
                table_model_.removeRow(i);
                break;
            }
        }
    });

    // 构造测试包
#ifdef _DEBUG

#endif
}

ServerCore::~ServerCore()
{
    // 释放资源
    for (auto &client_item : client_items_) {
        if (client_item)
            delete client_item;
    }
    stop();
}

void ServerCore::run(std::string ip, unsigned port)
{
    if (!tcp_socket_)
        tcp_socket_ = new TCPSocket;
    if (tcp_socket_->create() != 0)
        return;
    SocketAddress socket_address(ip.c_str(), port);
    tcp_socket_->set_address(socket_address);
    if (tcp_socket_->bind() != 0)
        return;
    if (tcp_socket_->listen(SOMAXCONN) != 0)
        return;

    // 开启接收线程
    is_run = true;
    accept_thread_handle = (HANDLE)_beginthreadex(NULL, 0, accept, this, 0, NULL);
    //heart_thread_handle = (HANDLE)_beginthreadex(NULL, 0, heart_thread, this, 0, NULL);
}

void ServerCore::stop()
{
    is_run = false;
    //::TerminateThread(heart_thread_handle, 0);
    //::WaitForSingleObject(heart_thread_handle, INFINITE);
    ::TerminateThread(accept_thread_handle, 0);
    ::WaitForSingleObject(accept_thread_handle, INFINITE);

    if (tcp_socket_)
        delete tcp_socket_;
    tcp_socket_ = nullptr;
}

void ServerCore::deal_user_login(ClientItem *client, ProtocolHead &head, char *buf)
{
    ChatLogin *chat_login = (ChatLogin *)buf;
    // 查找用户是否存在
    critical_section_.lock();
    if (find_client_by_name(chat_login->name)) {
        // 用户存在
        ProtocolHead protocol_head { 1, (uint16_t)ChatCommand::CHAT_LOGIN_ERROR, 0 };
        client->send_pack((const char *)&protocol_head, sizeof(ProtocolHead));
        auto item = client_items_.back();
        client_items_.pop_back();
        delete item;
        critical_section_.unlock();
        return;
    }
    critical_section_.unlock();

    client->name = chat_login->name;
    client->clock_time = clock();
    // 记录
    QString ip = QString::fromLocal8Bit(client->tcp_socket->get_address().get_ip().c_str());
    QString port = QString::number(client->tcp_socket->get_address().get_port());
    QString name = QString::fromLocal8Bit(client->name.c_str());
    emit append_item(QStringList() << ip << port << name);

    critical_section_.lock();

    // 通知当前用户登录成功
    ProtocolHead protocol_head { 1, (uint16_t)ChatCommand::CHAT_LOGIN_OK, head.len };
    client->send_pack((const char *)&protocol_head, sizeof(ProtocolHead));
    client->send_pack((const char *)chat_login, protocol_head.len);


    // 通知所有用户，有用户登录
    for (auto &item : client_items_) {
        if (item != client) {
            item->send_pack((const char *)&head, sizeof(ProtocolHead));
            item->send_pack((const char *)chat_login, head.len);
        }
    }

    // 通知当前登录用户有哪些用户在线
    for (auto &item : client_items_) {
        if (item != client) {
            client->send_pack((const char *)&head, sizeof(ProtocolHead));
            client->send_pack((const char *)item->name.c_str(), head.len);
        }
    }

    critical_section_.unlock();
}

void ServerCore::deal_user_logout(ClientItem *client, ProtocolHead &head, char *buf)
{
    ChatLogout *chat_logout = (ChatLogout *)buf;

    critical_section_.lock();
    remove_client_item(client);
    emit remove_item(QString::fromLocal8Bit(chat_logout->name));
    // 通知其他用户
    for (auto &item : client_items_) {
        item->send_pack((const char *)&head, sizeof(ProtocolHead));
        item->send_pack((const char *)chat_logout, sizeof(ChatLogin));
    }
    critical_section_.unlock();
    delete client;
}

void ServerCore::deal_heart(ClientItem *client, ProtocolHead &head, char *buf)
{
    client->clock_time = clock();
}

void ServerCore::deal_say_all(ClientItem *client, ProtocolHead &head, char *buf)
{
    // 转发包
    ChatSayALL *chat_say_all = (ChatSayALL *)buf;
    critical_section_.lock();

    for (auto &item : client_items_) {
        if (item != client) {
            item->send_pack((const char *)&head, sizeof(ProtocolHead));
            item->send_pack(buf, head.len);
        }
    }

    critical_section_.unlock();
}

void ServerCore::deal_say_to(ClientItem *client, ProtocolHead &head, char *buf)
{
    ChatSayTo *chat_say_to = (ChatSayTo *)buf;
    critical_section_.lock();
    auto item = find_client_by_name(chat_say_to->target_name);
    if(item) {
        item->send_pack((const char *)&head, sizeof(ProtocolHead));
        item->send_pack(buf, head.len);
    }
    critical_section_.unlock();
}

void ServerCore::deal_ctrl_screen_request(ProtocolHead &head, char *buf)
{
    CTRL_ScreenRequest *ctrl_screen_request = (CTRL_ScreenRequest *)buf;
    critical_section_.lock();

    auto item = find_client_by_name(ctrl_screen_request->target_name);
    if(item) {
        item->send_pack((const char *)&head, sizeof(ProtocolHead));
        item->send_pack((const char *)ctrl_screen_request, head.len);
    }

    critical_section_.unlock();
}

void ServerCore::deal_ctrl_screen_data(ProtocolHead &head, char *buf)
{
    CTRL_ScreenData *ctrl_screen_data = (CTRL_ScreenData *)buf;
    critical_section_.lock();

    auto item = find_client_by_name(ctrl_screen_data->target_name);
    if (item) {
        item->send_pack((const char *)&head, sizeof(ProtocolHead));
        item->send_pack((const char *)ctrl_screen_data, head.len);
    }

    critical_section_.unlock();
}

void ServerCore::deal_ctrl_screen_close(ProtocolHead &head, char *buf)
{
    CTRL_ScreenClose *ctrl_screen_close = (CTRL_ScreenClose *)buf;

    critical_section_.lock();

    auto item = find_client_by_name(ctrl_screen_close->target_name);
    if (item) {
        item->send_pack((const char *)&head, sizeof(ProtocolHead));
        item->send_pack((const char *)ctrl_screen_close, head.len);
    }

    critical_section_.unlock();

}

void ServerCore::deal_ctrl_mouse(ProtocolHead &head, char *buf)
{
    CTRL_Mouse *ctrl_mouse = (CTRL_Mouse *)buf;

    critical_section_.lock();

    auto item = find_client_by_name(ctrl_mouse->target_name);
    if (item) {
        item->send_pack((const char *)&head, sizeof(ProtocolHead));
        item->send_pack((const char *)ctrl_mouse, head.len);
    }

    critical_section_.unlock();
}

void ServerCore::deal_ctrl_keyboard(ProtocolHead &head, char *buf)
{
    CTRL_Keyboard *ctrl_keyboard = (CTRL_Keyboard *)buf;

    critical_section_.lock();

    auto item = find_client_by_name(ctrl_keyboard->target_name);
    if (item) {
        item->send_pack((const char *)&head, sizeof(ProtocolHead));
        item->send_pack((const char *)ctrl_keyboard, head.len);
    }

    critical_section_.unlock();
}

void ServerCore::deal_ctrl_cmd_request(ProtocolHead &head, char *buf)
{
    CTRL_CmdRequest *ctrl_cmd_request = (CTRL_CmdRequest *)buf;
    critical_section_.lock();

    auto item = find_client_by_name(ctrl_cmd_request->target_name);
    if (item) {
        item->send_pack((const char *)&head, sizeof(ProtocolHead));
        item->send_pack((const char *)ctrl_cmd_request, head.len);
    }

    critical_section_.unlock();
}

void ServerCore::deal_ctrl_cmd_data(ProtocolHead &head, char *buf)
{
    CTRL_CmdData *ctrl_cmd_data = (CTRL_CmdData *)buf;
    critical_section_.lock();

    auto item = find_client_by_name(ctrl_cmd_data->target_name);
    if (item) {
        item->send_pack((const char *)&head, sizeof(ProtocolHead));
        item->send_pack((const char *)ctrl_cmd_data, head.len);
    }

    critical_section_.unlock();
}

void ServerCore::deal_ctrl_cmd_exec(ProtocolHead &head, char *buf)
{
    CTRL_CmdExec *ctrl_cmd_exec = (CTRL_CmdExec *)buf;
    critical_section_.lock();

    auto item = find_client_by_name(ctrl_cmd_exec->target_name);
    if (item) {
        item->send_pack((const char *)&head, sizeof(ProtocolHead));
        item->send_pack((const char *)ctrl_cmd_exec, head.len);
    }

    critical_section_.unlock();
}

void ServerCore::deal_ctrl_driver_request(ProtocolHead &head, char *buf)
{
    CTRL_DriverRequest *ctrl_driver_request = (CTRL_DriverRequest *)buf;
    critical_section_.lock();

    auto item = find_client_by_name(ctrl_driver_request->target_name);
    if (item) {
        item->send_pack((const char *)&head, sizeof(ProtocolHead));
        item->send_pack((const char *)ctrl_driver_request, head.len);
    }

    critical_section_.unlock();
}

void ServerCore::deal_ctrl_driver_data(ProtocolHead &head, char *buf)
{
    CTRL_DriverData *ctrl_driver_data = (CTRL_DriverData *)buf;
    critical_section_.lock();

    auto item = find_client_by_name(ctrl_driver_data->target_name);
    if (item) {
        item->send_pack((const char *)&head, sizeof(ProtocolHead));
        item->send_pack((const char *)ctrl_driver_data, head.len);
    }

    critical_section_.unlock();
}

void ServerCore::deal_ctrl_files_request(ProtocolHead &head, char *buf)
{
    CTRL_FilesRequest *ctrl_files_request = (CTRL_FilesRequest *)buf;
    critical_section_.lock();

    auto item = find_client_by_name(ctrl_files_request->target_name);
    if (item) {
        item->send_pack((const char *)&head, sizeof(ProtocolHead));
        item->send_pack((const char *)ctrl_files_request, head.len);
    }

    critical_section_.unlock();
}

void ServerCore::deal_ctrl_files_data(ProtocolHead &head, char *buf)
{
    CTRL_FilesData *ctrl_files_data = (CTRL_FilesData *)buf;
    critical_section_.lock();

    auto item = find_client_by_name(ctrl_files_data->target_name);
    if (item) {
        item->send_pack((const char *)&head, sizeof(ProtocolHead));
        item->send_pack((const char *)ctrl_files_data, head.len);
    }

    critical_section_.unlock();
}

void ServerCore::deal_ctrl_download_file_request(ProtocolHead &head, char *buf)
{
    CTRL_DownloadFileRequest *ctrl_download_file_request = (CTRL_DownloadFileRequest *)buf;
    critical_section_.lock();

    auto item = find_client_by_name(ctrl_download_file_request->target_name);
    if (item) {
        item->send_pack((const char *)&head, sizeof(ProtocolHead));
        item->send_pack((const char *)ctrl_download_file_request, head.len);
    }

    critical_section_.unlock();
}

void ServerCore::deal_ctrl_download_file_data(ProtocolHead &head, char *buf)
{
    CTRL_DownloadFileData *ctrl_download_file_data = (CTRL_DownloadFileData *)buf;
    critical_section_.lock();

    auto item = find_client_by_name(ctrl_download_file_data->target_name);
    if (item) {
        item->send_pack((const char *)&head, sizeof(ProtocolHead));
        item->send_pack((const char *)ctrl_download_file_data, head.len);
    }

    critical_section_.unlock();
}

void ServerCore::deal_ctrl_download_file_complete(ProtocolHead &head, char *buf)
{
    CTRL_DownloadFileComplete *ctrl_download_file_complete = (CTRL_DownloadFileComplete *)buf;
    critical_section_.lock();

    auto item = find_client_by_name(ctrl_download_file_complete->target_name);
    if (item) {
        item->send_pack((const char *)&head, sizeof(ProtocolHead));
        item->send_pack((const char *)ctrl_download_file_complete, head.len);
    }

    critical_section_.unlock();
}

void ServerCore::deal_ctrl_download_file_cancel(ProtocolHead &head, char *buf)
{
    CTRL_DownloadFileCancel *ctrl_download_file_cancel = (CTRL_DownloadFileCancel *)buf;
    critical_section_.lock();

    auto item = find_client_by_name(ctrl_download_file_cancel->target_name);
    if (item) {
        item->send_pack((const char *)&head, sizeof(ProtocolHead));
        item->send_pack((const char *)ctrl_download_file_cancel, head.len);
    }

    critical_section_.unlock();
}

void ServerCore::deal_ctrl_upload_file_request(ProtocolHead &head, char *buf)
{
    CTRL_UploadFileRequest *ctrl_upload_file_request = (CTRL_UploadFileRequest *)buf;
    critical_section_.lock();

    auto item = find_client_by_name(ctrl_upload_file_request->target_name);
    if (item) {
        item->send_pack((const char *)&head, sizeof(ProtocolHead));
        item->send_pack((const char *)ctrl_upload_file_request, head.len);
    }

    critical_section_.unlock();
}

void ServerCore::deal_ctrl_upload_file_data(ProtocolHead &head, char *buf)
{
    CTRL_UploadFileData *ctrl_upload_file_data = (CTRL_UploadFileData *)buf;
    critical_section_.lock();

    auto item = find_client_by_name(ctrl_upload_file_data->target_name);
    if (item) {
        item->send_pack((const char *)&head, sizeof(ProtocolHead));
        item->send_pack((const char *)ctrl_upload_file_data, head.len);
    }

    critical_section_.unlock();
}

void ServerCore::deal_ctrl_upload_file_complete(ProtocolHead &head, char *buf)
{
    CTRL_UploadFileComplete *ctrl_upload_file_complete = (CTRL_UploadFileComplete *)buf;
    critical_section_.lock();

    auto item = find_client_by_name(ctrl_upload_file_complete->target_name);
    if (item) {
        item->send_pack((const char *)&head, sizeof(ProtocolHead));
        item->send_pack((const char *)ctrl_upload_file_complete, head.len);
    }

    critical_section_.unlock();
}

//void ServerCore::deal_ctrl_upload_file_cancel(ProtocolHead &head, char *buf)
//{
//    CTRL_UploadFileCancel *ctrl_upload_file_cancel = (CTRL_UploadFileCancel *)buf;
//    critical_section_.lock();
//
//    auto item = find_client_by_name(ctrl_upload_file_cancel->target_name);
//    if (item) {
//        item->send_pack((const char *)&head, sizeof(ProtocolHead));
//        item->send_pack((const char *)ctrl_upload_file_cancel, head.len);
//    }
//
//    critical_section_.unlock();
//}

ClientItem * ServerCore::remove_client_item(ClientItem *client_item)
{
    auto iter = client_items_.begin();
    while (iter != client_items_.end()) {
        if (*iter == client_item) {
            iter = client_items_.erase(iter);
        } else {
            ++iter;
        }
    }
    return client_item;
}

ClientItem * ServerCore::find_client_by_name(std::string name)
{
    for (auto &item : client_items_) {
        if (item->name == name) {
            critical_section_.unlock();
            return item;
        }
    }
    return nullptr;
}

unsigned ServerCore::accept(void *arg)
{
    ServerCore* server_core = (ServerCore *)arg;

    while (server_core->is_run) {
        TCPSocket *client = server_core->tcp_socket_->accept();
        if (client) {
            ClientItem *client_item = new ClientItem(client, clock());
            client_item->server_core = server_core;
            client_item->thread_handle = (HANDLE)_beginthreadex(NULL, 0, client_thread, client_item, 0, NULL);

            server_core->critical_section_.lock();
            server_core->client_items_.push_back(client_item);
            server_core->critical_section_.unlock();
        }
    }
    return 0;
}

unsigned ServerCore::client_thread(void *arg)
{
    ClientItem *client_item = (ClientItem *)arg;
    while (true) {
        ProtocolHead protocol_head { 0 };
        char *buf = nullptr;
        int recv_bytes = 0;
        if ((recv_bytes = client_item->recv_pack((char *)&protocol_head, sizeof(ProtocolHead))) <= 0) {
            break;;
        }

        if (protocol_head.version == 0)
            continue;

        if (protocol_head.len > 0) {
            buf = new char[protocol_head.len];
            if (client_item->recv_pack(buf, protocol_head.len) <= 0) {
                delete[] buf;
                break;
            }
        }

        client_item->parse_command(protocol_head, buf);

        if (buf)
            delete[] buf;
    }
    return 0;
}

unsigned ServerCore::heart_thread(void *arg)
{
    ServerCore *server_core = (ServerCore *)arg;

    const unsigned int interval_time = 10000;    // 间隔
    const unsigned int timeout = 60000;          // 超时
    while (server_core->is_run) {
        server_core->critical_section_.lock();

        // 遍历客户端
        auto iter = server_core->client_items_.begin();
        while (iter != server_core->client_items_.end()) {
            clock_t current_time = clock();

            auto item = *iter;
            if (current_time - item->clock_time > timeout) {
                // 超时，踢除用户
                iter = server_core->client_items_.erase(iter);
                // 通知所有客户端
                ChatLogout chat_logout { 0 };
                strncpy(chat_logout.name, item->name.c_str(), item->name.length());
                ProtocolHead head { 1, (uint16_t)ChatCommand::CHAT_USER_LOGOUT, sizeof(ChatLogout) };
                for (auto &client_item : server_core->client_items_) {
                    // 发协议头
                    client_item->send_pack((const char *)&head, sizeof(ProtocolHead));
                    // 发数据
                    client_item->send_pack((const char *)&chat_logout, sizeof(ChatLogin));
                }
                // 删除项目
                emit server_core->remove_item(QString::fromLocal8Bit(item->name.c_str()));
                delete item;
            } else {
                // 发心跳包
                ProtocolHead head { 1, (uint16_t)ChatCommand::CHAT_HEART, 0 };
                item->send_pack((const char *)&head, sizeof(ProtocolHead));
                ++iter;
            }
        }


        server_core->critical_section_.unlock();
        ::Sleep(interval_time);
    }
    return 0;
}
