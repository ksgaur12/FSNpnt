#include "filemodel.h"
#include "filemanager.h"
#include <QStandardItem>
#include <QDebug>

extern FileManager file_manager;

filemodel::filemodel(QObject *parent)
    : QStandardItemModel {parent}
{
    setColumnCount(1);
    QStandardItem* rootItem = invisibleRootItem();

    QStandardItem* home = new QStandardItem;
    home->setText("/");
    home->setDropEnabled(true);
    rootItem->appendRow(home);
}

void filemodel::item_clicked(QModelIndex index)
{
    QString str = this->data(index).toByteArray();
    QStandardItem* item = this->itemFromIndex(index)->parent();

    while(item != nullptr){
        str.prepend("/");
        str.prepend(item->text());
        item = item->parent();
    }
    _last_str_path = str;
    if(this->itemFromIndex(index)->hasChildren()){
        return;
    }

     file_manager.listDirectory(str);

    _last_index_clicked = index;
}

void filemodel::append_item(QString val)
{
    QStandardItem* item = this->itemFromIndex(_last_index_clicked);
    QStandardItem* child = new QStandardItem;
    child->setText(val);
    item->appendRow(child);
}

void filemodel::download_file(QString dir_url){
    if(_last_str_path.isEmpty()){
        return;
    }
    dir_url.remove(0,7);
    //qDebug() << dir_url;
    QDir dir;
    dir.setPath(dir_url);
    qDebug() << dir.exists();
   // qDebug() << _last_str_path;

    file_manager.downloadFile(_last_str_path, dir, true);
}
