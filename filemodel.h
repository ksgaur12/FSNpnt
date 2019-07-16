#ifndef FILEMODEL_H
#define FILEMODEL_H

#include <QStandardItemModel>
#include <QModelIndex>

class filemodel : public QStandardItemModel
{
    Q_OBJECT
public:
    filemodel(QObject *parent = nullptr);

private:
    QModelIndex _last_index_clicked;
    QString _last_str_path;

public slots:
    void download_file(QString dir_url);
    void item_clicked(QModelIndex index);
    void append_item(QString val);
};

#endif // FILEMODEL_H
