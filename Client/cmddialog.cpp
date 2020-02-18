#include "cmddialog.h"


#include "ui_cmddialog.h"
#include "mainwindow.h"

CmdDialog::CmdDialog(QString target, QWidget *parent) :
    QDialog(parent), target_name(target),
    ui(new Ui::CmdDialog)
{
    ui->setupUi(this);


    if (static_cast<MainWindow *>(this->parent())->get_client_core()) {
        client_core_ = static_cast<MainWindow *>(this->parent())->get_client_core();
        source_name = static_cast<MainWindow *>(this->parent())->get_client_core()->user_name;
        // 连接信号
        connect(client_core_, static_cast<void (ClientCore::*)(QString)>(&ClientCore::ctrl_cmd),
            this, [=](QString text) {
            ui->textEdit_context->setPlainText(ui->textEdit_context->toPlainText() + text);
            ui->textEdit_context->moveCursor(QTextCursor::End, QTextCursor::MoveAnchor);
        });

        // 发远控mcd请求
        CTRL_CmdRequest ctrl_cmd_request { 0 };
        std::string s_name = source_name.toLocal8Bit().data();
        std::string t_name = target_name.toLocal8Bit().data();
        strncpy(ctrl_cmd_request.source_name, s_name.c_str(), s_name.length());
        strncpy(ctrl_cmd_request.target_name, t_name.c_str(), t_name.length());
        ProtocolHead head { 1, (uint16_t)ChatCommand::CONTROL_CMD_REQUEST, sizeof(CTRL_CmdRequest) };
        client_core_->send_pack((const char *)&head, sizeof(ProtocolHead));
        client_core_->send_pack((const char *)&ctrl_cmd_request, head.len);
    }

    ui->lineEdit_command->setFocus();
}

CmdDialog::~CmdDialog()
{
    delete ui;
}

void CmdDialog::on_pushButton_execute_clicked()
{
    QString exec_text = ui->lineEdit_command->text();
    if(exec_text.isEmpty()) {
        return;
    }

    // 构造包
    exec_text += "\n";
    std::string exec = exec_text.toLocal8Bit().data();
    ProtocolHead head { 1, (uint16_t)ChatCommand::CONTROL_CMD_EXEC, sizeof(CTRL_CmdExec) + exec.length() + 1 };
    CTRL_CmdExec *ctrl_cmd_exec = (CTRL_CmdExec *)calloc(1, head.len);
    std::string t_name = target_name.toLocal8Bit().data();
    strncpy(ctrl_cmd_exec->target_name, t_name.c_str(), t_name.length());
    strncpy(ctrl_cmd_exec->data, exec.c_str(), exec.length());

    client_core_->send_pack((const char *)&head, sizeof(ProtocolHead));
    client_core_->send_pack((const char *)ctrl_cmd_exec, head.len);
    ui->lineEdit_command->clear();

    free(ctrl_cmd_exec);
}


void CmdDialog::closeEvent(QCloseEvent *event)
{
    std::string exec = "exit\n";
    ProtocolHead head { 1, (uint16_t)ChatCommand::CONTROL_CMD_EXEC, sizeof(CTRL_CmdExec) + exec.length() + 1 };
    CTRL_CmdExec *ctrl_cmd_exec = (CTRL_CmdExec *)calloc(1, head.len);
    std::string t_name = target_name.toLocal8Bit().data();
    strncpy(ctrl_cmd_exec->target_name, t_name.c_str(), t_name.length());
    strncpy(ctrl_cmd_exec->data, exec.c_str(), exec.length());

    client_core_->send_pack((const char *)&head, sizeof(ProtocolHead));
    client_core_->send_pack((const char *)ctrl_cmd_exec, head.len);

    free(ctrl_cmd_exec);
}