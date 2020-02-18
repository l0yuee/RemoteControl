#include "clientcore.h"
#include <process.h>
#include <QApplication>
#include <QScreen>
#include <QPainter>
#include <QBuffer>
#include <QDebug>
#include <QStorageInfo>
#include <QDateTime>

#pragma comment(lib, "User32.lib")

ClientCore::ClientCore(QObject *parent) : QObject(parent)
{
    // 设置table_model表头
    table_model_.setHorizontalHeaderLabels(QStringList() << "Name");

    connect(this, static_cast<void (ClientCore::*)(QString)>(&ClientCore::append_item),
        this, [=](QString name) {
        table_model_.appendRow(new QStandardItem(name));
    });


    connect(this, static_cast<void (ClientCore::*)(QString)>(&ClientCore::remove_item),
        this, [=](QString name) {
        for (int i = 0; i < table_model_.rowCount(); i++) {
            if (table_model_.index(i, 0).data().toString() == name) {
                table_model_.removeRow(i);
                break;
            }
        }
    });
}

ClientCore::~ClientCore()
{
    /*if(work_thread_handle)
        ::TerminateThread(work_thread_handle, 0);*/
    if (tcp_socket_)
        delete tcp_socket_;
    if (server_address)
        delete server_address;
}

void ClientCore::run(QString &server_ip, QString &server_port, QString &name)
{
    tcp_socket_ = new TCPSocket();
    tcp_socket_->create();

    std::string ip = server_ip.toLocal8Bit().data();
    unsigned port = server_port.toUInt();
    std::string  user_name = name.toLocal8Bit().data();

    server_address = new SocketAddress(ip.c_str(), port);
    if (tcp_socket_->connect(*server_address) != 0) {
        delete server_address;
        server_address = nullptr;
        tcp_socket_->close();
        delete tcp_socket_;
        tcp_socket_ = nullptr;
        return;
    }

    user_login(user_name);
    work_thread_handle = (HANDLE)_beginthreadex(NULL, 0, work_thread, this, 0, NULL);
}

void ClientCore::user_login(std::string name)
{
    ChatLogin chat_login { 0 };
    strncpy(chat_login.name, name.c_str(), name.length());
    ProtocolHead head { 1, (uint16_t)ChatCommand::CHAT_USER_LOGIN, sizeof(ChatLogin) };

    send_pack((const char *)&head, sizeof(ProtocolHead));
    send_pack((const char *)&chat_login, sizeof(ChatLogin));
}

void ClientCore::user_logout(std::string name)
{
    QString user_name = QString::fromLocal8Bit(name.c_str());
    emit remove_item(user_name);
}

void ClientCore::user_login_ok(const char *buf)
{
    ChatLogin *chat_login = (ChatLogin *)buf;
    std::string user_name = chat_login->name;
    QString name = QString::fromLocal8Bit(user_name.c_str());
    emit append_item(name);
}

void ClientCore::myself_login_ok(const char *buf)
{
    ChatLogin *chat_login = (ChatLogin *)buf;
    std::string user_name = chat_login->name;
    QString name = QString::fromLocal8Bit(user_name.c_str());
    this->user_name = name;
    emit myself_ok(name);
}

void ClientCore::heart(ProtocolHead &head)
{
    send_pack((const char *)&head, sizeof(ProtocolHead));
}

void ClientCore::deal_say_all(ChatSayALL *chat_say_all)
{
    QString source_name = QString::fromLocal8Bit(chat_say_all->source_name);
    QString message = QString::fromLocal8Bit(chat_say_all->buf);

    QString plain_text = source_name + " Say: \n" + message + "\n\n";
    emit append_msg(plain_text);
}

void ClientCore::deal_say_to(ChatSayTo *chat_say_to)
{
    QString source_name = QString::fromLocal8Bit(chat_say_to->source_name);
    QString message = QString::fromLocal8Bit(chat_say_to->buf);

    QString plain_text = source_name + " Say to you: \n" + message + "\n\n";
    emit append_msg(plain_text);
}

void ClientCore::deal_ctrl_screen_request(CTRL_ScreenRequest *ctrl_screen_request)
{
    t_name = ctrl_screen_request->source_name;
    screen_thread_handle = (HANDLE)_beginthreadex(NULL, 0, screen_thread, this, 0, NULL);
}

void ClientCore::deal_ctrl_screen_data(CTRL_ScreenData *ctrl_screen_data)
{
    unsigned long original_size = ctrl_screen_data->original_size;
    unsigned long current_size = ctrl_screen_data->current_size;
    int width = ctrl_screen_data->width;
    int height = ctrl_screen_data->height;

    char *buf = new char[original_size];
    if (Z_OK == uncompress((unsigned char *)buf, &original_size, (unsigned char *)ctrl_screen_data->data, current_size)) {
        QPixmap pixmap;
        if (pixmap.loadFromData((uchar *)buf, original_size, "JPG")) {
            QSize scaled_size(width, height);
            emit ctrl_screen(pixmap.scaled(scaled_size, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        }
    }
    delete[] buf;
}

int ClientCore::recv_pack(char *buf, int length)
{
    int current_bytes = 0;
    int total = length;

    while (current_bytes < total) {
        int recv_bytes = tcp_socket_->recv(buf + current_bytes, total - current_bytes);
        if (recv_bytes < 0)
            break;
        current_bytes += recv_bytes;
    }
    return current_bytes;
}

int ClientCore::send_pack(const char *buf, int length)
{
    return tcp_socket_->send(buf, length);
}

bool ClientCore::parse_command(ProtocolHead &head, char *buf)
{
    switch (head.command) {
    case ChatCommand::CHAT_USER_LOGIN:
        user_login_ok(buf);
        break;
    case ChatCommand::CHAT_LOGIN_OK:
        myself_login_ok(buf);
        break;
    case ChatCommand::CHAT_LOGIN_ERROR:
        emit myself_error();
        return false;
    case ChatCommand::CHAT_USER_LOGOUT:
        user_logout(buf);
        break;
    case ChatCommand::CHAT_HEART:
        heart(head);
        break;
    case ChatCommand::CHAT_SAY_ALL:
        deal_say_all((ChatSayALL *)buf);
        break;
    case ChatCommand::CHAT_SAY_TO:
        deal_say_to((ChatSayTo *)buf);
        break;
    case ChatCommand::CONTROL_SCREEN_REQUEST:
        deal_ctrl_screen_request((CTRL_ScreenRequest *)buf);
        break;
    case ChatCommand::CONTROL_SCREEN_DATA:
        deal_ctrl_screen_data((CTRL_ScreenData *)buf);
        break;
    case ChatCommand::CONTROL_SCREEN_CLOSE:
        deal_ctrl_screen_close();
        break;
    case ChatCommand::CONTROL_MOUSE_CLICK:
        deal_ctrl_mouse_click((CTRL_Mouse *)buf);
        break;
    case ChatCommand::CONTROL_MOUSE_UPLIFT:
        deal_ctrl_mouse_uplift((CTRL_Mouse *)buf);
        break;
    case ChatCommand::CONTROL_MOUSE_DOUBLE_CLICK:
        deal_ctrl_mouse_double_click((CTRL_Mouse *)buf);
        break;
    case ChatCommand::CONTROL_KEYBOARD_DOWN:
        deal_ctrl_mouse_keyboard_down((CTRL_Keyboard *)buf);
        //break;
    case ChatCommand::CONTROL_KEYBOARD_UP:
        deal_ctrl_mouse_keyboard_up((CTRL_Keyboard *)buf);
        break;
    case ChatCommand::CONTROL_CMD_REQUEST:
        deal_ctrl_cmd_request((CTRL_CmdRequest *)buf);
        break;
    case ChatCommand::CONTROL_CMD_DATA:
        deal_ctrl_cmd_data((CTRL_CmdData *)buf);
        break;
    case ChatCommand::CONTROL_CMD_EXEC:
        deal_ctrl_cmd_exec((CTRL_CmdExec *)buf);
        break;
    case ChatCommand::CONTROL_DRIVER_REQUEST:
        deal_ctrl_driver_requset((CTRL_DriverRequest *)buf);
        break;
    case ChatCommand::CONTROL_DRIVER_DATA:
        deal_ctrl_driver_data((CTRL_DriverData *)buf);
        break;
    case ChatCommand::CONTROL_FILES_REQUEST:
        deal_ctrl_files_request((CTRL_FilesRequest *)buf);
        break;
    case ChatCommand::CONTROL_FILES_DATA:
        deal_ctrl_files_data((CTRL_FilesData *)buf);
        break;
    case ChatCommand::CONTROL_DOWNLOAD_FILE_REQUEST:
        deal_ctrl_download_file_request((CTRL_DownloadFileRequest *)buf);
        break;
    case ChatCommand::CONTROL_DOWNLOAD_FILE_DATA:
        deal_ctrl_download_file_data((CTRL_DownloadFileData *)buf);
        break;
    case ChatCommand::CONTROL_DOWNLOAD_FILE_COMPLETE:
        deal_ctrl_download_file_complete((CTRL_DownloadFileComplete *)buf);
        break;
    case ChatCommand::CONTROL_DOWNLOAD_FILE_CANCEL:
        deal_ctrl_download_file_cancel((CTRL_DownloadFileComplete *)buf);
        break;
    case ChatCommand::CONTROL_UPLOAD_FILE_REQUEST:
        deal_ctrl_upload_file_request((CTRL_UploadFileRequest *)buf);
        break;
    case ChatCommand::CONTROL_UPLOAD_FILE_DATA:
        deal_ctrl_upload_file_data((CTRL_UploadFileData *)buf);
        break;
    case ChatCommand::CONTROL_UPLOAD_FILE_COMPLETE:
        deal_ctrl_upload_file_complete((CTRL_UploadFileComplete *)buf);
        break;
    default:
        break;
    }
    return true;
}

void ClientCore::deal_ctrl_screen_close()
{
    screen_is_run = false;
    //t_name.clear();
    /*::TerminateThread(screen_thread_handle, 0);
    screen_thread_handle = nullptr;*/
}

void ClientCore::deal_ctrl_mouse_click(CTRL_Mouse *ctrl_mouse_click)
{
    int x = ctrl_mouse_click->x;
    int y = ctrl_mouse_click->y;
    ::SetCursorPos(x, y);
    if (ctrl_mouse_click->type == 1) {
        ::mouse_event(MOUSEEVENTF_LEFTDOWN, x, y, 0, 0);
    } else if (ctrl_mouse_click->type == 2) {
        ::mouse_event(MOUSEEVENTF_RIGHTDOWN, x, y, 0, 0);
    }
}

void ClientCore::deal_ctrl_mouse_uplift(CTRL_Mouse *ctrl_mouse_uplift)
{
    int x = ctrl_mouse_uplift->x;
    int y = ctrl_mouse_uplift->y;
    ::SetCursorPos(x, y);
    if (ctrl_mouse_uplift->type == 1) {
        ::mouse_event(MOUSEEVENTF_LEFTUP, x, y, 0, 0);
    } else if (ctrl_mouse_uplift->type == 2) {
        ::mouse_event(MOUSEEVENTF_RIGHTUP, x, y, 0, 0);
    }
}

void ClientCore::deal_ctrl_mouse_double_click(CTRL_Mouse *ctrl_mouse_double_click)
{
    int x = ctrl_mouse_double_click->x;
    int y = ctrl_mouse_double_click->y;
    ::SetCursorPos(x, y);
    if (ctrl_mouse_double_click->type == 1) {
        ::mouse_event(MOUSEEVENTF_LEFTDOWN | MOUSEEVENTF_LEFTUP, x, y, 0, 0);
    } else if (ctrl_mouse_double_click->type == 2) {
        ::mouse_event(MOUSEEVENTF_RIGHTDOWN | MOUSEEVENTF_RIGHTUP, x, y, 0, 0);
    }
}

void ClientCore::deal_ctrl_mouse_keyboard_down(CTRL_Keyboard *ctrl_keyboard)
{
    ::keybd_event(ctrl_keyboard->vkey, 0, /*KEYEVENTF_EXTENDEDKEY*/0, 0);
}

void ClientCore::deal_ctrl_mouse_keyboard_up(CTRL_Keyboard *ctrl_keyboard)
{
    ::keybd_event(ctrl_keyboard->vkey, 0, KEYEVENTF_KEYUP, 0);
}

void ClientCore::deal_ctrl_cmd_request(CTRL_CmdRequest *ctrl_cmd_request)
{
    t_name = ctrl_cmd_request->source_name;
    cmd_thread_handle = (HANDLE)_beginthreadex(NULL, 0, cmd_thread, this, 0, NULL);
}

void ClientCore::deal_ctrl_cmd_data(CTRL_CmdData *ctrl_cmd_data)
{
    std::string data = ctrl_cmd_data->data;
    QString text = QString::fromLocal8Bit(data.c_str());
    emit ctrl_cmd(text);
}

void ClientCore::deal_ctrl_cmd_exec(CTRL_CmdExec *ctrl_cmd_exec)
{
    std::string exec = ctrl_cmd_exec->data;
    cs_pipe.lock();
    DWORD dwWritenSize = 0;
    ::WriteFile(h_parent_write, exec.c_str(), exec.length(), &dwWritenSize, NULL);
    cs_pipe.unlock();
    if (exec == "exit\n") {
        cmd_is_run = false;
        t_name.clear();
    }
}

void ClientCore::deal_ctrl_driver_requset(CTRL_DriverRequest *ctrl_driver_request)
{
    t_name = ctrl_driver_request->source_name;
    ::CloseHandle((HANDLE)_beginthreadex(NULL, 0, get_driver_info_thread, this, 0, NULL));
}

void ClientCore::deal_ctrl_driver_data(CTRL_DriverData *ctrl_driver_data)
{
    QString root_path = QString::fromLocal8Bit(ctrl_driver_data->path);
    root_path.replace("/", "\\");
    emit append_driver_info(root_path);
}

void ClientCore::deal_ctrl_files_request(CTRL_FilesRequest *ctrl_files_request)
{
    QString path = QString::fromLocal8Bit(ctrl_files_request->path);
    QDir dir(path);
    if(!dir.exists()) {
        return;
    }

    dir.setFilter(QDir::Dirs | QDir::Files);
    dir.setSorting(QDir::DirsFirst);
    QFileInfoList files_list = dir.entryInfoList();

    ProtocolHead head { 1, (uint16_t)ChatCommand::CONTROL_FILES_DATA, sizeof(CTRL_FilesData) };

    for(auto &item : files_list) {
        if(item.fileName() == "." || item.fileName() == "..") {     // .和..跳过
            continue;
        }

        CTRL_FilesData ctrl_files_data { 0 };
        strncpy(ctrl_files_data.target_name, t_name.c_str(), t_name.length());

        if(item.isDir()) {      // 是目录
            ctrl_files_data.type = 1;
        } else {    // 是文件
            ctrl_files_data.type = 2;
        }
        // 填文件名
        std::string file_name = item.fileName().toLocal8Bit().data();
        strncpy(ctrl_files_data.file_name, file_name.c_str(), file_name.length());
        ctrl_files_data.file_size = item.size();
        std::string modify_time = item.lastModified().toString("yyyy.MM.dd hh.mm.ss").toLocal8Bit().data();
        strncpy(ctrl_files_data.file_modify_time, modify_time.c_str(), modify_time.length());

        send_pack((const char *)&head, sizeof(ProtocolHead));
        send_pack((const char *)&ctrl_files_data, head.len);
    }
}

void ClientCore::deal_ctrl_files_data(CTRL_FilesData *ctrl_files_data)
{
    QString file_name = QString::fromLocal8Bit(ctrl_files_data->file_name);
    if(ctrl_files_data->type == 1) {    // 目录
        emit append_dir_info(current_expanded_index, file_name);
    } else if(ctrl_files_data->type == 2) { // 文件
        // 文件大小转换位KB
        double num = ctrl_files_data->file_size / 1024.0;
        QString file_size = QString("%1 KB").arg(num);
        QString modify_time = QString::fromLocal8Bit(ctrl_files_data->file_modify_time);
        QStringList list;
        list << file_name << modify_time << file_size;
        emit append_file_info(list);
    }
}

void ClientCore::deal_ctrl_download_file_request(CTRL_DownloadFileRequest *ctrl_download_file_request)
{
    t_name = ctrl_download_file_request->source_name;
    need_download_file = ctrl_download_file_request->path;
    ::CloseHandle((HANDLE)_beginthreadex(NULL, 0, download_file, this, 0, NULL));
}

void ClientCore::deal_ctrl_download_file_data(CTRL_DownloadFileData *ctrl_download_file_data)
{
    // 打开目标文件
    /*if(handle_dst_file == INVALID_HANDLE_VALUE) {
        handle_dst_file = ::CreateFileA(save_download_file.c_str(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    } else {
        handle_dst_file = ::CreateFileA(save_download_file.c_str(), GENERIC_WRITE, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    }*/
    
    //::SetFilePointer(handle_dst_file, NULL, NULL, FILE_END);
    // 写文件
    DWORD writed_size = 0;
    ::WriteFile(handle_dst_file, ctrl_download_file_data->data, ctrl_download_file_data->current_size, &writed_size, NULL);
    total_download_size += writed_size;
    progress_value = (int)(((double)total_download_size / ctrl_download_file_data->file_size) * 100);
}

void ClientCore::deal_ctrl_download_file_complete(CTRL_DownloadFileComplete *ctrl_download_file_complete)
{
    ::CloseHandle(handle_dst_file);
    handle_dst_file = INVALID_HANDLE_VALUE;
    total_download_size = 0;
}

void ClientCore::deal_ctrl_download_file_cancel(CTRL_DownloadFileComplete *ctrl_download_file_complete)
{
    download_file_is_run = false;
}

void ClientCore::deal_ctrl_upload_file_request(CTRL_UploadFileRequest *ctrl_upload_file_request)
{
    handle_dst_file = ::CreateFileA(ctrl_upload_file_request->path, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
}

void ClientCore::deal_ctrl_upload_file_data(CTRL_UploadFileData *ctrl_upload_file_data)
{
    DWORD writed_size = 0;
    ::WriteFile(handle_dst_file, ctrl_upload_file_data->data, ctrl_upload_file_data->current_size, &writed_size, NULL);
}

void ClientCore::deal_ctrl_upload_file_complete(CTRL_UploadFileComplete *ctrl_upload_file_complete)
{
    ::CloseHandle(handle_dst_file);
    handle_dst_file = INVALID_HANDLE_VALUE;
}

unsigned ClientCore::work_thread(void *arg)
{
    ClientCore *client_core = (ClientCore *)arg;
    while (true) {
        ProtocolHead protocol_head { 0 };
        char *buf = nullptr;

        // 接收失败断线程
        if (client_core->recv_pack((char *)&protocol_head, sizeof(ProtocolHead)) <= 0) {
            break;
        }

        if (protocol_head.version == 0)
            continue;
        if (protocol_head.len > 0) {
            buf = new char[protocol_head.len];
            if (client_core->recv_pack(buf, protocol_head.len) <= 0) {
                delete[] buf;
                break;
            }
        }

        bool ret = client_core->parse_command(protocol_head, buf);

        if (buf)
            delete[] buf;

        if (!ret)
            break;
    }

    return 0;
}

unsigned ClientCore::screen_thread(void *arg)
{
    ClientCore *client_core = (ClientCore *)arg;
    client_core->screen_is_run = true;

    while (client_core->screen_is_run) {
        int width = ::GetSystemMetrics(SM_CXSCREEN);
        int height = ::GetSystemMetrics(SM_CYSCREEN);

        // 截屏
        QScreen *screen = QGuiApplication::primaryScreen();
        QSize scaled_size(width /*/ 2*/, height /*/ 2*/);
        QPixmap pixmap = screen->grabWindow(0).scaled(scaled_size, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        QByteArray screen_img_data;
        QBuffer buffer(&screen_img_data);
        buffer.open(QIODevice::WriteOnly);
        bool is_ok = pixmap.save(&buffer, "JPG", 60);
        if (is_ok) {
            //qDebug() << screen_img_data.size();
            unsigned long buf_size = screen_img_data.size();
            unsigned char *buf = new unsigned char[buf_size];

            if (Z_OK == compress(buf, &buf_size, (unsigned char *)screen_img_data.data(), screen_img_data.size())) {
                // 构造包
                ProtocolHead head { 1, (uint16_t)ChatCommand::CONTROL_SCREEN_DATA, sizeof(CTRL_ScreenData) + buf_size };
                CTRL_ScreenData *ctrl_screen_data = (CTRL_ScreenData *)calloc(1, head.len);
                strncpy(ctrl_screen_data->target_name, client_core->t_name.c_str(), client_core->t_name.length());
                ctrl_screen_data->original_size = (unsigned long)screen_img_data.size();
                ctrl_screen_data->width = width;
                ctrl_screen_data->height = height;

                ctrl_screen_data->current_size = buf_size;
                memcpy(ctrl_screen_data->data, buf, buf_size);

                // 发包
                client_core->send_pack((const char *)&head, sizeof(ProtocolHead));
                client_core->send_pack((const char *)ctrl_screen_data, head.len);


                delete[] buf;
                free(ctrl_screen_data);
            }
        }
        ::Sleep(300);
    }
    return 0;
}

unsigned ClientCore::cmd_thread(void *arg)
{
    ClientCore *client_core = (ClientCore *)arg;

    // 创建管道
    if (!create_pipe(client_core->h_child_read, client_core->h_parent_write)) {
        return 0;
    }
    if (!create_pipe(client_core->h_parent_read, client_core->h_child_write)) {
        return 0;
    }

    // 创建cmd
    if (!create_cmd(client_core->h_child_read, client_core->h_child_write)) {
        return 0;
    }


    DWORD total_bytes = 0;
    DWORD read_bytes = 0;
    char buf[4096] { 0 };
    client_core->cmd_is_run = true;
    while (client_core->cmd_is_run) {
        client_core->cs_pipe.lock();
        if (::PeekNamedPipe(client_core->h_parent_read, buf, sizeof(buf), &read_bytes, &total_bytes, NULL)) {
            memset(buf, 0, sizeof(buf));
            if (total_bytes) {
                if (!::ReadFile(client_core->h_parent_read, buf, sizeof(buf), &read_bytes, NULL)) {
                    client_core->cs_pipe.unlock();
                    return 0;
                }

                // 构造包
                ProtocolHead head { 1, (uint16_t)ChatCommand::CONTROL_CMD_DATA, sizeof(CTRL_CmdData) + read_bytes + 1 };
                CTRL_CmdData *ctrl_cmd_data = (CTRL_CmdData *)calloc(1, head.len);
                strncpy(ctrl_cmd_data->target_name, client_core->t_name.c_str(), client_core->t_name.length());
                memcpy(ctrl_cmd_data->data, buf, read_bytes);

                // 发包
                client_core->send_pack((const char *)&head, sizeof(ProtocolHead));
                client_core->send_pack((const char *)ctrl_cmd_data, head.len);


                free(ctrl_cmd_data);
            }
            client_core->cs_pipe.unlock();
        } else {
            client_core->cs_pipe.unlock();
            Sleep(100);
        }
    }

    return 0;
}

bool ClientCore::create_pipe(HANDLE&h_read_pipe, HANDLE&h_write_pipe)
{
    SECURITY_ATTRIBUTES sa { 0 };
    sa.nLength = sizeof(SECURITY_ATTRIBUTES);
    sa.bInheritHandle = TRUE;
    SYSTEM_INFO si;
    ::GetSystemInfo(&si);
    if (!::CreatePipe(&h_read_pipe, &h_write_pipe, &sa, si.dwAllocationGranularity)) {
        return false;
    }
    return true;
}

bool ClientCore::create_cmd(HANDLE &input, HANDLE &output)
{
    TCHAR proc[] = TEXT("cmd.exe");
    STARTUPINFO si { 0 };
    PROCESS_INFORMATION pi { 0 };
    si.cb = sizeof(STARTUPINFOA);
    si.hStdInput = input;
    si.hStdOutput = si.hStdError = output;
    si.dwFlags = STARTF_USESTDHANDLES;
    if (!::CreateProcessW(NULL, proc, NULL, NULL, TRUE, CREATE_NO_WINDOW/*CREATE_NEW_CONSOLE*/, NULL, NULL, &si, &pi)) {
        return false;
    }
    return true;
}

unsigned ClientCore::get_driver_info_thread(void *arg)
{
    ClientCore *client_core = (ClientCore *)arg;
    auto info_list = QStorageInfo::mountedVolumes();

    ProtocolHead head { 1, (uint16_t)ChatCommand::CONTROL_DRIVER_DATA, sizeof(CTRL_DriverData) };

    for (auto &item : info_list) {
        qDebug() << item.rootPath();
        // 构造包
        std::string path = item.rootPath().toLocal8Bit().data();
        CTRL_DriverData ctrl_driver_data { 0 };
        strncpy(ctrl_driver_data.target_name, client_core->t_name.c_str(), client_core->t_name.length());
        strncpy(ctrl_driver_data.path, path.c_str(), path.length());

        // 发包
        client_core->send_pack((const char *)&head, sizeof(ProtocolHead));
        client_core->send_pack((const char *)&ctrl_driver_data, head.len);
    }

    return 0;
}

unsigned ClientCore::download_file(void *arg)
{
    ClientCore *client_core = (ClientCore *)arg;
    client_core->download_file_is_run = true;

    // 打开文件
    auto handle_src_file = ::CreateFileA(client_core->need_download_file.c_str(), GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    // 获取源文件大小
    DWORD src_file_size_hi = 0;
    DWORD src_file_size_lo = ::GetFileSize(handle_src_file, &src_file_size_hi);
    // 计算总大小
    uint64_t src_file_size = src_file_size_hi;
    src_file_size <<= 32;
    src_file_size |= src_file_size_lo;

    CTRL_DownloadFileData ctrl_download_file_data;
    ctrl_download_file_data.file_size = src_file_size;
    strncpy(ctrl_download_file_data.target_name, client_core->t_name.c_str(), client_core->t_name.length());

    
    while(client_core->download_file_is_run) {
        DWORD readed_size = 0;
        // 读文件
        ::ReadFile(handle_src_file, ctrl_download_file_data.data, sizeof(ctrl_download_file_data.data), &readed_size, NULL);
        if (readed_size <= 0) {
            break;
        }
        ctrl_download_file_data.current_size = readed_size;

        // 发包
        ProtocolHead head { 1, (uint16_t)ChatCommand::CONTROL_DOWNLOAD_FILE_DATA, sizeof(CTRL_DownloadFileData) };
        client_core->send_pack((const char *)&head, sizeof(ProtocolHead));
        client_core->send_pack((const char *)&ctrl_download_file_data, head.len);
    }

    client_core->download_file_is_run = false;
    // 完成包
    ProtocolHead head { 1, (uint16_t)ChatCommand::CONTROL_DOWNLOAD_FILE_COMPLETE, sizeof(CTRL_DownloadFileComplete) };
    CTRL_DownloadFileComplete ctrl_download_file_complete;
    strncpy(ctrl_download_file_complete.target_name, client_core->t_name.c_str(), client_core->t_name.length());
    client_core->send_pack((const char *)&head, sizeof(ProtocolHead));
    client_core->send_pack((const char *)&ctrl_download_file_complete, head.len);
    ::CloseHandle(handle_src_file);

    return 0;
}

unsigned ClientCore::upload_file(void *arg)
{
    ClientCore *client_core = (ClientCore *)arg;
    client_core->upload_file_is_run = true;

    // 打开文件
    auto handle_src_file = ::CreateFileA(client_core->need_upload_file.c_str(), GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    // 获取源文件大小
    DWORD src_file_size_hi = 0;
    DWORD src_file_size_lo = ::GetFileSize(handle_src_file, &src_file_size_hi);
    // 计算总大小
    uint64_t src_file_size = src_file_size_hi;
    src_file_size <<= 32;
    src_file_size |= src_file_size_lo;

    uint64_t current_upload_size = 0;

    CTRL_UploadFileData ctrl_upload_file_data;
    ctrl_upload_file_data.file_size = src_file_size;
    strncpy(ctrl_upload_file_data.target_name, client_core->upload_target.c_str(), client_core->upload_target.length());


    while (client_core->upload_file_is_run) {
        DWORD readed_size = 0;
        // 读文件
        ::ReadFile(handle_src_file, ctrl_upload_file_data.data, sizeof(ctrl_upload_file_data.data), &readed_size, NULL);
        if (readed_size <= 0) {
            break;
        }
        ctrl_upload_file_data.current_size = readed_size;

        // 发包
        ProtocolHead head { 1, (uint16_t)ChatCommand::CONTROL_UPLOAD_FILE_DATA, sizeof(CTRL_UploadFileData) };
        client_core->send_pack((const char *)&head, sizeof(ProtocolHead));
        client_core->send_pack((const char *)&ctrl_upload_file_data, head.len);

        current_upload_size += readed_size;
        client_core->progress_value = (int)((double)current_upload_size / src_file_size * 100);
    }


    client_core->upload_file_is_run = false;
    // 完成包
    ProtocolHead head { 1, (uint16_t)ChatCommand::CONTROL_UPLOAD_FILE_COMPLETE, sizeof(CTRL_UploadFileComplete) };
    CTRL_UploadFileComplete ctrl_upload_file_complete;
    strncpy(ctrl_upload_file_complete.target_name, client_core->upload_target.c_str(), client_core->upload_target.length());
    client_core->send_pack((const char *)&head, sizeof(ProtocolHead));
    client_core->send_pack((const char *)&ctrl_upload_file_complete, head.len);
    ::CloseHandle(handle_src_file);

    return 0;
}
