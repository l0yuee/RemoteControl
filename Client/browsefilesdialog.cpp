#include "browsefilesdialog.h"


#include "ui_browsefilesdialog.h"
#include "mainwindow.h"
#include <QList>
#include <QFileDialog>
#include <QDebug>
#include "myprogressbar.h"
#include <process.h>

BrowseFilesDialog::BrowseFilesDialog(QString target, QWidget *parent) :
    QDialog(parent), target_name(target),
    ui(new Ui::BrowseFilesDialog)
{
    ui->setupUi(this);
    ui->splitter->setStretchFactor(0, 3);
    ui->splitter->setStretchFactor(1, 7);

    QStringList head_label;
    head_label << "Driver/Directory";
    dirs_model_->setHorizontalHeaderLabels(head_label);
    head_label.clear();
    head_label << "File Name" << "Modify Time" << "File Size";
    files_model_->setHorizontalHeaderLabels(head_label);

    ui->treeView_dirs->setModel(dirs_model_);
    ui->tableView_files->setModel(files_model_);

    // tableView 右键菜单
    ui->tableView_files->setContextMenuPolicy(Qt::CustomContextMenu);
    table_view_pop_menu = new QMenu(this);
    table_view_pop_menu->addAction(ui->action_upload_file);
    table_view_pop_menu->addAction(ui->action_download_file);

    connect(ui->tableView_files, static_cast<void (QTableView::*)(const QPoint &)>(&QTableView::customContextMenuRequested),
        this, [=](const QPoint &pos) {
        auto select_model = ui->tableView_files->selectionModel();
        auto enable_flag = select_model->hasSelection();
        ui->action_download_file->setEnabled(enable_flag);
        table_view_pop_menu->exec(ui->tableView_files->mapToGlobal(pos) + QPoint(0, 25));
    });

    connect(ui->treeView_dirs, static_cast<void (QTreeView::*)(const QModelIndex &)>(&QTreeView::collapsed),
        this, static_cast<void (BrowseFilesDialog::*)(const QModelIndex &)>(&BrowseFilesDialog::dir_collapsed));

    connect(ui->treeView_dirs, static_cast<void (QTreeView::*)(const QModelIndex &)>(&QTreeView::expanded),
        this, static_cast<void (BrowseFilesDialog::*)(const QModelIndex &)>(&BrowseFilesDialog::dir_expanded));

    if (static_cast<MainWindow *>(this->parent())->get_client_core()) {
        client_core_ = static_cast<MainWindow *>(this->parent())->get_client_core();
        source_name = static_cast<MainWindow *>(this->parent())->get_client_core()->user_name;
        // 连接信号
        connect(client_core_, static_cast<void (ClientCore::*)(QString)>(&ClientCore::append_driver_info),
            this, [=](QString path) {
            dirs_model_->appendRow(new QStandardItem(path));
        });

        connect(client_core_, static_cast<void (ClientCore::*)(QModelIndex, QString)>(&ClientCore::append_dir_info), 
            this, [=](QModelIndex index, QString name) {
            dirs_model_->itemFromIndex(index)->appendRow(new QStandardItem(name));
        });

        connect(client_core_, static_cast<void (ClientCore::*)(QStringList)>(&ClientCore::append_file_info),
            this, [=](QStringList info) {
            QList<QStandardItem *> list;

            for(auto &item : info) {
                list.append(new QStandardItem(item));
            }

            files_model_->appendRow(list);
        });


        // 发请求
        CTRL_DriverRequest ctrl_driver_request { 0 };
        std::string s_name = source_name.toLocal8Bit().data();
        std::string t_name = target_name.toLocal8Bit().data();
        strncpy(ctrl_driver_request.source_name, s_name.c_str(), s_name.length());
        strncpy(ctrl_driver_request.target_name, t_name.c_str(), t_name.length());
        ProtocolHead head { 1, (uint16_t)ChatCommand::CONTROL_DRIVER_REQUEST, sizeof(CTRL_DriverRequest) };
        client_core_->send_pack((const char *)&head, sizeof(ProtocolHead));
        client_core_->send_pack((const char *)&ctrl_driver_request, head.len);
    }
}

BrowseFilesDialog::~BrowseFilesDialog()
{
    delete ui;
}

void BrowseFilesDialog::dir_collapsed(const QModelIndex &index)
{
    auto root = dirs_model_->itemFromIndex(index);
    root->removeRows(0, root->rowCount());
}

void BrowseFilesDialog::dir_expanded(const QModelIndex &index)
{
    client_core_->current_expanded_index = index;
    // 构造路径
    std::string path = generate_path(index).toLocal8Bit().data();

    CTRL_FilesRequest ctrl_files_request { 0 };
    std::string s_name = source_name.toLocal8Bit().data();
    std::string t_name = target_name.toLocal8Bit().data();
    strncpy(ctrl_files_request.target_name, t_name.c_str(), t_name.length());
    strncpy(ctrl_files_request.source_name, s_name.c_str(), s_name.length());
    strncpy(ctrl_files_request.path, path.c_str(), path.length());

    ProtocolHead head { 1, (uint16_t)ChatCommand::CONTROL_FILES_REQUEST, sizeof(CTRL_FilesRequest) };
    client_core_->send_pack((const char *)&head, sizeof(ProtocolHead));
    client_core_->send_pack((const char *)&ctrl_files_request, head.len);

    files_model_->removeRows(0, files_model_->rowCount());
}

QString BrowseFilesDialog::generate_path(QModelIndex index)
{
    auto path = index.data(0).toString();
    auto ancestor = index.parent();
    while(ancestor.isValid()) {
        if(ancestor.parent().isValid()) {   // 不是根
            path = ancestor.data(0).toString() + "/" + path;
        } else {    // 是根
            path = ancestor.data(0).toString() + path;
        }
        ancestor = ancestor.parent();
    }
    path.replace("\\", "/");
    if(path[path.length() - 1] != '/') {
        path += "/";
    }
    return path;
}

void BrowseFilesDialog::on_action_download_file_triggered()
{
    auto index = ui->tableView_files->selectionModel()->currentIndex();
    QString download_file = files_model_->index(index.row(), 0).data(0).toString();
    QString target_path = generate_path(client_core_->current_expanded_index) + download_file;

    QString save_file = QFileDialog::getSaveFileName(this, "Save File", QDir::currentPath() + "/" + download_file, "All Files (*.*)");

    if(save_file.isEmpty()) {
        return;
    }

    std::string save_download_file = save_file.toLocal8Bit().data();
    client_core_->handle_dst_file = ::CreateFileA(save_download_file.c_str(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);


    // 构造包
    CTRL_DownloadFileRequest ctrl_download_file_request { 0 };
    std::string s_name = source_name.toLocal8Bit().data();
    std::string t_name = target_name.toLocal8Bit().data();
    std::string path = target_path.toLocal8Bit().data();
    strncpy(ctrl_download_file_request.target_name, t_name.c_str(), t_name.length());
    strncpy(ctrl_download_file_request.source_name, s_name.c_str(), s_name.length());
    strncpy(ctrl_download_file_request.path, path.c_str(), path.length());
    ProtocolHead head { 1, (uint16_t)ChatCommand::CONTROL_DOWNLOAD_FILE_REQUEST, sizeof(CTRL_DownloadFileRequest) };

    // 发包
    client_core_->send_pack((const char *)&head, sizeof(ProtocolHead));
    client_core_->send_pack((const char *)&ctrl_download_file_request, head.len);

    // 弹进度条 子类化
    /*QProgressDialog *progress_dialog = new QProgressDialog("Copying File: " + download_file + " From:" + target_name, "Cancel", 0, 100, this);
    progress_dialog->setWindowTitle("Copying...");*/
    MyProgressBar progress_dialog(ProgressType::DOWNLOAD, client_core_, this);
    progress_dialog.setWindowModality(Qt::WindowModal);
    progress_dialog.setWindowTitle("Downloading...");
    progress_dialog.setLabelText("DownLoad: " + download_file + " From " + target_name + ": " + target_path);
    progress_dialog.progress_timer->start(1000);
    progress_dialog.exec();
    if(progress_dialog.wasCanceled()) {
        // 取消了下载
        CTRL_DownloadFileCancel ctrl_download_file_cancel { 0 };
        strncpy(ctrl_download_file_cancel.target_name, t_name.c_str(), t_name.length());
        ProtocolHead head { 1, (uint16_t)ChatCommand::CONTROL_DOWNLOAD_FILE_CANCEL, sizeof(CTRL_DownloadFileCancel) };
        client_core_->send_pack((const char *)&head, sizeof(ProtocolHead));
        client_core_->send_pack((const char *)&ctrl_download_file_cancel, head.len);
    }
    client_core_->progress_value = 0;
}

void BrowseFilesDialog::on_action_upload_file_triggered()
{
    if(!client_core_->current_expanded_index.isValid()) {
        // 无法构建出目录，直接返回
        return;
    }

    // 构建上传目录
    QString target_path = generate_path(client_core_->current_expanded_index);
    qDebug() << target_path;
    std::string t_name = target_name.toLocal8Bit().data();  // 对方主机名
    client_core_->upload_target = t_name;
    QString save_file = QFileDialog::getOpenFileName(this, "Open File", QDir::currentPath(), "All Files (*.*)");

    // 上传文件不能为空
    if (save_file.isEmpty()) {
        return;
    }

    client_core_->need_upload_file = save_file.toLocal8Bit().data();

    auto tokens = save_file.split("/");
    std::string filename = tokens[tokens.size() - 1].toLocal8Bit().data();

    std::string path = target_path.toLocal8Bit().data() + filename;

    // 构造包
    CTRL_UploadFileRequest ctrl_upload_file_request { 0 };
    strncpy(ctrl_upload_file_request.target_name, t_name.c_str(), t_name.length());
    strncpy(ctrl_upload_file_request.path, path.c_str(), path.length());
    ProtocolHead head { 1, (uint16_t)ChatCommand::CONTROL_UPLOAD_FILE_REQUEST, sizeof(CTRL_UploadFileRequest) };
    client_core_->send_pack((const char *)&head, sizeof(ProtocolHead));
    client_core_->send_pack((const char *)&ctrl_upload_file_request, head.len);

    // 创建上传线程
    ::CloseHandle((HANDLE)_beginthreadex(NULL, 0, ClientCore::upload_file, client_core_, 0, NULL));


    MyProgressBar progress_dialog(ProgressType::UPLOAD, client_core_, this);
    progress_dialog.setWindowModality(Qt::WindowModal);
    progress_dialog.setWindowTitle("UpLoading...");
    progress_dialog.setLabelText("UpLoad: " + save_file + " To " + target_name + ": " + target_path);
    progress_dialog.progress_timer->start(1000);
    progress_dialog.exec();
    if (progress_dialog.wasCanceled()) {
        // 取消了上传
        /*CTRL_UploadFileCancel ctrl_upload_file_cancel { 0 };
        strncpy(ctrl_upload_file_cancel.target_name, t_name.c_str(), t_name.length());
        ProtocolHead head { 1, (uint16_t)ChatCommand::CONTROL_UPLOAD_FILE_CANCEL, sizeof(CTRL_UploadFileCancel) };
        client_core_->send_pack((const char *)&head, sizeof(ProtocolHead));
        client_core_->send_pack((const char *)&ctrl_upload_file_cancel, head.len);*/
        client_core_->upload_file_is_run = false;
    }
    client_core_->progress_value = 0;
}
