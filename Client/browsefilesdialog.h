#ifndef BROWSEFILESDIALOG_H
#define BROWSEFILESDIALOG_H

#include <QDialog>
#include "clientcore.h"
#include "treemodel.h"
#include <QMenu>

namespace Ui {
class BrowseFilesDialog;
}

class BrowseFilesDialog : public QDialog
{
    Q_OBJECT

public:
    explicit BrowseFilesDialog(QString target, QWidget *parent = nullptr);
    ~BrowseFilesDialog();


public slots:
    // 目录收起
    void dir_collapsed(const QModelIndex &index);
    // 目录展开
    void dir_expanded(const QModelIndex &index);



protected:
    QString generate_path(QModelIndex index);

private slots:
    void on_action_download_file_triggered();

    void on_action_upload_file_triggered();

private:
    Ui::BrowseFilesDialog *ui;
    QString target_name;
    QString source_name;
    ClientCore *client_core_;

    QStandardItemModel * dirs_model_ = new TreeModel(this);
    QStandardItemModel *files_model_ = new QStandardItemModel(this);

    QMenu *table_view_pop_menu;
};

#endif // BROWSEFILESDIALOG_H
