#ifndef MYPROGRESSBAR_H
#define MYPROGRESSBAR_H

#include <QWidget>
#include "clientcore.h"
#include <QProgressDialog>
#include <QTimer>

enum ProgressType {
    DOWNLOAD,   // обть
    UPLOAD      // ио╢╚
};

class MyProgressBar : public QProgressDialog
{
    Q_OBJECT
public:
    explicit MyProgressBar(ProgressType type, ClientCore *client, QWidget *parent = nullptr);

signals:

public slots:


public:
    ClientCore *client_core = nullptr;
    QTimer *progress_timer;
    ProgressType progress_type;
};

#endif // MYPROGRESSBAR_H
