/********************************************************************************
** Form generated from reading UI file 'cmddialog.ui'
**
** Created by: Qt User Interface Compiler version 5.13.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CMDDIALOG_H
#define UI_CMDDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_CmdDialog
{
public:
    QGridLayout *gridLayout;
    QVBoxLayout *verticalLayout;
    QTextEdit *textEdit_context;
    QHBoxLayout *horizontalLayout;
    QLineEdit *lineEdit_command;
    QPushButton *pushButton_execute;

    void setupUi(QDialog *CmdDialog)
    {
        if (CmdDialog->objectName().isEmpty())
            CmdDialog->setObjectName(QString::fromUtf8("CmdDialog"));
        CmdDialog->resize(716, 455);
        gridLayout = new QGridLayout(CmdDialog);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        textEdit_context = new QTextEdit(CmdDialog);
        textEdit_context->setObjectName(QString::fromUtf8("textEdit_context"));
        textEdit_context->setReadOnly(true);

        verticalLayout->addWidget(textEdit_context);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        lineEdit_command = new QLineEdit(CmdDialog);
        lineEdit_command->setObjectName(QString::fromUtf8("lineEdit_command"));

        horizontalLayout->addWidget(lineEdit_command);

        pushButton_execute = new QPushButton(CmdDialog);
        pushButton_execute->setObjectName(QString::fromUtf8("pushButton_execute"));

        horizontalLayout->addWidget(pushButton_execute);


        verticalLayout->addLayout(horizontalLayout);


        gridLayout->addLayout(verticalLayout, 0, 0, 1, 1);


        retranslateUi(CmdDialog);

        QMetaObject::connectSlotsByName(CmdDialog);
    } // setupUi

    void retranslateUi(QDialog *CmdDialog)
    {
        CmdDialog->setWindowTitle(QCoreApplication::translate("CmdDialog", "Dialog", nullptr));
        pushButton_execute->setText(QCoreApplication::translate("CmdDialog", "\346\211\247\350\241\214", nullptr));
    } // retranslateUi

};

namespace Ui {
    class CmdDialog: public Ui_CmdDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CMDDIALOG_H
