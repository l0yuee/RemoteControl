#ifndef CMDDIALOG_H
#define CMDDIALOG_H

#include <QDialog>
#include "clientcore.h"

namespace Ui {
class CmdDialog;
}

class CmdDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CmdDialog(QString target, QWidget *parent = nullptr);
    ~CmdDialog();

private slots:
    void on_pushButton_execute_clicked();

private:
    Ui::CmdDialog *ui;
    QString target_name;
    QString source_name;
    ClientCore *client_core_;

    // QWidget interface
protected:
    void closeEvent(QCloseEvent *event);
};

#endif // CMDDIALOG_H
