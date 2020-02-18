#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <servercore.h>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void ui_init();

private slots:
    void on_pushButton_listen_toggled(bool checked);

private:
    Ui::MainWindow *ui;
    ServerCore *server_core_;

protected:
    void start_server();
    void stop_server();
};
#endif // MAINWINDOW_H
