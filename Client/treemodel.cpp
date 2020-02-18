#include "treemodel.h"

TreeModel::TreeModel(QObject *parent) : QStandardItemModel(parent)
{
    
}


bool TreeModel::hasChildren(const QModelIndex &parent) const
{
    return true;
}
