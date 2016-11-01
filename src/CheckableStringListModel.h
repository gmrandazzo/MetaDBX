#ifndef CHECKABLESTRINGLISTMODEL_H
#define CHECKABLESTRINGLISTMODEL_H

#include <QStringListModel>
#include <QtGui>

#include <iostream>

class CheckableStringListModel : public QStringListModel
{

public:
  void setCheckState(int indx, Qt::CheckState cstate){
    if(indx > CheckedStatus.size()){
      return;
    }
    else{
      CheckedStatus[indx] = cstate;
      emit endResetModel();
    }
  }

  void setStrList(const QStringList &strings){
    emit beginResetModel();
    CheckedStatus.resize(strings.size());
    for(int i = 0; i < CheckedStatus.size(); ++i){
      CheckedStatus[i] = Qt::Unchecked;
    }
    setStringList(strings);
    emit endResetModel();
  }

private:
  QVector<Qt::CheckState> CheckedStatus;

  Qt::ItemFlags flags(const QModelIndex& index) const{
    // If the index is not valid
    if (index.row() < 0 || index.row() >= rowCount()){
      return Qt::NoItemFlags;
    }
    return Qt::ItemIsEditable | Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsUserCheckable;
  }

  QVariant data (const QModelIndex  &index , int role ) const {
    // From QStringListModel
    if (index.row() < 0 || index.row() >= rowCount()){
      return QVariant();
    }

    if (role == Qt::DisplayRole || role == Qt::EditRole){
      return stringList().at(index.row());
    }

    if(role == Qt::CheckStateRole){
      return CheckedStatus[index.row()];
    }

    return QVariant();
  }

  bool setData (const QModelIndex &index, const QVariant &value, int role){
    // From QStringListModel
    if (index.row() < 0 || index.row() >= rowCount()){
      return false;
    }

    if(role == Qt::EditRole || role == Qt::DisplayRole){
      stringList().replace(index.row(), value.toString());
      emit dataChanged(index, index);
      return true;
    }

    if(role == Qt::CheckStateRole){
      //CheckedStatus[index.row()] = value.value<Qt::CheckState>();
      CheckedStatus[index.row()] = static_cast<Qt::CheckState>(value.toUInt());

      emit dataChanged(index, index);
    }
    return true;
  }
};

#endif
