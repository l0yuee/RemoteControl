#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "clientcore.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    inline ClientCore * get_client_core() { return client_core_; }

protected:
    void ui_init();
    void ui_enable(bool enable = true);
    void say_all(QString message);
    void say_to(QString message);
    bool get_select_user_name(QString &name);

private slots:
    void on_pushButton_login_clicked();

    void on_radioButton_chat_us_clicked();

    void on_radioButton_chat_to_clicked();

    void on_pushButton_send_clicked();

    void on_action_screen_triggered();

    void on_action_cmd_triggered();

    void on_action_browse_triggered();

private:
    Ui::MainWindow *ui;

    ClientCore *client_core_;

    enum {
        MODEL_SAY_ALL,
        MODEL_SAY_TO
    } SAY_MODEL;

    // QWidget interface
protected:
    void closeEvent(QCloseEvent *event);
};
#endif // MAINWINDOW_H
