#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QDebug>
#include "screendialog.h"
#include "cmddialog.h"
#include "browsefilesdialog.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    client_core_ = new ClientCore(this);

    connect(client_core_, static_cast<void (ClientCore::*)(QString)>(&ClientCore::myself_ok),
        this, [=](QString name) {
        QString title = windowTitle() + " - " + name;
        setWindowTitle(title);
        ui_enable();
    });

    connect(client_core_, &ClientCore::myself_error, this, [=]() {
        QMessageBox msg;
        msg.setText("Login failed!");
        msg.exec();
    });

    connect(client_core_, static_cast<void (ClientCore::*)(QString)>(&ClientCore::append_msg), 
        this, [=](QString plain_text) {
        ui->textEdit_context->setPlainText(ui->textEdit_context->toPlainText() + plain_text);
    });

    ui_init();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::ui_init()
{
    QRegExp reg("\\b(?:(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\b");
    ui->lineEdit_server_ip->setValidator(new QRegExpValidator(reg, this));

    ui->tableView_users->setModel(client_core_->get_table_model());
    ui->lineEdit_user_name->setFocus();

    SAY_MODEL = MODEL_SAY_ALL;

    ui_enable(false);
}

void MainWindow::ui_enable(bool enable)
{
    ui->lineEdit_server_ip->setEnabled(!enable);
    ui->lineEdit_user_name->setEnabled(!enable);
    ui->spinBox_server_port->setEnabled(!enable);
    ui->pushButton_login->setEnabled(!enable);

    ui->textEdit_context->setEnabled(enable);
    ui->textEdit_msg->setEnabled(enable);
    ui->pushButton_send->setEnabled(enable);
    ui->tableView_users->setEnabled(enable);
    ui->groupBox->setEnabled(enable);
    ui->menubar->setEnabled(enable);
}

void MainWindow::say_all(QString message)
{
    //qDebug() << "MainWindow::say_all: " << message;
    std::string msg = message.toLocal8Bit().data();
    std::string source_name = client_core_->user_name.toLocal8Bit().data();

    // 构造数据包
    ProtocolHead head { 1, (uint16_t)ChatCommand::CHAT_SAY_ALL, sizeof(ChatSayALL) + msg.length() + 1 };
    ChatSayALL *chat_say_all = (ChatSayALL *)calloc(1, head.len);
    strncpy(chat_say_all->source_name, source_name.c_str(), source_name.length());
    memcpy(chat_say_all->buf, msg.c_str(), msg.length() + 1);
    // 发包
    client_core_->send_pack((const char *)&head, sizeof(ProtocolHead));
    client_core_->send_pack((const char *)chat_say_all, head.len);
    free(chat_say_all);

    ui->textEdit_context->setPlainText(ui->textEdit_context->toPlainText() + client_core_->user_name + " Say: \n" + message + "\n\n");
}

void MainWindow::say_to(QString message)
{
    QString name;
    if(!get_select_user_name(name)) {
        QMessageBox msgbox;
        msgbox.setText("Please select a contact!");
        msgbox.exec();
        return;
    }

    //qDebug() << "MainWindow::say_to: (" << target_name << ")" << message;
    // 构造数据包
    std::string msg = message.toLocal8Bit().data();
    std::string source_name = client_core_->user_name.toLocal8Bit().data();
    std::string target_name = name.toLocal8Bit().data();

    ProtocolHead head { 1, (uint16_t)ChatCommand::CHAT_SAY_TO, sizeof(ChatSayTo) + msg.length() + 1 };
    ChatSayTo *chat_say_to = (ChatSayTo *)calloc(1, head.len);
    strncpy(chat_say_to->source_name, source_name.c_str(), source_name.length());
    strncpy(chat_say_to->target_name, target_name.c_str(), target_name.length());
    memcpy(chat_say_to->buf, msg.c_str(), msg.length() + 1);

    //// 发包
    client_core_->send_pack((const char *)&head, sizeof(ProtocolHead));
    client_core_->send_pack((const char *)chat_say_to, head.len);
    free(chat_say_to);

    QString plain_text = QString("%1Say to %3 :\n%4\n\n").arg(ui->textEdit_context->toPlainText()).arg(name).arg(message);
    ui->textEdit_context->setPlainText(plain_text);
}

bool MainWindow::get_select_user_name(QString &name)
{
    auto index = ui->tableView_users->selectionModel()->currentIndex();
    if(!index.isValid()) {
        return false;
    }
    name = client_core_->get_table_model()->index(index.row(), 0).data().toString();
    return true;
}


void MainWindow::on_pushButton_login_clicked()
{
    QMessageBox msg;
    QString server_ip = ui->lineEdit_server_ip->text();
    QString server_port = ui->spinBox_server_port->text();
    QString name = ui->lineEdit_user_name->text();
    if(server_ip.isEmpty()) {
        msg.setText("Server IP cannot be empty.");
        msg.exec();
        return;
    }
    if (server_port.isEmpty()) {
        msg.setText("Server Port cannot be empty.");
        msg.exec();
        return;
    }
    if (name.isEmpty()) {
        msg.setText("User Name cannot be empty.");
        msg.exec();
        return;
    }

    // 客户端启动
    client_core_->run(server_ip, server_port, name);
}


void MainWindow::closeEvent(QCloseEvent *event)
{
    if(client_core_->get_socket()) {
        // 构造登出包
        ChatLogout chat_logout { 0 };
        ProtocolHead head { 1, (uint16_t)ChatCommand::CHAT_USER_LOGOUT, sizeof(ChatLogout) };
        std::string user_name = client_core_->user_name.toLocal8Bit().data();
        strncpy(chat_logout.name, user_name.c_str(), user_name.length());

        // 发协议头
        client_core_->send_pack((const char *)&head, sizeof(ProtocolHead));
        // 发数据
        client_core_->send_pack((const char *)&chat_logout, sizeof(ChatLogin));
    }
}
void MainWindow::on_radioButton_chat_us_clicked()
{
    SAY_MODEL = MODEL_SAY_ALL;
}

void MainWindow::on_radioButton_chat_to_clicked()
{
    SAY_MODEL = MODEL_SAY_TO;
}

void MainWindow::on_pushButton_send_clicked()
{
    QString message = ui->textEdit_msg->toPlainText();
    if(message.isEmpty()) {
        QMessageBox msgbox;
        msgbox.setText("Message is cannot be empty!");
        msgbox.exec();
        return;
    }

    switch(SAY_MODEL) {
    case MODEL_SAY_ALL:
        say_all(message);
        break;
    case MODEL_SAY_TO:
        say_to(message);
        break;
    }
    ui->textEdit_msg->clear();
}

void MainWindow::on_action_screen_triggered()
{
    QString name;
    if (!get_select_user_name(name)) {
        QMessageBox msgbox;
        msgbox.setText("Please select a contact!");
        msgbox.exec();
        return;
    }

    ScreenDialog *dlg = new ScreenDialog(name, this);
    /*dlg->setAttribute(Qt::WA_DeleteOnClose);
    dlg->show();*/
    dlg->setWindowTitle("Remote Contorl From: " + name);
    dlg->setAttribute(Qt::WA_DeleteOnClose);
    dlg->exec();
}

void MainWindow::on_action_cmd_triggered()
{
    QString name;
    if (!get_select_user_name(name)) {
        QMessageBox msgbox;
        msgbox.setText("Please select a contact!");
        msgbox.exec();
        return;
    }

    CmdDialog *dlg = new CmdDialog(name, this);
    dlg->setWindowTitle("Remote Contorl From: " + name);
    dlg->setAttribute(Qt::WA_DeleteOnClose);
    dlg->exec();
}

void MainWindow::on_action_browse_triggered()
{
    QString name;
    if (!get_select_user_name(name)) {
        QMessageBox msgbox;
        msgbox.setText("Please select a contact!");
        msgbox.exec();
        return;
    }

    BrowseFilesDialog *dlg = new  BrowseFilesDialog(name, this);
    dlg->setWindowTitle("Remote Contorl From: " + name);
    dlg->setAttribute(Qt::WA_DeleteOnClose);
    dlg->exec();
}
