#ifndef HMDBREADER_H
#define HMDBREADER_H

#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QtXml>

#include "database.h"

class HMDBReader
{
public:
    HMDBReader(QTreeWidget *tree, Tree *db_);
    bool readFile(const QString &fileName);

private:
    void readMetabocardindexElement();
    QTreeWidgetItem *GetQTreeWidgetItem(QString reader_name, QTreeWidgetItem *parent);
    void readTagElement(QTreeWidgetItem *parent);
    void readContentElement(QTreeWidgetItem *parent);
    void skipUnknownElement();

    QTreeWidget *treeWidget;
    QXmlStreamReader reader;
    Tree *db;
    QString molname, hmdbid;
    QStringList tag_key_added;
};

#endif
