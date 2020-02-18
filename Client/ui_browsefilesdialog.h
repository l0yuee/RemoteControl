/********************************************************************************
** Form generated from reading UI file 'browsefilesdialog.ui'
**
** Created by: Qt User Interface Compiler version 5.13.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_BROWSEFILESDIALOG_H
#define UI_BROWSEFILESDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QSplitter>
#include <QtWidgets/QTableView>
#include <QtWidgets/QTreeView>

QT_BEGIN_NAMESPACE

class Ui_BrowseFilesDialog
{
public:
    QAction *action_download_file;
    QAction *action_upload_file;
    QGridLayout *gridLayout;
    QSplitter *splitter;
    QTreeView *treeView_dirs;
    QTableView *tableView_files;

    void setupUi(QDialog *BrowseFilesDialog)
    {
        if (BrowseFilesDialog->objectName().isEmpty())
            BrowseFilesDialog->setObjectName(QString::fromUtf8("BrowseFilesDialog"));
        BrowseFilesDialog->resize(696, 467);
        action_download_file = new QAction(BrowseFilesDialog);
        action_download_file->setObjectName(QString::fromUtf8("action_download_file"));
        action_upload_file = new QAction(BrowseFilesDialog);
        action_upload_file->setObjectName(QString::fromUtf8("action_upload_file"));
        gridLayout = new QGridLayout(BrowseFilesDialog);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        splitter = new QSplitter(BrowseFilesDialog);
        splitter->setObjectName(QString::fromUtf8("splitter"));
        splitter->setOrientation(Qt::Horizontal);
        treeView_dirs = new QTreeView(splitter);
        treeView_dirs->setObjectName(QString::fromUtf8("treeView_dirs"));
        treeView_dirs->setEditTriggers(QAbstractItemView::NoEditTriggers);
        splitter->addWidget(treeView_dirs);
        tableView_files = new QTableView(splitter);
        tableView_files->setObjectName(QString::fromUtf8("tableView_files"));
        tableView_files->setEditTriggers(QAbstractItemView::NoEditTriggers);
        tableView_files->setSelectionMode(QAbstractItemView::SingleSelection);
        tableView_files->setSelectionBehavior(QAbstractItemView::SelectRows);
        splitter->addWidget(tableView_files);
        tableView_files->verticalHeader()->setVisible(false);

        gridLayout->addWidget(splitter, 0, 0, 1, 1);


        retranslateUi(BrowseFilesDialog);

        QMetaObject::connectSlotsByName(BrowseFilesDialog);
    } // setupUi

    void retranslateUi(QDialog *BrowseFilesDialog)
    {
        BrowseFilesDialog->setWindowTitle(QCoreApplication::translate("BrowseFilesDialog", "Dialog", nullptr));
        action_download_file->setText(QCoreApplication::translate("BrowseFilesDialog", "\344\270\213\350\275\275", nullptr));
        action_upload_file->setText(QCoreApplication::translate("BrowseFilesDialog", "\344\270\212\344\274\240", nullptr));
#if QT_CONFIG(tooltip)
        action_upload_file->setToolTip(QCoreApplication::translate("BrowseFilesDialog", "\344\270\212\344\274\240", nullptr));
#endif // QT_CONFIG(tooltip)
    } // retranslateUi

};

namespace Ui {
    class BrowseFilesDialog: public Ui_BrowseFilesDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_BROWSEFILESDIALOG_H
