#ifndef DATABASE_H
#define DATABASE_H

#include <QString>
#include <QStringList>
#include <QPair>

// database
class CONTENT
{
public:
  //operator to be used with qSort();
  bool operator<(const CONTENT& other) const {
    return molid < other.molid; // sort by hmdbid
  }
  //operator to be used with indexOf();
  bool operator==(const CONTENT& other) const {
    if(molid.compare(other.molid) == 0)
      return true;
    else
      return false;
  }
  QString name;
  QString molid;
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
      if(list[mid].molid.compare(hdmbid_) == 0){
        return mid;
      }
      else if(list[from].molid.compare(hdmbid_) == 0){
        return from;
      }
      else if(list[to].molid.compare(hdmbid_) == 0){
        return to;
      }
      else{
        BinarySearchHMDBID(from, mid, hdmbid_);
        BinarySearchHMDBID(mid+1, to, hdmbid_);
      }
    }
    return -1;
  }
  int GetIndexOfByMOLID(QString str){
    CONTENT c;
    c.molid = str;
    return list.indexOf(c);
    //return BinarySearchHMDBID(0, list.size()-1, str);
  }
  void append(QString hmdbid_, QString name_){
    list.append(CONTENT());
    list.last().molid = hmdbid_;
    list.last().name = name_;
  }
  void append(QString hmdbid_){
    list.append(CONTENT());
    list.last().molid = hmdbid_;
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

    QStringList tags_;
    QList<LISTCONTENT> content_;
    for(int i = 0; i < taglst.size(); i++){
      tags_.append(tags[taglst[i].second]);
      content_.append(content[taglst[i].second]);
    }
    tags = tags_;
    content = content_;

    /*for(int i = 0; i < taglst.size(); i++){
      LISTCONTENT tmpcontent = content[i];
      QString tmptag = tags[i];
      content[i] = content[taglst[i].second];
      tags[i] = tags[taglst[i].second];
      content[taglst[i].second] = tmpcontent;
      tags[taglst[i].second] = tmptag;
    }*/
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
  void sortNodes(){

  }
  void clear(){
    for(int i = 0; i < nodes.size(); i++)
      nodes[i].clear();
    nodes.clear();
    rootnames.clear();
  }
  QStringList rootnames;
  QList<NODE> nodes;

private:
  static bool caseInsensitiveLessThan(const QPair<QString, int>& e1, const QPair<QString, int>& e2){
    return e1.first < e2.first;
  }
};
//End database

#endif
