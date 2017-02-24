#include "dynmetid.h"

#include <QtCore>
#include <QFileDialog>
#include <QMessageBox>
#include <QMenu>
#include <QtConcurrent>

#include "aboutdialog.h"
#include "dymetdb.h"


void DynMetID::exportTabRes()
{
  QString savefile = QFileDialog::getSaveFileName(this, tr("Save CSV"), "", tr("CSV (*.csv)"));
  if(!savefile.isEmpty()){

    QFile file(savefile);
    if(!file.open(QIODevice::WriteOnly)) {
        QMessageBox::information(0, "error", file.errorString());
        return;
    }

    QTextStream stream(&file);
    for(int i = 0; i < tabmodel->rowCount(); i++){
      QString row_;
      for(int j = 0; j < tabmodel->columnCount()-1; j++){
        if(tabmodel->item(i, j) != 0){
          row_ += tabmodel->item(i, j)->data(Qt::DisplayRole).toString().replace("\n", ",")+";";
        }
      }

      if(tabmodel->item(i, tabmodel->columnCount()-1) != 0){
        row_ += tabmodel->item(i, tabmodel->columnCount()-1)->data(Qt::DisplayRole).toString().replace("\n", ",");
      }

      stream << row_ +"\n";
    }
    file.close();
  }
  else{
    return;
  }
}

//private slots
void DynMetID::OpenMetabList()
{
  QString metabolst = QFileDialog::getOpenFileName(this,
                                             tr("Open File"),
                                             QDir::currentPath(),
                                             tr("TXT Files (*.txt);;CSV Files(*.csv)"),
                                             0,
                                             QFileDialog::DontUseNativeDialog);
  ui.lineEdit->setText(metabolst);
}

void DynMetID::DBBuild()
{
  db->clear();
  // it takes time!!
  db->init(ui.lineEdit_2->text().toStdString());
}

void DynMetID::OpenDB()
{
  QString fdb = QFileDialog::getOpenFileName(this,
                                             tr("Open File"),
                                             QDir::currentPath(),
                                             tr("TXT Files (*.txt);;CSV (*.csv)"),
                                             0,
                                             QFileDialog::DontUseNativeDialog);
  ui.lineEdit_2->setText(fdb);
  DBBuild();

  /*QFuture<void> future = QtConcurrent::run(this, &DynMetID::DBBuild);
  this->FutureWatcher.setFuture(future);*/
}

void DynMetID::OpenRTTuning()
{
  QString trtunf = QFileDialog::getOpenFileName(this,
                                             tr("Open File"),
                                             QDir::currentPath(),
                                             tr("TXT Files (*.txt)"),
                                             0,
                                             QFileDialog::DontUseNativeDialog);
  ui.lineEdit_3->setText(trtunf);
}

void DynMetID::Identify()
{
  if(!ui.lineEdit->text().isEmpty() && !db->isEmpty()){
    QFuture<void> future = QtConcurrent::run(this, &DynMetID::Identify_Worker);
    this->FutureWatcher.setFuture(future);
  }
  else if(!ui.lineEdit->text().isEmpty() && db->isEmpty()){
    QMessageBox::warning(this, tr("Warning!"), tr("Database is empty!\nPlease open a database.\n"), QMessageBox::Close);
  }
  else if(ui.lineEdit->text().isEmpty() && !db->isEmpty()){
    QMessageBox::warning(this, tr("Warning!"), tr("No metabolite list found!\nPlease give a metabolite list as \"ms;tr\" or only \"ms\"\n"), QMessageBox::Close);
  }
  else{
    QMessageBox::warning(this, tr("Warning!"), tr("No metabolite list found!\nPlease give a metabolite list as \"ms;tr\" or only \"ms\"\nDatabase is empty!\nPlease open a database.\n"), QMessageBox::Close);
  }

}

void DynMetID::About()
{
  AboutDialog about("Dynamic Metabolite Identifier", 1,2,0);
  about.exec();
}

void DynMetID::Quit()
{
  QCoreApplication::instance()->exit();
}

void DynMetID::ContextMenuTableView(const QPoint pos)
{
  QAction *exportResultsAction = 0,
          *editResultsAction = 0,
          *removeResultsAction = 0;

  exportResultsAction = new QAction(tr("&Export results "), this);
  //ViewEditMetCardAct->setShortcuts(QKeySequence::Edit);
  exportResultsAction->setStatusTip(tr("Export table results into a file"));
  connect(exportResultsAction, SIGNAL(triggered()), this, SLOT(exportTabRes()));


  editResultsAction = new QAction(tr("&Edit results"), this);
  //ViewEditMetCardAct->setShortcuts(QKeySequence::Edit);
  editResultsAction->setStatusTip(tr("Edit table results"));
  //connect(ViewEditMetCardAct, SIGNAL(triggered()), this, SLOT(ViewEditMetaboCard()));



  removeResultsAction = new QAction(tr("&Remove results"), this);
  //SaveListAct->setShortcuts(QKeySequence::SaveAs);
  removeResultsAction->setStatusTip(tr("Remove results from table"));
  //connect(SaveListAct, SIGNAL(triggered()), this, SLOT(SaveMoleculeList()));

  QMenu menu(this);
  //menu.addAction(editResultsAction);
  //menu.addAction(removeResultsAction);
  menu.addAction(exportResultsAction);

  menu.exec(ui.tableView->viewport()->mapToGlobal(pos));
}

void DynMetID::slot_finished()
{
  this->ui.progressBar->hide();
  Enablebuttons();
}

//private methods
void DynMetID::Identify_Worker()
{
  // open list of rt _at_ metabolite
  // for each metabolite of list
  //     open db and query for exact_mw with a binary search!
  //     if retention time prediction is on refine mass with retention time prediction
  //       append filtered queries of mass founds
  //     else
  //       append all queries of mass founds


  double mserror = ui.masserrorSpinBox->value();

  tabmodel->clear();

  QList<ADDUCT> ext_adducts;
  for(int i = 0; i < lstmodel->rowCount(); i++){
    QModelIndex index = lstmodel->index(i, 0);
    if(index.data(Qt::CheckStateRole) == Qt::Checked){
      ext_adducts.append(adducts[i]);
    }
    else
      continue;
  }

  // Variables used only when if(ui.rtBox->isChecked());
  double tr_err = ui.rterrorSpinBox->value();
  double flux = ui.fluxSpinBox->value();
  double vd = ui.dwellSpinBox->value();
  double vm = flux * ui.t0SpinBox->value();
  double init_B = ui.gstartSpinBox->value();
  double final_B = ui.gstopSpinBox->value();
  double tg = ui.gtimeSpinBox->value();

  //qDebug() << mserror << tr_err << flux << vd << vm << init_B << final_B << tg;

  QFile file(ui.lineEdit->text());
  if(!file.open(QIODevice::ReadOnly)) {
      QMessageBox::information(0, "error", file.errorString());
      return;
  }


  if(ui.rtBox->isChecked()){
    if(!ui.lineEdit_3->text().isEmpty()){
      QString lqline = QString("tR: -1 error: -1 init: %1 final: %2 tg: %3 flux: %4 vm: %5 vd: %6").arg(init_B).arg(final_B).arg(tg).arg(flux).arg(vm).arg(vd);
      db->setRTLinearAligner(ui.lineEdit_3->text().toStdString(), lqline.toStdString());
    }
    else{
      db->setRTLinearAligner(1.f, 0.f);
    }
  }

  QTextStream in(&file);
  while(!in.atEnd()) {
    QString line = in.readLine();
    //qDebug() << QString("Process %1").arg(line);
    //QStringList v1 = line.split(";");
    // Progenesis variable parsing
    QStringList v1 = line.split("_");
    double ms = 0.f;
    double tr = v1[0].toDouble();
    if(v1[1].contains("n")){
      // this is a neutral mass!!
      // then remove also here the adduct
      v1[1].replace("n", "");
      ms = v1[1].toDouble();
    }
    else{
      // is already charged
      v1[1].replace("m/z", "");
      ms = v1[1].toDouble();
    }

    QList<QStandardItem *> row;
    row << new QStandardItem(line);
    std::vector<std::string> allr;
    QList<QString> addtype;
    if(v1.size() == 2){
      // iterate for each adduct!!!
      for(int k = 0; k < ext_adducts.size(); k++){
        std::vector<std::string> r;
        if(ui.rtBox->isChecked()){
          r = db->find(QString("MS %1 error %2 add %3; tR: %4 error: %5 init: %6 final: %7 tg: %8 flux: %9 vm: %10 vd: %11").arg(ms).arg(mserror).arg(ext_adducts[k].mass).arg(tr).arg(tr_err).arg(init_B).arg(final_B).arg(tg).arg(flux).arg(vm).arg(vd).toStdString());
        }
        else{
          r = db->find(QString("MS %1 error %2 add %3").arg(ms).arg(mserror).arg(ext_adducts[k].mass).toStdString());
        }

        if(r.size() > 0){
          if(allr.size() == 0){
            for(int i = 0; i < (int)r.size(); i++){
              allr.push_back(r[i]);
              addtype << QString("%1").arg(ext_adducts[k].name);
            }
          }
          else{
            for(int i = 0; i < (int)r.size(); i++){
              int exists_id = -1;
              for(int j = 0; j < (int)allr.size(); j++){
                if(r[i].compare(allr[j]) == 0){
                  exists_id = i;
                  break;
                }
                else{
                  continue;
                }
              }

              if(exists_id == -1){
                allr.push_back(r[i]);
                addtype << QString("%1").arg(ext_adducts[k].name);
              }
              else{
                addtype[exists_id].append(QString(";%1").arg(ext_adducts[k].name));
              }
            }
          }
        }
      }

      // Finalizing output in one row!
      for(int i = 0; i < (int)allr.size(); i++){
        QStringList v2 = QString(QString::fromStdString(allr[i])) .split(";");
        QString row_;
        if(ui.rtBox->isChecked()){
          row_ = QString("%1 \n%2 %3 ppm \n%4  %5 %\n").arg(v2[0]).arg(v2[1]).arg(v2[2]).arg(v2[3]).arg(v2[4]);
          if(5 < v2.size()){
            for(int j = 5; j < v2.size(); j++)
              row_ += QString("%1\n").arg(v2[j]);
          }
        }
        else{
          row_ = QString("%1 \n%2 %3 ppm \n").arg(v2[0]).arg(v2[1]).arg(v2[2]);
          if(3 < v2.size()){
            for(int j = 3; j < v2.size(); j++)
              row_ += QString("%1\n").arg(v2[j]);
          }
        }

        row_ += QString("as: %1\n").arg(addtype[i]);
        row << new QStandardItem(row_);
      }
    }
    tabmodel->appendRow(row);
  }
  file.close();
}

void DynMetID::DisableButtons()
{
  ui.openMetabolstButton->setEnabled(false);
  ui.openDBButton->setEnabled(false);
  ui.masserrorSpinBox->setEnabled(false);
  ui.rterrorSpinBox->setEnabled(false);
  ui.t0SpinBox->setEnabled(false);
  ui.dwellSpinBox->setEnabled(false);
  ui.gstartSpinBox->setEnabled(false);
  ui.gstopSpinBox->setEnabled(false);
  ui.gtimeSpinBox->setEnabled(false);
  ui.identifyButton->setEnabled(false);
  ui.quitButton->setEnabled(false);
}

void DynMetID::Enablebuttons()
{
  ui.openMetabolstButton->setEnabled(true);
  ui.openDBButton->setEnabled(true);
  ui.masserrorSpinBox->setEnabled(true);
  ui.rterrorSpinBox->setEnabled(true);
  ui.t0SpinBox->setEnabled(true);
  ui.dwellSpinBox->setEnabled(true);
  ui.gstartSpinBox->setEnabled(true);
  ui.gstopSpinBox->setEnabled(true);
  ui.gtimeSpinBox->setEnabled(true);
  ui.identifyButton->setEnabled(true);
  ui.quitButton->setEnabled(true);
}

DynMetID::DynMetID(QWidget *parent)
{
  ui.setupUi(this);
  ui.progressBar->hide();
  ui.progressBar->setRange(0, 0);
  db = new DyMetDB();
  tabmodel = new QStandardItemModel();
  lstmodel = new CheckableStringListModel();
  ui.tableView->setModel(tabmodel);
  ui.listView->setModel(lstmodel);

  double e_mass = 0.0005485670779832; /*Da*/
  /*Positive adducts*/
  adducts.append(ADDUCT("M+", - e_mass));
  adducts.append(ADDUCT("M+H", 1.00782503207 - e_mass));
  adducts.append(ADDUCT("M+2H", 2.01565006414 - e_mass));
  adducts.append(ADDUCT("M+3H", 3.02347509621 - e_mass));
  adducts.append(ADDUCT("M+H2O+H", 19.018389715771598 - e_mass));
  adducts.append(ADDUCT("M+H-H2O", -17.0027396516316 - e_mass));
  adducts.append(ADDUCT("M+H-2H2O", -35.013304335333196 - e_mass));
  adducts.append(ADDUCT("M+Na", 22.989769280929 - e_mass));
  adducts.append(ADDUCT("M+Na+H", 23.997594312999 - e_mass));
  adducts.append(ADDUCT("M+CH3OH+H", 33.0340397799116 - e_mass));
  adducts.append(ADDUCT("M+CH3OH+Na", 55.0159840287706 - e_mass));
  adducts.append(ADDUCT("M+ACN+H", 42.034374133140005 - e_mass));
  adducts.append(ADDUCT("M+ACN+Na", 64.016318381999 - e_mass));

  /*Negative adducts*/
  adducts.append(ADDUCT("M-", -e_mass));
  adducts.append(ADDUCT("M-H", -1.00782503207 + e_mass));
  adducts.append(ADDUCT("M-2H", -2.01565006414 + e_mass));
  adducts.append(ADDUCT("M-3H", -3.02347509621 + e_mass));
  adducts.append(ADDUCT("M-H2O-H", -19.018389715771598 + e_mass));
  adducts.append(ADDUCT("M-H-2H2O", -35.013304335333196 + e_mass));
  adducts.append(ADDUCT("M-Na", -22.989769280929 + e_mass));
  adducts.append(ADDUCT("M-Na-H", -23.997594312999 + e_mass));
  adducts.append(ADDUCT("M-CH3OH-H", -33.0340397799116 + e_mass));
  adducts.append(ADDUCT("M-CH3OH-Na", -55.0159840287706 + e_mass));
  adducts.append(ADDUCT("M-ACN-H", -42.034374133140005 + e_mass));
  adducts.append(ADDUCT("M-ACN-Na", 64.016318381999 + e_mass));

  QStringList lststdmass;
  for(int i = 0; i < adducts.size(); i++)
    lststdmass.append(adducts[i].name);

  lstmodel->setStrList(lststdmass);
  lstmodel->setCheckState(0, Qt::Checked);

  // for future calculations
  connect(&this->FutureWatcher, SIGNAL (finished()), this, SLOT (slot_finished()));

  connect(ui.openMetabolstButton, SIGNAL(clicked()), SLOT(OpenMetabList()));
  connect(ui.openDBButton, SIGNAL(clicked()), SLOT(OpenDB()));
  connect(ui.opentrtuning, SIGNAL(clicked()), SLOT(OpenRTTuning()));


  ui.tableView->horizontalHeader()->setDefaultSectionSize(320);
  ui.tableView->verticalHeader()->setDefaultSectionSize(80);
  ui.tableView->setContextMenuPolicy(Qt::CustomContextMenu);
  //ui.tableView->horizontalHeader()->hide();
  connect(ui.tableView, SIGNAL(customContextMenuRequested(QPoint)), SLOT(ContextMenuTableView(QPoint)));


  connect(ui.identifyButton, SIGNAL(clicked()), SLOT(Identify()));

  connect(ui.aboutButton, SIGNAL(clicked()), SLOT(About()));
  connect(ui.quitButton, SIGNAL(clicked()), SLOT(Quit()));
}

DynMetID::~DynMetID()
{
  delete db;
  delete tabmodel;
  delete lstmodel;
}
