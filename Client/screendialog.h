#ifndef SCREENDIALOG_H
#define SCREENDIALOG_H

#include <QDialog>
#include "clientcore.h"
#include <QPoint>

namespace Ui {
class ScreenDialog;
}

class ScreenDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ScreenDialog(QString target, QWidget *parent = nullptr);
    ~ScreenDialog();

private:
    Ui::ScreenDialog *ui;
    QString target_name;
    QString source_name;
    ClientCore *client_core_;

public slots:
    void screen_show(QPixmap pixmap);

    // QWidget interface
protected:
    void closeEvent(QCloseEvent *event);

    // QWidget interface
protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseDoubleClickEvent(QMouseEvent *event);

    // QWidget interface
protected:
    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);
};

#endif // SCREENDIALOG_H
