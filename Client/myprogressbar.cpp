#include "myprogressbar.h"

MyProgressBar::MyProgressBar(ProgressType type, ClientCore *client, QWidget *parent) : QProgressDialog(parent), client_core(client), progress_type(type)
{
    progress_timer = new QTimer(this);
    
    connect(progress_timer, &QTimer::timeout, this, [=]() {
        if(client_core) {
            this->setValue(client_core->progress_value);
            if(client_core->progress_value == 100 || this->wasCanceled()) {
                this->setValue(100);
                /*client_core->progress_value = 0;*/
                progress_timer->stop();
            }
        }
    });
}
