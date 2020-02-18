#include "screendialog.h"
#include "ui_screendialog.h"
#include "mainwindow.h"
#include "../common/protocol.h"
#include <QScrollArea>
#include <QScrollBar>
#include <QMouseEvent>
#include <QDebug>
#include <QTimer>
#include <QDateTime>

ScreenDialog::ScreenDialog(QString target, QWidget *parent) :
    QDialog(parent), target_name(target),
    ui(new Ui::ScreenDialog)
{
    ui->setupUi(this);
    ui->scrollArea->setFrameShape(QFrame::NoFrame);

    if (static_cast<MainWindow *>(this->parent())->get_client_core()) {
        client_core_ = static_cast<MainWindow *>(this->parent())->get_client_core();
        source_name = static_cast<MainWindow *>(this->parent())->get_client_core()->user_name;
        // 连接信号
        connect(client_core_, static_cast<void (ClientCore::*)(QPixmap)>(&ClientCore::ctrl_screen),
            this, static_cast<void (ScreenDialog::*)(QPixmap)>(&ScreenDialog::screen_show));

        // 发远控屏幕请求
        CTRL_ScreenRequest ctrl_screen_request { 0 };
        std::string s_name = source_name.toLocal8Bit().data();
        std::string t_name = target_name.toLocal8Bit().data();
        strncpy(ctrl_screen_request.source_name, s_name.c_str(), s_name.length());
        strncpy(ctrl_screen_request.target_name, t_name.c_str(), t_name.length());
        ProtocolHead head { 1, (uint16_t)ChatCommand::CONTROL_SCREEN_REQUEST, sizeof(CTRL_ScreenRequest) };
        client_core_->send_pack((const char *)&head, sizeof(ProtocolHead));
        client_core_->send_pack((const char *)&ctrl_screen_request, head.len);
    }
}

ScreenDialog::~ScreenDialog()
{
    /*disconnect(client_core_, static_cast<void (ClientCore::*)(QPixmap)>(&ClientCore::ctrl_screen),
        this, static_cast<void (ScreenDialog::*)(QPixmap)>(&ScreenDialog::screen_show));*/

    delete ui;
}


void ScreenDialog::screen_show(QPixmap pixmap)
{
    ui->label->setPixmap(pixmap);
}

void ScreenDialog::closeEvent(QCloseEvent *event)
{
    // 构造包 有问题
    if (client_core_) {
        CTRL_ScreenClose ctrl_screen_close { 0 };
        std::string t_name = target_name.toLocal8Bit().data();
        strncpy(ctrl_screen_close.target_name, t_name.c_str(), t_name.length());
        ProtocolHead head { 1, (uint16_t)ChatCommand::CONTROL_SCREEN_CLOSE, sizeof(CTRL_ScreenClose) };
        client_core_->send_pack((const char *)&head, sizeof(ProtocolHead));
        client_core_->send_pack((const char *)&ctrl_screen_close, sizeof(CTRL_ScreenClose));
    }
}

void ScreenDialog::mousePressEvent(QMouseEvent *event)
{
    //qDebug() << event->pos();
    int dx = ui->scrollArea->horizontalScrollBar()->value();
    int dy = ui->scrollArea->verticalScrollBar()->value();
    int mouse_x = event->x() + dx;
    int mouse_y = event->y() + dy;

    // 构造包
    CTRL_Mouse ctrl_mouse_click { 0 };
    std::string t_name = target_name.toLocal8Bit().data();
    strncpy(ctrl_mouse_click.target_name, t_name.c_str(), t_name.length());
    ctrl_mouse_click.x = mouse_x;
    ctrl_mouse_click.y = mouse_y;
    if (event->button() == Qt::LeftButton) {
        ctrl_mouse_click.type = 1;
    } else if (event->button() == Qt::RightButton) {
        ctrl_mouse_click.type = 2;
    }

    ProtocolHead head { 1, (uint16_t)ChatCommand::CONTROL_MOUSE_CLICK, sizeof(CTRL_Mouse) };
    client_core_->send_pack((const char *)&head, sizeof(ProtocolHead));
    client_core_->send_pack((const char *)&ctrl_mouse_click, head.len);
}

void ScreenDialog::mouseReleaseEvent(QMouseEvent *event)
{
    int dx = ui->scrollArea->horizontalScrollBar()->value();
    int dy = ui->scrollArea->verticalScrollBar()->value();
    int mouse_x = event->x() + dx;
    int mouse_y = event->y() + dy;


    CTRL_Mouse ctrl_mouse_uplift { 0 };
    std::string t_name = target_name.toLocal8Bit().data();
    strncpy(ctrl_mouse_uplift.target_name, t_name.c_str(), t_name.length());
    ctrl_mouse_uplift.x = mouse_x;
    ctrl_mouse_uplift.y = mouse_y;
    if (event->button() == Qt::LeftButton) {
        ctrl_mouse_uplift.type = 1;
    } else if (event->button() == Qt::RightButton) {
        ctrl_mouse_uplift.type = 2;
    }

    ProtocolHead head { 1, (uint16_t)ChatCommand::CONTROL_MOUSE_UPLIFT, sizeof(CTRL_Mouse) };
    client_core_->send_pack((const char *)&head, sizeof(ProtocolHead));
    client_core_->send_pack((const char *)&ctrl_mouse_uplift, head.len);
}

void ScreenDialog::mouseDoubleClickEvent(QMouseEvent *event)
{
    int dx = ui->scrollArea->horizontalScrollBar()->value();
    int dy = ui->scrollArea->verticalScrollBar()->value();
    int mouse_x = event->x() + dx;
    int mouse_y = event->y() + dy;

    // 构造包
    CTRL_Mouse ctrl_mouse_double_click { 0 };
    std::string t_name = target_name.toLocal8Bit().data();
    strncpy(ctrl_mouse_double_click.target_name, t_name.c_str(), t_name.length());
    ctrl_mouse_double_click.x = mouse_x;
    ctrl_mouse_double_click.y = mouse_y;
    if (event->button() == Qt::LeftButton) {
        ctrl_mouse_double_click.type = 1;
    } else if (event->button() == Qt::RightButton) {
        ctrl_mouse_double_click.type = 2;
    }

    ProtocolHead head { 1, (uint16_t)ChatCommand::CONTROL_MOUSE_DOUBLE_CLICK, sizeof(CTRL_Mouse) };
    client_core_->send_pack((const char *)&head, sizeof(ProtocolHead));
    client_core_->send_pack((const char *)&ctrl_mouse_double_click, head.len);
}

void ScreenDialog::keyPressEvent(QKeyEvent *event)
{
    CTRL_Keyboard ctrl_keyboard { 0 };
    std::string t_name = target_name.toLocal8Bit().data();
    strncpy(ctrl_keyboard.target_name, t_name.c_str(), t_name.length());
    ctrl_keyboard.vkey = event->nativeVirtualKey();
    ProtocolHead head { 1, (uint16_t)ChatCommand::CONTROL_KEYBOARD_DOWN, sizeof(CTRL_Keyboard) };

    client_core_->send_pack((const char *)&head, sizeof(ProtocolHead));
    client_core_->send_pack((const char *)&ctrl_keyboard, head.len);

    if (ctrl_keyboard.vkey == VK_ESCAPE) {
        // 保存屏幕图片
        QDateTime time = QDateTime::currentDateTime();
        QString str_time = time.toString("yyyy-MM-dd hh:mm:ss");
        auto pixmap = ui->label->pixmap();
        pixmap->save(str_time + ".jpg", "JPG", 100);
    }
}

void ScreenDialog::keyReleaseEvent(QKeyEvent *event)
{
    /*CTRL_Keyboard ctrl_keyboard { 0 };
    std::string t_name = target_name.toLocal8Bit().data();
    strncpy(ctrl_keyboard.target_name, t_name.c_str(), t_name.length());
    ctrl_keyboard.vkey = event->nativeVirtualKey();
    ProtocolHead head { 1, (uint16_t)ChatCommand::CONTROL_KEYBOARD_UP, sizeof(CTRL_Keyboard) };

    client_core_->send_pack((const char *)&head, sizeof(ProtocolHead));
    client_core_->send_pack((const char *)&ctrl_keyboard, head.len);*/
}