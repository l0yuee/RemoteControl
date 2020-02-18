/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.13.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QTableView>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QAction *action_screen;
    QAction *action_cmd;
    QAction *action_browse;
    QWidget *centralwidget;
    QGridLayout *gridLayout_2;
    QHBoxLayout *horizontalLayout_3;
    QLabel *label;
    QLineEdit *lineEdit_server_ip;
    QLabel *label_2;
    QSpinBox *spinBox_server_port;
    QLabel *label_3;
    QLineEdit *lineEdit_user_name;
    QPushButton *pushButton_login;
    QHBoxLayout *horizontalLayout_2;
    QVBoxLayout *verticalLayout_2;
    QTextEdit *textEdit_context;
    QHBoxLayout *horizontalLayout;
    QTextEdit *textEdit_msg;
    QVBoxLayout *verticalLayout;
    QGroupBox *groupBox;
    QGridLayout *gridLayout;
    QRadioButton *radioButton_chat_us;
    QRadioButton *radioButton_chat_to;
    QPushButton *pushButton_send;
    QVBoxLayout *verticalLayout_3;
    QLabel *label_4;
    QTableView *tableView_users;
    QMenuBar *menubar;
    QMenu *menu;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(715, 457);
        action_screen = new QAction(MainWindow);
        action_screen->setObjectName(QString::fromUtf8("action_screen"));
        action_cmd = new QAction(MainWindow);
        action_cmd->setObjectName(QString::fromUtf8("action_cmd"));
        action_browse = new QAction(MainWindow);
        action_browse->setObjectName(QString::fromUtf8("action_browse"));
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        gridLayout_2 = new QGridLayout(centralwidget);
        gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        label = new QLabel(centralwidget);
        label->setObjectName(QString::fromUtf8("label"));

        horizontalLayout_3->addWidget(label);

        lineEdit_server_ip = new QLineEdit(centralwidget);
        lineEdit_server_ip->setObjectName(QString::fromUtf8("lineEdit_server_ip"));

        horizontalLayout_3->addWidget(lineEdit_server_ip);

        label_2 = new QLabel(centralwidget);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        horizontalLayout_3->addWidget(label_2);

        spinBox_server_port = new QSpinBox(centralwidget);
        spinBox_server_port->setObjectName(QString::fromUtf8("spinBox_server_port"));
        spinBox_server_port->setMaximum(65535);
        spinBox_server_port->setValue(23333);

        horizontalLayout_3->addWidget(spinBox_server_port);

        label_3 = new QLabel(centralwidget);
        label_3->setObjectName(QString::fromUtf8("label_3"));

        horizontalLayout_3->addWidget(label_3);

        lineEdit_user_name = new QLineEdit(centralwidget);
        lineEdit_user_name->setObjectName(QString::fromUtf8("lineEdit_user_name"));
        lineEdit_user_name->setMaxLength(31);

        horizontalLayout_3->addWidget(lineEdit_user_name);

        pushButton_login = new QPushButton(centralwidget);
        pushButton_login->setObjectName(QString::fromUtf8("pushButton_login"));

        horizontalLayout_3->addWidget(pushButton_login);


        gridLayout_2->addLayout(horizontalLayout_3, 0, 0, 1, 1);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        verticalLayout_2 = new QVBoxLayout();
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        textEdit_context = new QTextEdit(centralwidget);
        textEdit_context->setObjectName(QString::fromUtf8("textEdit_context"));

        verticalLayout_2->addWidget(textEdit_context);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        textEdit_msg = new QTextEdit(centralwidget);
        textEdit_msg->setObjectName(QString::fromUtf8("textEdit_msg"));

        horizontalLayout->addWidget(textEdit_msg);

        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        groupBox = new QGroupBox(centralwidget);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(groupBox->sizePolicy().hasHeightForWidth());
        groupBox->setSizePolicy(sizePolicy);
        gridLayout = new QGridLayout(groupBox);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        radioButton_chat_us = new QRadioButton(groupBox);
        radioButton_chat_us->setObjectName(QString::fromUtf8("radioButton_chat_us"));
        radioButton_chat_us->setChecked(true);

        gridLayout->addWidget(radioButton_chat_us, 0, 0, 1, 1);

        radioButton_chat_to = new QRadioButton(groupBox);
        radioButton_chat_to->setObjectName(QString::fromUtf8("radioButton_chat_to"));

        gridLayout->addWidget(radioButton_chat_to, 1, 0, 1, 1);


        verticalLayout->addWidget(groupBox);

        pushButton_send = new QPushButton(centralwidget);
        pushButton_send->setObjectName(QString::fromUtf8("pushButton_send"));

        verticalLayout->addWidget(pushButton_send);

        verticalLayout->setStretch(0, 1);
        verticalLayout->setStretch(1, 4);

        horizontalLayout->addLayout(verticalLayout);


        verticalLayout_2->addLayout(horizontalLayout);

        verticalLayout_2->setStretch(0, 4);
        verticalLayout_2->setStretch(1, 1);

        horizontalLayout_2->addLayout(verticalLayout_2);

        verticalLayout_3 = new QVBoxLayout();
        verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
        label_4 = new QLabel(centralwidget);
        label_4->setObjectName(QString::fromUtf8("label_4"));

        verticalLayout_3->addWidget(label_4);

        tableView_users = new QTableView(centralwidget);
        tableView_users->setObjectName(QString::fromUtf8("tableView_users"));
        tableView_users->setEditTriggers(QAbstractItemView::NoEditTriggers);
        tableView_users->setSelectionMode(QAbstractItemView::SingleSelection);
        tableView_users->setSelectionBehavior(QAbstractItemView::SelectRows);

        verticalLayout_3->addWidget(tableView_users);


        horizontalLayout_2->addLayout(verticalLayout_3);

        horizontalLayout_2->setStretch(0, 4);
        horizontalLayout_2->setStretch(1, 1);

        gridLayout_2->addLayout(horizontalLayout_2, 1, 0, 1, 1);

        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        menubar->setGeometry(QRect(0, 0, 715, 23));
        menu = new QMenu(menubar);
        menu->setObjectName(QString::fromUtf8("menu"));
        MainWindow->setMenuBar(menubar);

        menubar->addAction(menu->menuAction());
        menu->addAction(action_screen);
        menu->addAction(action_cmd);
        menu->addAction(action_browse);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "Chat Client", nullptr));
        action_screen->setText(QCoreApplication::translate("MainWindow", "\345\261\217\345\271\225\347\233\221\346\216\247", nullptr));
#if QT_CONFIG(tooltip)
        action_screen->setToolTip(QCoreApplication::translate("MainWindow", "\345\261\217\345\271\225\347\233\221\346\216\247", nullptr));
#endif // QT_CONFIG(tooltip)
        action_cmd->setText(QCoreApplication::translate("MainWindow", "\350\277\234\347\250\213CMD", nullptr));
#if QT_CONFIG(tooltip)
        action_cmd->setToolTip(QCoreApplication::translate("MainWindow", "\350\277\234\347\250\213CMD", nullptr));
#endif // QT_CONFIG(tooltip)
        action_browse->setText(QCoreApplication::translate("MainWindow", "\346\265\217\350\247\210\346\226\207\344\273\266", nullptr));
#if QT_CONFIG(tooltip)
        action_browse->setToolTip(QCoreApplication::translate("MainWindow", "\346\265\217\350\247\210\346\226\207\344\273\266", nullptr));
#endif // QT_CONFIG(tooltip)
        label->setText(QCoreApplication::translate("MainWindow", "\346\234\215\345\212\241\345\231\250IP:", nullptr));
        lineEdit_server_ip->setText(QCoreApplication::translate("MainWindow", "127.0.0.1", nullptr));
        label_2->setText(QCoreApplication::translate("MainWindow", "\346\234\215\345\212\241\345\231\250\347\253\257\345\217\243\357\274\232", nullptr));
        label_3->setText(QCoreApplication::translate("MainWindow", "\347\224\250\346\210\267\345\220\215\357\274\232", nullptr));
        pushButton_login->setText(QCoreApplication::translate("MainWindow", "\347\231\273\345\275\225", nullptr));
        groupBox->setTitle(QCoreApplication::translate("MainWindow", "\350\201\212\345\244\251\346\250\241\345\274\217", nullptr));
        radioButton_chat_us->setText(QCoreApplication::translate("MainWindow", "\347\276\244\350\201\212", nullptr));
        radioButton_chat_to->setText(QCoreApplication::translate("MainWindow", "\347\247\201\350\201\212", nullptr));
        pushButton_send->setText(QCoreApplication::translate("MainWindow", "\345\217\221\351\200\201", nullptr));
        label_4->setText(QCoreApplication::translate("MainWindow", "\350\201\224\347\263\273\344\272\272\357\274\232", nullptr));
        menu->setTitle(QCoreApplication::translate("MainWindow", "\345\212\237\350\203\275", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
