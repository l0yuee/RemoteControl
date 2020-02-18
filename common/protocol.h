#ifndef _PROTOCOL_H_
#define _PROTOCOL_H_

#include <WinSock2.h>
#include <stdint.h>

// 命令种类
enum struct ChatCommand {
    CHAT_USER_LOGIN = 1,            // 登录
    CHAT_USER_LOGOUT,               // 登出
    CHAT_LOGIN_OK,                  // 登录成功
    CHAT_LOGIN_ERROR,               // 登录失败
    CHAT_SAY_ALL,                   // 群聊
    CHAT_SAY_TO,                    // 私聊
    CHAT_HEART,                     // 心跳
    CONTROL_SCREEN_REQUEST,         // 远控屏幕请求
    CONTROL_SCREEN_DATA,            // 远控屏幕数据
    CONTROL_SCREEN_CLOSE,           // 远控屏幕关闭
    CONTROL_MOUSE_CLICK,            // 远控鼠标点击
    CONTROL_MOUSE_UPLIFT,           // 远控鼠标抬起
    CONTROL_MOUSE_DOUBLE_CLICK,     // 远控鼠标双击
    CONTROL_KEYBOARD_DOWN,          // 远控键盘按下
    CONTROL_KEYBOARD_UP,            // 远控键盘抬起
    CONTROL_CMD_REQUEST,            // 远控cmd请求
    CONTROL_CMD_DATA,               // 远控cmd数据
    CONTROL_CMD_EXEC,               // 远控cmd执行
    CONTROL_CMD_CLOSE,              // 远控cmd关闭
    CONTROL_DRIVER_REQUEST,         // 远控驱动器请求
    CONTROL_DRIVER_DATA,            // 远控驱动器数据
    CONTROL_FILES_REQUEST,          // 远控文件浏览请求
    CONTROL_FILES_DATA,             // 远控文件数据
    CONTROL_DOWNLOAD_FILE_REQUEST,  // 远控下载文件请求
    CONTROL_DOWNLOAD_FILE_DATA,     // 远控下载文件数据
    CONTROL_DOWNLOAD_FILE_COMPLETE, // 远控下载文件完成
    CONTROL_DOWNLOAD_FILE_CANCEL,   // 远控下载文件取消
    CONTROL_UPLOAD_FILE_REQUEST,    // 远控上传文件请求
    CONTROL_UPLOAD_FILE_DATA,       // 远控上传文件数据
    CONTROL_UPLOAD_FILE_COMPLETE,   // 远控上传文件完成
    CONTROL_UPLOAD_FILE_CANCEL      // 远控上传文件取消
};

#pragma pack(push)
#pragma pack(1)
// 协议头
struct ProtocolHead {
    uint16_t version;       // 版本
    uint16_t command;       // 命令
    uint32_t len;           // 后续长度
};

// 登录包
struct ChatLogin {
    char name[32];
};

// 登出包
struct ChatLogout {
    char name[32];
};

// 群聊包
struct ChatSayALL {
    char source_name[32];
    char buf[0];
};

// 私聊包
struct ChatSayTo {
    char source_name[32];
    char target_name[32];
    char buf[0];
};

// 远控屏幕请求
struct CTRL_ScreenRequest {
    char source_name[32];
    char target_name[32];
};

// 远控屏幕数据
struct CTRL_ScreenData {
    char target_name[32];
    uint32_t original_size;
    uint32_t current_size;
    uint32_t width;
    uint32_t height;
    char data[0];
};

// 远控屏幕关闭
struct CTRL_ScreenClose {
    char target_name[32];
};

// 远控鼠标
struct CTRL_Mouse {
    char target_name[32];
    uint32_t x;
    uint32_t y;
    uint32_t type;      // 左键1， 右键2
};

// 远控键盘
struct CTRL_Keyboard {
    char target_name[32];
    uint32_t vkey;
};

// 远控cmd请求
struct CTRL_CmdRequest {
    char source_name[32];
    char target_name[32];
};

// 远控cmd数据
struct CTRL_CmdData {
    char target_name[32];
    char data[0];
};

// 远控cmd执行
struct CTRL_CmdExec {
    char target_name[32];
    char data[0];
};

// 远控驱动器请求
struct CTRL_DriverRequest {
    char source_name[32];
    char target_name[32];
};

// 远控驱动器数据
struct CTRL_DriverData {
    char target_name[32];
    char path[256];
};

// 远控文件浏览请求
struct CTRL_FilesRequest {
    char source_name[32];
    char target_name[32];
    char path[256];
};

// 远控文件数据
struct CTRL_FilesData {
    char target_name[32];
    uint8_t type;   // 1目录  2文件
    char file_name[256];
    uint64_t file_size;
    char file_modify_time[20];
};

// 远控下载文件请求
struct CTRL_DownloadFileRequest {
    char source_name[32];
    char target_name[32];
    char path[256];
};

// 远控下载文件数据
struct CTRL_DownloadFileData {
    char target_name[32];
    uint64_t file_size;
    uint64_t current_size;
    char data[4096];
};

// 远控下载文件完成
struct CTRL_DownloadFileComplete {
    char target_name[32];
};

// 远控下载文件取消
struct CTRL_DownloadFileCancel {
    char target_name[32];
};

// 远控上传文件请求
struct CTRL_UploadFileRequest {
    char target_name[32];
    char path[256];
};

// 远控上传文件数据
struct CTRL_UploadFileData {
    char target_name[32];
    uint64_t file_size;
    uint64_t current_size;
    char data[4096];
};

// 远控上传文件完成
struct CTRL_UploadFileComplete {
    char target_name[32];
};

// 远控下载文件取消
struct CTRL_UploadFileCancel {
    char target_name[32];
};

#pragma pack(pop)

#endif