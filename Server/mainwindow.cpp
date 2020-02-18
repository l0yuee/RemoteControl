#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QValidator>
#include <QRegExp>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    server_core_ = new ServerCore(this);

    ui_init();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::ui_init()
{
    QRegExp reg("\\b(?:(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\b");
    ui->lineEdit_ip->setValidator(new QRegExpValidator(reg, this));

    ui->tableView->setModel(server_core_->get_table_model());
    ui->tableView->setFocus();
}


void MainWindow::on_pushButton_listen_toggled(bool checked)
{
    ui->lineEdit_ip->setEnabled(!checked);
    ui->spinBox_port->setEnabled(!checked);
    if(checked) {
        start_server();
        ui->pushButton_listen->setText("Stop Listening");
    } else {

        ui->pushButton_listen->setText("Start Listening");
        stop_server();
    }
}

void MainWindow::start_server()
{
    std::string ip = ui->lineEdit_ip->text().toLocal8Bit().data();
    unsigned int port = ui->spinBox_port->text().toUInt();
    server_core_->run(ip, port);
}

void MainWindow::stop_server()
{
    server_core_->stop();
}
