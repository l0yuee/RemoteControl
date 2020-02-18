#ifndef TREEMODEL_H
#define TREEMODEL_H

#include <QObject>
#include <QStandardItemModel>

class TreeModel : public QStandardItemModel
{
    Q_OBJECT
public:
    explicit TreeModel(QObject *parent = nullptr);
    
signals:
    
public slots:
    
    // QAbstractItemModel interface
public:
    virtual bool hasChildren(const QModelIndex &parent) const override;
};

#endif // TREEMODEL_H
