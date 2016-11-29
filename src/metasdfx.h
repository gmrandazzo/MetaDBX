#ifndef METASDFX_H
#define METASDFX_H

#include <QWidget>
#include <QMessageBox>
#include <QStringListModel>
#include <QStandardItemModel>
#include <QFutureWatcher>

#include "ui_metasdfx.h"
#include "database.h"

class MetaSDFX : public QWidget
{
   Q_OBJECT

public:
  MetaSDFX(QWidget *parent = 0);
  ~MetaSDFX();

private slots:
  void OpenSDF();
  void OpenDBFile();
  void SaveSDF();

  void Update_n_selected_molecules();
  void Update_xml_contents(QTreeWidgetItem *item, int column);
  void Update_sdf_molname(const QItemSelection selected, const QItemSelection deselected);

  void SelectAll_Molecule();
  void InvertSelection_Molecule();
  void Selectbylist_Molecule();
  void Unselect_Molecule();

  void Extract();
  void About();
  void Quit();

  void LoadSDF();
  void LoadDBFields();

  void slot_finished();

  void ContextMenuMetaboCard(const QPoint &);
  void OpenOnlineTheMetaboCard();
  void ViewEditMetaboCard();
  void SaveMoleculeList();

private:
  void LoadHMDBXML_Worker();
  void LoadGeneric_Worker();
  bool CheckErrors();
  void Extract_Worker();
  void WriteSDF(QStringList sdf, QString outfile);

  void DisableButtons();
  void Enablebuttons();

  Ui::MetaSDFX ui;
  Tree *db;
  QStringListModel *sdf_molecule_model;
  // <class>content</class>
  QStringListModel *db_content_model;
  QFutureWatcher<void> FutureWatcher;

  int root_content_id;

};

#endif
