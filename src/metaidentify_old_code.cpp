oid MetaIdentify::Identify_Worker()
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

  if(ui.rtBox->isChecked()){
    double tr_err = ui.rterrorSpinBox->value();
    double flux = ui.fluxSpinBox->value();
    double vd = ui.dwellSpinBox->value();
    double vm = flux * ui.t0SpinBox->value();
    double init_B = ui.gstartSpinBox->value();
    double final_B = ui.gstopSpinBox->value();
    double tg = ui.gtimeSpinBox->value();
    qDebug() << mserror << tr_err << flux << vd << vm << init_B << final_B << tg;


    QFile file(ui.lineEdit->text());
    if(!file.open(QIODevice::ReadOnly)) {
        QMessageBox::information(0, "error", file.errorString());
        return;
    }

    QTextStream in(&file);
    while(!in.atEnd()) {
      QString line = in.readLine();
      QStringList v = line.split(";");
      if(v.size() == 2){
        // iterate for each adduct!!!
        for(int k = 0; k < ext_adducts.size(); k++){
          double ms = v[0].toDouble();
          double tr = v[1].toDouble();
          //qDebug() << QString("MS %1 error %2 add %3; tR: %4 error: %5 init: %6 final: %7 tg: %8 flux: %9 vm: %10 vd: %11").arg(ms).arg(mserror).arg(ext_adducts[k].mass).arg(tr).arg(tr_err).arg(init_B).arg(final_B).arg(tg).arg(flux).arg(vm).arg(vd);
          std::vector<std::string> r = db->find(QString("MS %1 error %2 add %3; tR: %4 error: %5 init: %6 final: %7 tg: %8 flux: %9 vm: %10 vd: %11").arg(ms).arg(mserror).arg(ext_adducts[k].mass).arg(tr).arg(tr_err).arg(init_B).arg(final_B).arg(tg).arg(flux).arg(vm).arg(vd).toStdString());
          /*tabmodel->insertRows(0, 1);
          tabmodel->insertColumns(0, r.size()+1);
          QModelIndex index = tabmodel->index(0, 0);
          tabmodel->setData(index, QVariant(QString("MS%1 as %2").arg(ms).arg(ext_adducts[k].name)));*/
          QList<QStandardItem *> row;
          row << new QStandardItem(QString("MS %1 as %2 tr: %3 min").arg(v[0]).arg(ext_adducts[k].name).arg(v[1]));
          for(int i = 0; i < (int)r.size(); i++){
            QStringList v = QString(QString::fromStdString(r[i])) .split(";");

            /*QModelIndex index = tabmodel->index(0, i+1);
            tabmodel->setData(index, QVariant(QString("Name: %1 \n MS: %2 \n MS Error: %3 \n tr: %4").arg(v[0]).arg(v[1]).arg(v[2]).arg(v[3])));*/
            QString row_ = QString("%1 \n%2 %3 ppm \n%4  %5 %\n").arg(v[0]).arg(v[1]).arg(v[2]).arg(v[3]).arg(v[4]);
            if(5 < v.size()){
              for(int j = 5; j < v.size(); j++)
                row_ += QString("%1\n").arg(v[j]);
            }
            row << new QStandardItem(row_);
            //row.last()->setIcon(QIcon(QPixmap("imagepath")));
          }
          if(row.size() > 1)
            tabmodel->appendRow(row);
        }
      }
    }
    file.close();
  }
  else{

    QFile file(ui.lineEdit->text());
    if(!file.open(QIODevice::ReadOnly)) {
        QMessageBox::information(0, "error", file.errorString());
    }

    QTextStream in(&file);
    while(!in.atEnd()) {
      QString line = in.readLine();
      QStringList v = line.split(";");
      for(int k = 0; k < ext_adducts.size(); k++){
        double ms = v[0].toDouble();
        //qDebug() << QString("MS %1 error %2 add %3").arg(ms).arg(mserror).arg(ext_adducts[k].mass);

        std::vector<std::string> r = db->find(QString("MS %1 error %2 add %3").arg(v[0]).arg(mserror).arg(ext_adducts[k].mass).toStdString());
        /*
        tabmodel->insertRows(0, 1);
        tabmodel->insertColumns(0, r.size()+1);
        QModelIndex index = tabmodel->index(0, 0);
        tabmodel->setData(index, QVariant(QString("MS%1\n%2").arg(ms).arg(ext_adducts[k].name)));
        */
        QList<QStandardItem *> row;
        row << new QStandardItem(QString("MS %1 as %2").arg(ms).arg(ext_adducts[k].name));
        for(int i = 0; i < (int)r.size(); i++){
          QStringList v = QString(QString::fromStdString(r[i])) .split(";");
          //QModelIndex index = tabmodel->index(0, i+1);
          //tabmodel->setData(index, QVariant(QString("Name: %1 \n MS: %2 \n MS Error: %3").arg(v[0]).arg(v[1]).arg(v[2])));
          QString row_ = QString("%1 \n%2 %3 ppm \n").arg(v[0]).arg(v[1]).arg(v[2]);
          if(3 < v.size()){
            for(int j = 3; j < v.size(); j++)
              row_ += QString("%1\n").arg(v[j]);
          }
          row << new QStandardItem(row_);

          //row << new QStandardItem(QString(" Name: %1 \n MS: %2  error: %3 ppm").arg(v[0]).arg(v[1]).arg(v[2]));
            //row.last()->setIcon(QIcon(QPixmap("imagepath")));
        }
        if(row.size() > 1)
          tabmodel->appendRow(row);
      }
    }
    file.close();
  }
}
