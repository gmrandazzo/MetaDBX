#include <QtCore>
#include <QList>
#include <QDebug>
#include "database.h"

int main(void)
{
  DBSTRUCT *db = new DBSTRUCT;
  // test add
  db->appendTag("HMDB61150");
  db->content.last().append("HMDB61168");
  db->content.last().append("HMDB61169");
  db->content.last().append("HMDB61170");
  db->content.last().append("HMDB61172");
  db->appendTag("HMDB61151");
  db->content.last().append("HMDB61160");
  db->content.last().append("HMDB61161");
  db->content.last().append("HMDB61162");
  db->content.last().append("HMDB61163");
  db->appendTag("HMDB61152");
  db->content.last().append("HMDB61164");
  db->content.last().append("HMDB61165");
  db->content.last().append("HMDB61166");
  db->content.last().append("HMDB61167");
  db->appendTag("HMDB61153");
  db->content.last().append("HMDB61159");
  db->content.last().append("HMDB61158");
  db->content.last().append("HMDB61157");
  db->content.last().append("HMDB61156");
  db->appendTag("HMDB61154");
  db->content.last().append("HMDB61173");
  db->content.last().append("HMDB61174");
  db->content.last().append("HMDB61175");
  db->content.last().append("HMDB61176");
  db->appendTag("HMDB61155");
  db->content.last().append("HMDB61177");
  db->content.last().append("HMDB61178");
  db->content.last().append("HMDB61185");
  db->content.last().append("HMDB61196");


  //Test search tag function
  if(db->indexOfTag("HMDB61153") != 3)
    qDebug() << "Error in indexOfTag " << db->indexOfTag("HMDB61153") << " != 3";
  qDebug() << db->indexOfTag("HMDB61153") << " == 3";

    //Test search for content
  if(db->content[3].GetIndexOfByHMDBID("HMDB61156") != 3)
    qDebug() << "Error in GetIndexOfByHMDBID " << db->content[3].GetIndexOfByHMDBID("HMDB61156")<< " != 3";
  qDebug() << db->content[3].GetIndexOfByHMDBID("HMDB61156") << " == 3";

  //Test sort third content!
  for(int i = 0; i < db->content[3].list.size(); i++)
    qDebug() << db->content[3].list[i].hmdbid;
  qDebug() << "-----------";

  qSort(db->content[3].list);

  for(int i = 0; i < db->content[3].list.size(); i++)
    qDebug() << db->content[3].list[i].hmdbid;

  delete db;
  return 0;
}
