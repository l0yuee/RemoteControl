#ifndef CLIENTCORE_H
#define CLIENTCORE_H

#include <QObject>
#include <QStandardItemModel>
#include "../common/TCPSocket.h"
#include "../common/protocol.h"
#include "../common/CriticalSection.h"
#include "../common/zlib/zconf.h"
#include "../common/zlib/zlib.h"

#pragma comment(lib, "../common/zlib/zdll.lib")




class ClientCore : public QObject
{
    Q_OBJECT
public:
    explicit ClientCore(QObject *parent = nullptr);
    ~ClientCore();

    inline QStandardItemModel * get_table_model() { return &table_model_; }
    inline TCPSocket * get_socket() const { return tcp_socket_; }

    void run(QString &server_ip, QString &server_port, QString &name);

    // 请求处理
    void user_login(std::string name);
    void user_logout(std::string name);
    void user_login_ok(const char *buf);
    void myself_login_ok(const char *buf);
    void heart(ProtocolHead &head);
    void deal_say_all(ChatSayALL *chat_say_all);
    void deal_say_to(ChatSayTo *chat_say_to);
    void deal_ctrl_screen_request(CTRL_ScreenRequest *ctrl_screen_request);
    void deal_ctrl_screen_data(CTRL_ScreenData *ctrl_screen_data);
    void deal_ctrl_screen_close();
    void deal_ctrl_mouse_click(CTRL_Mouse *ctrl_mouse_click);
    void deal_ctrl_mouse_uplift(CTRL_Mouse *ctrl_mouse_uplift);
    void deal_ctrl_mouse_double_click(CTRL_Mouse *ctrl_mouse_double_click);
    void deal_ctrl_mouse_keyboard_down(CTRL_Keyboard *ctrl_keyboard);
    void deal_ctrl_mouse_keyboard_up(CTRL_Keyboard *ctrl_keyboard);
    void deal_ctrl_cmd_request(CTRL_CmdRequest *ctrl_cmd_request);
    void deal_ctrl_cmd_data(CTRL_CmdData *ctrl_cmd_data);
    void deal_ctrl_cmd_exec(CTRL_CmdExec *ctrl_cmd_exec);
    void deal_ctrl_driver_requset(CTRL_DriverRequest *ctrl_driver_request);
    void deal_ctrl_driver_data(CTRL_DriverData *ctrl_driver_data);
    void deal_ctrl_files_request(CTRL_FilesRequest *ctrl_files_request);
    void deal_ctrl_files_data(CTRL_FilesData *ctrl_files_data);
    void deal_ctrl_download_file_request(CTRL_DownloadFileRequest *ctrl_download_file_request);
    void deal_ctrl_download_file_data(CTRL_DownloadFileData *ctrl_download_file_data);
    void deal_ctrl_download_file_complete(CTRL_DownloadFileComplete *ctrl_download_file_complete);
    void deal_ctrl_download_file_cancel(CTRL_DownloadFileComplete *ctrl_download_file_complete);
    void deal_ctrl_upload_file_request(CTRL_UploadFileRequest *ctrl_upload_file_request);
    void deal_ctrl_upload_file_data(CTRL_UploadFileData *ctrl_upload_file_data);
    void deal_ctrl_upload_file_complete(CTRL_UploadFileComplete *ctrl_upload_file_complete);
    

    // 收包
    int recv_pack(char *buf, int length);

    // 发包
    int send_pack(const char *buf, int length);

    // 解析
    bool parse_command(ProtocolHead &head, char *buf);



    QString user_name;
    std::string t_name;     // 远控来源机器
    CriticalSection cs_pipe;    // 管道锁
    QModelIndex current_expanded_index;
    HANDLE handle_dst_file = INVALID_HANDLE_VALUE;

    std::string need_download_file;
    std::string need_upload_file;
    uint64_t total_download_size = 0;
    int progress_value = 0;     // 进度

    std::string upload_target;
    bool upload_file_is_run = false;
    static unsigned int __stdcall upload_file(void *arg);

signals:
    void myself_ok(QString name);
    void myself_error();
    void append_item(QString name);
    void remove_item(QString name);
    void append_msg(QString plain_text);
    

    // 远控
    void ctrl_screen(QPixmap pixmap);
    void ctrl_cmd(QString text);
    void append_driver_info(QString path);
    void append_dir_info(QModelIndex index, QString dir_name);
    void append_file_info(QStringList strings);
    

public slots:

private:
    TCPSocket *tcp_socket_ = nullptr;
    SocketAddress *server_address = nullptr;
    QStandardItemModel table_model_;

    bool screen_is_run = false;
    bool cmd_is_run = false;
    bool download_file_is_run = false;
    

protected:
    HANDLE work_thread_handle = nullptr;
    static unsigned int __stdcall work_thread(void *arg);
    HANDLE screen_thread_handle = nullptr;
    static unsigned int __stdcall screen_thread(void *arg);
    HANDLE cmd_thread_handle = nullptr;
    static unsigned int __stdcall cmd_thread(void *arg);
    static bool create_pipe(HANDLE &h_read_pipe, HANDLE &h_write_pipe);
    static bool create_cmd(HANDLE &input, HANDLE &output);
    // 管道句柄
    HANDLE h_parent_read = nullptr;
    HANDLE h_parent_write = nullptr;
    HANDLE h_child_read = nullptr;
    HANDLE h_child_write = nullptr;


    static unsigned int __stdcall get_driver_info_thread(void *arg);
    static unsigned int __stdcall download_file(void *arg);
};

#endif // CLIENTCORE_H
