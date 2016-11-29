#ifndef DYNMETID_H
#define DYNMETID_H

#include <QWidget>
#include <QMessageBox>
#include <QStringListModel>
#include <QStandardItemModel>
#include <QFutureWatcher>

#include "ui_dynmetid.h"
#include "dymetdb.h"
#include "CheckableStringListModel.h"

struct ADDUCT
{
  ADDUCT(QString name_, double mass_)
  {
    name = name_;
    mass = mass_;
  }

  QString name;
  double mass;
};

class DynMetID : public QWidget
{
   Q_OBJECT

public:
  DynMetID(QWidget *parent = 0);
  ~DynMetID();

private slots:
  void OpenMetabList();
  void OpenDB();
  void OpenRTTuning();
  void Identify();
  void About();
  void Quit();
  void ContextMenuTableView(const QPoint pos);
  void slot_finished();
  void exportTabRes();

private:
  void Identify_Worker();
  void DisableButtons();
  void Enablebuttons();
  QList<ADDUCT> getSelectedMass();
  Ui::DynMetID ui;
  QFutureWatcher<void> FutureWatcher;
  QStandardItemModel *tabmodel;
  CheckableStringListModel *lstmodel;
  QList<ADDUCT> adducts;

  DyMetDB *db;
  void DBBuild();
};

#endif
