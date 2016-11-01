#ifndef KEGGREADER_H
#define KEGGREADER_H

#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QString>

#include "database.h"

class KEGGReader
{
public:
    KEGGReader(QTreeWidget *tree, Tree *db_);
    bool readFile(const QString &fileName);

private:
    QString GetRandomString() const;
    void compress(const QString &infileName , const QString &outfileName);
    void uncompress(const QString &infileName , const QString &outfileName);
    QTreeWidget *treeWidget;
    Tree *db;
    QString molname, hmdbid;
    QStringList tag_key_added;
};

#endif
