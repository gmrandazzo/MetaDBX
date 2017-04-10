#include "genericreader.h"
#include <QFile>
#include <QByteArray>
#include <QTextStream>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QDebug>
#include <QMessageBox>

QString GenericReader::GetRandomString() const
{
  const QString possibleCharacters("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789");
  const int randomStringLength = 5;

  QString rndstr;
  for(int i = 0; i < randomStringLength; i++){
   int index = qrand() % possibleCharacters.length();
   QChar nextChar = possibleCharacters.at(index);
   rndstr.append(nextChar);
  }
  return rndstr;
}

void GenericReader::compress(const QString &infileName , const QString &outfileName)
{
  QFile infile(infileName);
  QFile outfile(outfileName);
  infile.open(QIODevice::ReadOnly);
  outfile.open(QIODevice::WriteOnly);
  QByteArray uncompressed_data = infile.readAll();
  QByteArray compressed_data = qCompress(uncompressed_data, 9);
  outfile.write(compressed_data);
  infile.close();
  outfile.close();
}

void GenericReader::uncompress(const QString &infileName, const QString &outfileName)
{
  QFile infile(infileName);
  QFile outfile(outfileName);
  infile.open(QIODevice::ReadOnly);
  outfile.open(QIODevice::WriteOnly);
  QByteArray uncompressed_data = infile.readAll();
  QByteArray compressed_data = qUncompress(uncompressed_data);
  outfile.write(compressed_data);
  infile.close();
  outfile.close();
}

bool GenericReader::readFile(const QString &fileName)
{
  QString tmpfname = GetRandomString();
  uncompress(fileName, tmpfname);

  QFile file(tmpfname);

  if(!file.open(QIODevice::ReadOnly)) {
    QMessageBox::information(0, "error", file.errorString());
  }

  QTreeWidgetItem *item = new QTreeWidgetItem(treeWidget->invisibleRootItem());
  item->setText(0, "pathways");
  (*db).addNode("pathways");

  QTextStream in(&file);
  while(!in.atEnd()) {
    QString line = in.readLine();
    QStringList  fields = line.split(";");
    // fields:
    // 0: compoundid
    // 1: molname
    // 2: pathways
    // 3: cass number
    // 4: Other DBLINK
    // 5: Pubchem SID
    QStringList pathwayslst = fields[2].split("//", QString::SkipEmptyParts);
    for(int i = 0; i < pathwayslst.size(); i++){
      int node_indx = (*db).nodes[0].indexOfTag(pathwayslst[i]);
      if(node_indx > -1){ // node found
        if((*db).nodes[0].content[node_indx].GetIndexOfByMOLID(fields[0]) == -1){ // compoundid not in list
          (*db).nodes[0].content[node_indx].append(fields[0], fields[1]);
        }
        else{
          continue;
        }
      }
      else{ // node not found neither compoundid, name in list
        (*db).nodes[0].appendTag(pathwayslst[i]);
        (*db).nodes[0].lastTagContent().append(fields[0], fields[1]);
      }
    }
  }
  file.close();

  QFile::remove(tmpfname);
  return true;
}

GenericReader::GenericReader(QTreeWidget *tree, Tree *db_)
{
  treeWidget = tree;
  db = db_;
}
