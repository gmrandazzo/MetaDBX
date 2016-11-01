#include "metasdfx.h"
#include <QDesktopServices>
#include <QFileDialog>
#include <QFileInfo>
#include <QFile>
#include <QPair>
#include <QTextStream>
#include <QtCore>
#include <QDirIterator>
#include <QMessageBox>
#include <QMenu>
#include <QFileInfo>
// if qt5 #include <QtConcurrent>
//#include <QDebug>

#include "hmdbreader.h"
#include "keggreader.h"
#include "aboutdialog.h"

void MetaSDFX::LoadSDF()
{
  ui.progressBar->show();
  DisableButtons();
  //QFuture<void> future = QtConcurrent::run(this, &MetaSDFX::LoadSDF_Worker);
  //this->FutureWatcher.setFuture(future);
}

void MetaSDFX::LoadHMDBXML_Worker()
{
  QDirIterator it(ui.lineEdit_2->text(), QDirIterator::Subdirectories);
  HMDBReader reader(ui.treeWidget, db);
  while (it.hasNext()){
    QString filexml = it.next();
    if(filexml.contains(".xml") == true){
       reader.readFile(filexml);
    }
    else{
      continue;
    }
  }

  //Sort each node by tag name
  for(int i = 0; i < db->nodes.size(); i++){
    db->nodes[i].sortTag();
  }
}

void MetaSDFX::LoadKEGGCSV_Worker()
{
  KEGGReader reader(ui.treeWidget, db);
  reader.readFile(ui.lineEdit_2->text());
  //Sort each node by tag name
  for(int i = 0; i < db->nodes.size(); i++){
    db->nodes[i].sortTag();
  }
}

void MetaSDFX::LoadDBFields()
{
  if(ui.dbtype->currentIndex() == 0){
    ui.progressBar->show();
    DisableButtons();
    QFuture<void> future = QtConcurrent::run(this, &MetaSDFX::LoadHMDBXML_Worker);
    this->FutureWatcher.setFuture(future);
  }
  else{
    ui.progressBar->show();
    DisableButtons();
    QFuture<void> future = QtConcurrent::run(this, &MetaSDFX::LoadKEGGCSV_Worker);
    this->FutureWatcher.setFuture(future);
  }
}

void MetaSDFX::Enablebuttons(){
  ui.openSDFButton->setEnabled(true);
  ui.openMetabocardButton->setEnabled(true);
  ui.saveSDFButton->setEnabled(true);
  ui.extractButton->setEnabled(true);
}

void MetaSDFX::DisableButtons()
{
  ui.openSDFButton->setEnabled(false);
  ui.openMetabocardButton->setEnabled(false);
  ui.saveSDFButton->setEnabled(false);
  ui.extractButton->setEnabled(false);
}

void MetaSDFX::OpenSDF()
{
  QString sdf = QFileDialog::getOpenFileName(this,
                                             tr("Open File"),
                                             QDir::currentPath(),
                                             tr("SDF Files (*.sdf)"),
                                             0,
                                             QFileDialog::DontUseNativeDialog);
  ui.lineEdit->setText(sdf);
}

void MetaSDFX::OpenDBFile()
{
  // clear the previous data
  db_content_model->removeRows(0, db_content_model->rowCount());
  sdf_molecule_model->removeRows(0, sdf_molecule_model->rowCount());
  db->clear();
  QAbstractItemModel *model = ui.treeWidget->model();
  model->removeRows(0, model->rowCount());


  if(ui.dbtype->currentIndex() == 0){
    // HMDB
    QString xmldir = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                                                    QDir::currentPath(),
                                                    QFileDialog::ShowDirsOnly
                                                    | QFileDialog::DontResolveSymlinks);
    ui.lineEdit_2->setText(xmldir);
  }
  else{
    // KEGG CSV
    QString csvfile = QFileDialog::getOpenFileName(this,
                                               tr("Open File"),
                                               QDir::currentPath(),
                                               tr("KEGG File (*.kegg)"),
                                               0,
                                               QFileDialog::DontUseNativeDialog);

    ui.lineEdit_2->setText(csvfile);
  }
}
void MetaSDFX::SaveSDF()
{
  QString fileName = QFileDialog::getSaveFileName(this, tr("Save SDF"), "", tr("SDF (*.sdf)"));
  if(!fileName.isEmpty()){
    ui.lineEdit_3->setText(fileName);
  }
  else{
    return;
  }
}


void MetaSDFX::Update_n_selected_molecules()
{
  ui.selmol->setValue(ui.listView->selectionModel()->selectedRows().size());
}


void MetaSDFX::Update_xml_contents(QTreeWidgetItem *item, int column)
{
  QString content = item->data(column, Qt::DisplayRole).toString();
  root_content_id = db->rootnames.indexOf(content);
  if(root_content_id > -1){
    db_content_model->setStringList(db->nodes[root_content_id].tags);
  }
  else{
    return;
  }
}

void MetaSDFX::Update_sdf_molname(const QItemSelection selected, const QItemSelection deselected)
{
  Q_UNUSED(selected);
  Q_UNUSED(deselected);
  if(root_content_id > -1){
    QModelIndexList indexes = ui.listView_2->selectionModel()->selectedIndexes(); //selected.indexes();
    QStringList names;
    if(ui.showmolname->isChecked()){
      for(int i = 0; i < indexes.size(); i++){
        QList<CONTENT> lst = db->nodes[root_content_id].content[indexes[i].row()].list;
        for(int j = 0; j < lst.size(); j++)
          names << QString("%1 | %2").arg(lst[j].molid).arg(lst[j].name);
      }
    }
    else{
      for(int i = 0; i < indexes.size(); i++){
        QList<CONTENT> lst = db->nodes[root_content_id].content[indexes[i].row()].list;
        for(int j = 0; j < lst.size(); j++)
          names << QString("%1").arg(lst[j].molid);
      }
    }
    names.removeDuplicates();
    sdf_molecule_model->setStringList(names);
    ui.totalmol->setValue(ui.listView->model()->rowCount());
    ui.selmol->setValue(0);
    /*int row = current.row();
    if(row > -1 && row < tagcontent.size()){
      sdf_molecule_model->setStringList(tagcontent[row]);
      ui.totalmol->setValue(ui.listView->model()->rowCount());
    }
    else{
      return;
    }
    */
  }
  else{
    return;
  }

}

void MetaSDFX::SelectAll_Molecule()
{
   ui.listView->selectAll();
}

void MetaSDFX::InvertSelection_Molecule()
{
  QItemSelectionModel *selectionModel =  ui.listView->selectionModel();
  QAbstractItemModel *model = ui.listView->model();

  QModelIndex topLeft;
  QModelIndex bottomRight;
  QItemSelection toggleSelection;
  topLeft = model->index(0, 0);
  bottomRight = model->index(model->rowCount()-1, 0);
  toggleSelection.select(topLeft, bottomRight);
  selectionModel->select(toggleSelection, QItemSelectionModel::Toggle);
}

void MetaSDFX::Selectbylist_Molecule()
{
  QString txtlst = QFileDialog::getOpenFileName(this,
                                             tr("Open File"),
                                             QDir::currentPath(),
                                             tr("TXT Files (*.txt)"),
                                             0,
                                             QFileDialog::DontUseNativeDialog);

  QFile file(txtlst);
  if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
    return;
  QStringList molnames;
  QTextStream in(&file);
  while(!in.atEnd()){
    QString line = in.readLine();
    line = line.trimmed();
    if(line.isEmpty()){ // skip line
      continue;
    }
    else{
     molnames << line;
    }
  }
  file.close();

  QItemSelectionModel *selectionModel =  ui.listView->selectionModel();
  QAbstractItemModel* model = ui.listView->model() ;
  for(int i = 0; i < model->rowCount(); i++){
    QString molname = model->index(i, 0).data( Qt::DisplayRole ).toString();
    if(molnames.contains(molname) == true){
      selectionModel->select(model->index(i, 0), QItemSelectionModel::Select);
    }
    else{
      continue;
    }
  }
}

void MetaSDFX::Unselect_Molecule()
{
  ui.listView->selectionModel()->clear();
}

void MetaSDFX::WriteSDF(QStringList sdf, QString outfile)
{

  QFile file(outfile);
  if(file.open(QIODevice::Append)){
    QTextStream stream(&file);
    for(int i = 0; i < sdf.size(); i++)
      stream << sdf[i] << endl;
    file.close();
  }
}

bool MetaSDFX::CheckErrors()
{
  bool return_value = true;
  if(ui.lineEdit->text().size() == 0){
    QMessageBox::warning(this, tr("Warning!"), tr("No SDF input file found!\nPlease open the SDF input file.\n"), QMessageBox::Close);
    return_value = false;
  }

  if(ui.lineEdit_2->text().size() == 0){
    QMessageBox::warning(this, tr("Warning!"), tr("No XML directory found!\nPlease select the XML metabocard directory.\n"), QMessageBox::Close);
    return_value = false;
  }

  if(ui.lineEdit_3->text().size() == 0){
    QMessageBox::warning(this, tr("Warning!"), tr("No SDF file to save!\nPlease select the file SDF to be saved!\n"), QMessageBox::Close);
    return_value = false;
  }

  if(ui.listView->selectionModel()->selectedIndexes().size() == 0){
    QMessageBox::warning(this, tr("Warning!"), tr("No molecule selected!\n"), QMessageBox::Close);
    return_value = false;
  }

  return return_value;
}

void MetaSDFX::Extract_Worker()
{
  // open file sdf
  // scan and if object in list then add to line
  QString outfile = ui.lineEdit_3->text();

  QModelIndexList list = ui.listView->selectionModel()->selectedIndexes();
  QStringList slist;
  foreach(const QModelIndex &index, list){
    slist.append(index.data(Qt::DisplayRole ).toString().split("|")[0].trimmed());
  }

  bool next_is_name = false;
  bool write_sdf = false;
  QFile file(ui.lineEdit->text());
  if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
    return;
  QTextStream in(&file);
  QString field_name;
  QStringList field_name_list;
  QStringList single_sdf;
  while(!in.atEnd()){
    QString line = in.readLine();
    single_sdf.append(line);
    line = line.trimmed();
    if(line.isEmpty()){ // skip line
      continue;
    }
    else{
      if(line.contains("> <DATABASE_ID>") == true){
        next_is_name = true;
      }
      else if(line.contains("$$$$") == true){
        if(write_sdf == true){
          WriteSDF(single_sdf, outfile);
          write_sdf = false;
        }
        single_sdf.clear();
      }
      else{
        if(next_is_name == true){
          //int indx = db->sdf_field_index(field_name);
          QString value = line.trimmed();
          next_is_name = false;
          int indx = slist.indexOf(value);
          if(indx > -1){
            write_sdf = true;
            slist.removeAt(indx);
            if(slist.size() == 0)
              break;
          }
        }
        else{
          continue;
        }
      }
    }
  }
  file.close();
}

void MetaSDFX::Extract()
{
  if(CheckErrors() == false)
    return;

  QFileInfo checkFile(ui.lineEdit_3->text());
  bool run_extraction = false;
  if(checkFile.exists() && checkFile.isFile()){
    QMessageBox::StandardButton reply;
    reply = QMessageBox::warning(this, tr("Warning!"), tr("The file it exist already!\nAre you sure to replace the SDF file?\n"), QMessageBox::Yes|QMessageBox::No);
    if(reply == QMessageBox::Yes){
      run_extraction = true;
    }
    else{
      run_extraction = false;
    }
  }
  else
    run_extraction = true;


  if(run_extraction == true){
    ui.progressBar->show();
    DisableButtons();
    QFuture<void> future = QtConcurrent::run(this, &MetaSDFX::Extract_Worker);
    this->FutureWatcher.setFuture(future);
  }
  else
    return;

}

void MetaSDFX::About()
{
  AboutDialog about("Meta-SDFX", 1,3,0);
  about.exec();
}

void MetaSDFX::Quit()
{
  QCoreApplication::instance()->exit();
}


void MetaSDFX::slot_finished()
{
  this->ui.progressBar->hide();
  Enablebuttons();
}

void MetaSDFX::OpenOnlineTheMetaboCard()
{
  QString path = "http://www.hmdb.ca/metabolites/";
  QModelIndexList indexes = ui.listView->selectionModel()->selectedIndexes();
  bool show = true;
  if(indexes.size() > 100){
    QMessageBox::StandardButton reply;
    reply = QMessageBox::warning(this, tr("Warning!"),
            QString("Are you sure to open %1 metabo cards?\n").arg(indexes.size()),
            QMessageBox::Yes|QMessageBox::No);
    if(reply == QMessageBox::Yes){
      show = true;
    }
    else{
      show = false;
    }
  }
  else{
    show = true;
  }

  if(show == true){
    for(int i = 0; i < indexes.size(); i++){
      QString cardname = indexes[i].data(Qt::DisplayRole).toString().split("|")[0].trimmed();
      QDesktopServices::openUrl(QUrl::fromLocalFile(QString("%1/%2").arg(path).arg(cardname)));
    }
  }
  else
    return;
}

void MetaSDFX::ViewEditMetaboCard()
{
  QString path = ui.lineEdit_2->text();
  QModelIndexList indexes = ui.listView->selectionModel()->selectedIndexes();
  bool show = true;
  if(indexes.size() > 100){
    QMessageBox::StandardButton reply;
    reply = QMessageBox::warning(this, tr("Warning!"),
            QString("Are you sure to open %1 metabo cards?\n").arg(indexes.size()),
            QMessageBox::Yes|QMessageBox::No);
    if(reply == QMessageBox::Yes){
      show = true;
    }
    else{
      show = false;
    }
  }
  else{
    show = true;
  }

  if(show == true){
    for(int i = 0; i < indexes.size(); i++){
      QString cardname = indexes[i].data(Qt::DisplayRole).toString().split("|")[0].trimmed()+".xml";
      QDesktopServices::openUrl(QUrl::fromLocalFile(QString("%1/%2").arg(path).arg(cardname)));
    }
  }
  else
    return;
}

void MetaSDFX::SaveMoleculeList()
{
  QString fileName = QFileDialog::getSaveFileName(this, tr("Save list TXT"), "", tr("TXT (*.txt)"));
  if(!fileName.isEmpty()){
    QFile file(fileName);
    if(file.open(QIODevice::Append)){
      QTextStream stream(&file);
      QModelIndexList indexes = ui.listView->selectionModel()->selectedIndexes();
      for(int i = 0; i < indexes.size(); i++){
        stream << indexes[i].data(Qt::DisplayRole).toString() << endl;
      }
      file.close();
    }
  }
  else{
    return;
  }
}

void MetaSDFX::ContextMenuMetaboCard(const QPoint &pos)
{
  QPoint item = ui.listView->mapToGlobal(pos);
  QAction *OpenMetCardWeb = 0,
          *ViewEditMetCardAct = 0,
          *SaveListAct = 0;

  if(ui.dbtype->currentIndex() == 0){
    OpenMetCardWeb = new QAction(tr("&Open On-Line the metabocard"), this);
    //ViewEditMetCardAct->setShortcuts(QKeySequence::Edit);
    OpenMetCardWeb->setStatusTip(tr("Open On-line the current metabocard"));
    connect(OpenMetCardWeb, SIGNAL(triggered()), this, SLOT(OpenOnlineTheMetaboCard()));


    ViewEditMetCardAct = new QAction(tr("&View/Edit Metabocard"), this);
    //ViewEditMetCardAct->setShortcuts(QKeySequence::Edit);
    ViewEditMetCardAct->setStatusTip(tr("Open the current metabocard with a text editor"));
    connect(ViewEditMetCardAct, SIGNAL(triggered()), this, SLOT(ViewEditMetaboCard()));
  }


  SaveListAct = new QAction(tr("&Save molecule name list As.."), this);
  //SaveListAct->setShortcuts(QKeySequence::SaveAs);
  SaveListAct->setStatusTip(tr("Save the current molecule list"));
  connect(SaveListAct, SIGNAL(triggered()), this, SLOT(SaveMoleculeList()));

  QMenu menu(this);
  menu.addAction(OpenMetCardWeb);
  menu.addAction(ViewEditMetCardAct);
  menu.addAction(SaveListAct);

  menu.exec(item);
}

MetaSDFX::MetaSDFX(QWidget *parent)
{
  ui.setupUi(this);

  db = new Tree();

  ui.progressBar->hide();
  ui.progressBar->setRange(0, 0);
  connect(&this->FutureWatcher, SIGNAL (finished()), this, SLOT (slot_finished()));

  sdf_molecule_model = new QStringListModel();
  db_content_model = new QStringListModel();

  ui.listView->setModel(sdf_molecule_model);
  ui.listView_2->setModel(db_content_model);

  connect(ui.openSDFButton, SIGNAL(clicked()), SLOT(OpenSDF()));
  connect(ui.openMetabocardButton, SIGNAL(clicked()), SLOT(OpenDBFile()));
  connect(ui.saveSDFButton, SIGNAL(clicked()), SLOT(SaveSDF()));

  connect(ui.treeWidget, SIGNAL(itemClicked(QTreeWidgetItem *, int)), SLOT(Update_xml_contents(QTreeWidgetItem *, int)));
  connect(ui.listView->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)),SLOT(Update_n_selected_molecules()));
  ui.listView->setContextMenuPolicy(Qt::CustomContextMenu);
  connect(ui.listView, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(ContextMenuMetaboCard(const QPoint &)));
  connect(ui.listView_2->selectionModel(), SIGNAL(selectionChanged(const QItemSelection, const QItemSelection)), SLOT(Update_sdf_molname(QItemSelection, QItemSelection)));
  connect(ui.selectAllButton, SIGNAL(clicked()), SLOT(SelectAll_Molecule()));
  connect(ui.invertButton, SIGNAL(clicked()), SLOT(InvertSelection_Molecule()));
  connect(ui.selectbylistButton, SIGNAL(clicked()), SLOT(Selectbylist_Molecule()));
  connect(ui.unselectButton, SIGNAL(clicked()), SLOT(Unselect_Molecule()));

  connect(ui.extractButton, SIGNAL(clicked()), SLOT(Extract()));
  connect(ui.aboutButton, SIGNAL(clicked()), SLOT(About()));
  connect(ui.quitButton, SIGNAL(clicked()), SLOT(Quit()));



  //connect(ui.lineEdit, SIGNAL(textChanged(const QString &)), SLOT(LoadSDF()));
  connect(ui.lineEdit_2, SIGNAL(textChanged(const QString &)), SLOT(LoadDBFields()));
}

MetaSDFX::~MetaSDFX()
{
  delete db;
  delete sdf_molecule_model;
  delete db_content_model;
}
