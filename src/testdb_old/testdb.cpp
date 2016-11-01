#include <QtCore>
#include <QList>
#include <QDebug>

// database
class CONTENT
{
public:
  //operator to be used with qSort();
  bool operator<(const CONTENT& other) const {
    return hmdbid < other.hmdbid; // sort by hmdbid
  }
  //operator to be used with indexOf();
  bool operator==(const CONTENT& other) const {
    if(hmdbid.compare(other.hmdbid) == 0)
      return true;
    else
      return false;
  }
  QString name;
  QString hmdbid;
};

class LISTCONTENT
{
public:
  LISTCONTENT(){}
  ~LISTCONTENT(){ clear(); }
  void clear(){
    list.clear();
  }
  //Binary Search
  int BinarySearchHMDBID(int from, int to, QString hdmbid_){
    if(from == to){
      return -1;
    }
    else{
      int mid = (from+to)/2;
      if(list[mid].hmdbid.compare(hdmbid_) == 0){
        return mid;
      }
      else if(list[from].hmdbid.compare(hdmbid_) == 0){
        return from;
      }
      else if(list[to].hmdbid.compare(hdmbid_) == 0){
        return to;
      }
      else{
        BinarySearchHMDBID(from, mid, hdmbid_);
        BinarySearchHMDBID(mid+1, to, hdmbid_);
      }
    }
    return -1;
  }
  int GetIndexOfByHMDBID(QString str){
    CONTENT c;
    c.hmdbid = str;
    return list.indexOf(c);
    //return BinarySearchHMDBID(0, list.size()-1, str);
  }
  void append(QString hmdbid_, QString name_){
    list.append(CONTENT());
    list.last().hmdbid = hmdbid_;
    list.last().name = name_;
  }
  void append(QString hmdbid_){
    list.append(CONTENT());
    list.last().hmdbid = hmdbid_;
  }
  QList<CONTENT> list;
};

class NODE
{
public:
  NODE(){}
  ~NODE(){ clear(); }
  void clear(){
    for(int i = 0; i < content.size(); i++)
      content[i].list.clear();
    content.clear();
    tags.clear();
  }
  void appendTag(QString tag){
    tags.append(tag);
    content.append(LISTCONTENT());
  }
  void deleteTag(QString tag){
    int indx = tags.indexOf(tag);
    if(indx > -1){
      tags.removeAt(indx);
      content.removeAt(indx);
    }
    else
      return;
  }
  int indexOfTag(QString tag){
    return tags.indexOf(tag);
  }

  void sortTag(){
    QList< QPair<QString, int> > taglst;
    for(int i = 0; i < tags.size(); i++){
      taglst.append(qMakePair(tags[i], i));
    }
    qSort(taglst.begin(), taglst.end(), caseInsensitiveLessThan);

    QString tmptag;
    LISTCONTENT tmpcontent;
    for(int i = 0; i < taglst.size(); i++){
      tmpcontent = content[i];
      tmptag = tags[i];
      content[i] = content[taglst[i].second];
      tags[i] = tags[taglst[i].second];
      content[taglst[i].second] = tmpcontent;
      tags[taglst[i].second] = tmptag;
    }
  }
  int size(){ return content.size(); }
  QString &lastTagName(){ return tags[tags.size()-1]; }
  LISTCONTENT &lastTagContent(){ return  content[content.size()-1]; }
  QStringList tags;
  QList<LISTCONTENT> content;

private:
  static bool caseInsensitiveLessThan(const QPair<QString,int>& e1, const QPair<QString, int>& e2){
    return e1.first < e2.first;
  }
};

class Tree
{
public:
  Tree(){};
  ~Tree(){};
  void addNode(QString rootname){
    rootnames.append(rootname);
    nodes.append(NODE());
  }
  void deleteNode(int indx){
    rootnames.removeAt(indx);
    nodes.removeAt(indx);
  }
  NODE getLastNode(){ return nodes[nodes.size()-1]; }
//  QList <NODE> &getNodes(){ return nodes; }
//  QList <NODE> &getNodesNames(){ return rootnames; }
//private:
  void clear(){
    for(int i = 0; i < nodes.size(); i++)
      nodes[i].clear();
    nodes.clear();
    rootnames.clear();
  }
  QStringList rootnames;
  QList<NODE> nodes;
};

int main(void)
{
  NODE *db = new NODE;
  // test add
  db->appendTag("HMDB61150");
  db->lastTagContent().append("HMDB61168");
  db->lastTagContent().append("HMDB61169");
  db->lastTagContent().append("HMDB61170");
  db->lastTagContent().append("HMDB61172");
  db->appendTag("HMDB61151");
  db->lastTagContent().append("HMDB61160");
  db->lastTagContent().append("HMDB61161");
  db->lastTagContent().append("HMDB61162");
  db->lastTagContent().append("HMDB61163");
  db->appendTag("HMDB61152");
  db->lastTagContent().append("HMDB61164");
  db->lastTagContent().append("HMDB61165");
  db->lastTagContent().append("HMDB61166");
  db->lastTagContent().append("HMDB61167");
  db->appendTag("HMDB61153");
  db->lastTagContent().append("HMDB61159");
  db->lastTagContent().append("HMDB61158");
  db->lastTagContent().append("HMDB61157");
  db->lastTagContent().append("HMDB61156");
  db->appendTag("HMDB61154");
  db->lastTagContent().append("HMDB61173");
  db->lastTagContent().append("HMDB61174");
  db->lastTagContent().append("HMDB61175");
  db->lastTagContent().append("HMDB61176");
  db->appendTag("HMDB61155");
  db->lastTagContent().append("HMDB61177");
  db->lastTagContent().append("HMDB61178");
  db->lastTagContent().append("HMDB61185");
  db->lastTagContent().append("HMDB61196");


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
