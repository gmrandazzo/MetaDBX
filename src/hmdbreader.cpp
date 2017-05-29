#include "hmdbreader.h"
#include <QFile>

bool HMDBReader::readFile(const QString &fileName)
{
  qDebug() << fileName;
  QFile file(fileName);
  if (!file.open(QFile::ReadOnly | QFile::Text)){
    //qDebug() << "Error: Cannot read file " << fileName << qPrintable(file.errorString());
    // Error cannot read file fileName
    //std::cerr << "Error: Cannot read file " << qPrintable(fileName)
    //      << ": " << qPrintable(file.errorString()) << std::endl;
      return false;
  }
  reader.setDevice(&file);
  reader.readNext();
  while (!reader.atEnd()){
    if(reader.isStartElement()){
      if(reader.name() == "metabolite"){
        readMetabocardindexElement();
      }
      else{
        reader.raiseError(QObject::tr("Not a metabolite file"));
      }
    }
    else{
      reader.readNext();
    }
  }
  file.close();
  if(reader.hasError()){
    //std::cerr << "Error: Failed to parse file "
    //  << qPrintable(fileName) << ": "
    //  << qPrintable(reader.errorString()) << std::endl;
      return false;
  }
  else if(file.error() != QFile::NoError){
    //std::cerr << "Error: Cannot read file " << qPrintable(fileName)
    //  << ": " << qPrintable(file.errorString()) << std::endl;
    return false;
  }
  return true;
}

void HMDBReader::readMetabocardindexElement()
{
  reader.readNext();
  while(!reader.atEnd()){
    if(reader.isEndElement()){
      reader.readNext();
      break;
    }

    if(reader.isStartElement()){
      if(reader.name() == "taxonomy"
        || reader.name() == "ontology"
        || reader.name() == "diseases"
      /*|| reader.name() == "pathway"*/){
        readTagElement(treeWidget->invisibleRootItem());
      }
      else if(reader.name() == "pathways"){
        QTreeWidgetItem *item = GetQTreeWidgetItem("pathways", treeWidget->invisibleRootItem());
        readTagElement(item);
      }
      else if(reader.name() == "accession"){
        hmdbid = reader.readElementText();
        reader.readNext();
      }
      else if(reader.name() == "name"){
        molname = reader.readElementText();
        reader.readNext();
      }
      else if(reader.name() == "metagene"){
        QTreeWidgetItem *item = GetQTreeWidgetItem("metagene", treeWidget->invisibleRootItem());
        readContentElement(item);
        //reader.readNext();
      }
      else if(reader.name() == "metagene"){
      }
      else{
        skipUnknownElement();
      }
    }
    else{
      reader.readNext();
    }
  }
}

void visitTree_(QList<QTreeWidgetItem*> &list, QTreeWidgetItem *item){
  list << item;
  for(int i = 0; i < item->childCount(); i++)
    visitTree_(list, item->child(i));
}

QList<QTreeWidgetItem*> visitTree(QTreeWidget *tree){
  QList<QTreeWidgetItem*> list;
  for(int i = 0; i < tree->topLevelItemCount(); i++)
    visitTree_(list, tree->topLevelItem(i));
  return list;
}

QTreeWidgetItem *HMDBReader::GetQTreeWidgetItem(QString reader_name, QTreeWidgetItem *parent)
{
  QTreeWidgetItem *item;
  QList<QTreeWidgetItem*> all_tree = visitTree(treeWidget);
  int indx = -1;
  for(int i = 0; i < all_tree.size(); i++){
    if(all_tree[i]->text(0).compare(reader_name) == 0){
      indx = i;
      break;
    }
    else{
      continue;
    }
  }
  //found  = treeWidget->findItems(reader.name().toString(), 0);

  if(indx == -1){
    item = new QTreeWidgetItem(parent);
    item->setText(0, reader.name().toString());
  }
  else{
    item = all_tree[indx];
  }
  return item;
}

void HMDBReader::readTagElement(QTreeWidgetItem *parent)
{
  QString reader_name = reader.name().toString();
  QTreeWidgetItem *item = GetQTreeWidgetItem(reader_name, parent);

  reader.readNext();
  while(!reader.atEnd()){
    if(reader.isEndElement()){
      reader.readNext();
      break;
    }

    if(reader.isStartElement()){
      if(reader.name() == "origins"){
        readTagElement(item);
      }
      else if(reader.name() == "disease"){
        readTagElement(item);
      }
      else if(reader.name() == "pathway"){
        readTagElement(item);
      }
      else if(reader.name() == "super_class"
        || reader.name() == "class"
        || reader.name() == "kingdom"
        || reader.name() == "origin"
        || reader.name() == "name"){
        readContentElement(item);
      }
      else{
        skipUnknownElement();
      }
    }
    else{
      reader.readNext();
    }
  }
}

void HMDBReader::readContentElement(QTreeWidgetItem *parent)
{

  //QTreeWidgetItem *item = new QTreeWidgetItem(parent);
  //item->setText(0, reader_name);



  /*  "taxonomy"
    reader.name()  "kingdom"  content  "Organic Compounds"
    reader.name()  "super_class"  content  "Lipids"
    reader.name()  "class"  content  "Glycerophospholipids"
    "ontology"
    reader.name()  "origin"  content  "Endogenous"
    reader.name()  "origin"  content  "Food"
  */

  QString reader_name;
  if(parent->parent() != 0)
    reader_name = parent->text(0);
  else
    reader_name = reader.name().toString();
  QString content = reader.readElementText();

  //qDebug() << "reader.name() " << reader_name << " content " << content;
  int reader_indx = (*db).rootnames.indexOf(reader_name);

  if(reader_name.compare("metagene") == 0){ // START SPECIFIC PART METAGENE
    if(reader_indx == -1){ // root node not found! add!
      (*db).addNode("metagene");
      reader_indx = (*db).nodes.size()-1;
    }

    QStringList contentlst = content.split(";");
    contentlst.replaceInStrings(QRegExp("^\\s*"),"");
    for(int i = 0; i < contentlst.size(); i++){
      int node_indx = (*db).nodes[reader_indx].indexOfTag(contentlst[i]);
      if(node_indx > -1){ // node found
        if((*db).nodes[reader_indx].content[node_indx].GetIndexOfByMOLID(hmdbid) == -1){ // hmdbid not in list
          (*db).nodes[reader_indx].content[node_indx].append(hmdbid, molname);
        }
        else{
          continue;
        }
      }
      else{ // node not found neither hmdbid, name in list
        (*db).nodes[reader_indx].appendTag(contentlst[i]);
        (*db).nodes[reader_indx].lastTagContent().append(hmdbid, molname);
      }
    }
  } // END SPECIFIC PART METAGENE
  else{
    if(reader_indx == -1){
      // if is already in the tree do not add.... alse add as new!
      GetQTreeWidgetItem(reader_name, parent);
      //QTreeWidgetItem *item = new QTreeWidgetItem(parent);
      //item->setText(0, reader_name);
      (*db).addNode(reader_name);
      reader_indx = (*db).nodes.size()-1;
    }
    int node_indx = (*db).nodes[reader_indx].indexOfTag(content);
    if(node_indx > -1){ // content name found
      if((*db).nodes[reader_indx].content[node_indx].GetIndexOfByMOLID(hmdbid) == -1){ // molecule not found
        (*db).nodes[reader_indx].content[node_indx].append(hmdbid, molname);
      }
    }
    else{ // content name not found thus also the molecule
      (*db).nodes[reader_indx].appendTag(content);
      (*db).nodes[reader_indx].lastTagContent().append(hmdbid, molname);
    }
  }

  if (reader.isEndElement())
    reader.readNext();

}

void HMDBReader::skipUnknownElement()
{
  reader.readNext();
  while(!reader.atEnd()){
    if(reader.isEndElement()){
      reader.readNext();
      break;
    }

    if(reader.isStartElement()){
      skipUnknownElement();
    }
    else{
      reader.readNext();
    }
  }
}

HMDBReader::HMDBReader(QTreeWidget *tree, Tree *db_)
{
  treeWidget = tree;
  db = db_;
}
