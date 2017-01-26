#include "progenesis_isotope_info.h"
#include <QFile>
#include <QList>
#include <QStringList>
#include <QTextStream>

#include <QDebug>

void ProgenesisIsotopeInfo::readFile(const QString &fileName)
{
  QFile file(fileName);
  if (!file.open(QFile::ReadOnly | QFile::Text)){
    //qDebug() << "Error: Cannot read file " << fileName << qPrintable(file.errorString());
    // Error cannot read file fileName
    //std::cerr << "Error: Cannot read file " << qPrintable(fileName)
    //      << ": " << qPrintable(file.errorString()) << std::endl;
      return;
  }

  QTextStream reader(&file);
  QStringList row;
  QString feat_name;
  QString condition_name;
  while (!reader.atEnd()){
    QString line = reader.readLine().trimmed();
    if(line.contains("<?xml", Qt::CaseInsensitive) == true){
      continue;
    }
    else if(line.contains("<compound ", Qt::CaseInsensitive) == true){
      QStringList tmp =  line.split("\"");
      feat_name = tmp[1]; // get feature name
    }
    else if(line.contains("<condition name", Qt::CaseInsensitive) == true){
      QStringList tmp =  line.split("\"");
      condition_name = tmp[1]; // get condition name
    }
    else if(line.contains("<sample name", Qt::CaseInsensitive) == true){
      QStringList tmp =  line.split("\"");
      row << tmp[1] << tmp[3]; // get sample name (1) and abundance (2)
    }
    else if(line.contains("<adduct charge", Qt::CaseInsensitive) == true){
      QStringList tmp =  line.split("\"");
      row << tmp[1]; // get adduct charge
    }
    else if(line.contains("<adduct charge", Qt::CaseInsensitive) == true){
      QStringList tmp =  line.split("\"");
      row << tmp[1]; // get adduct charge
    }
    else if(line.contains("<mz>", Qt::CaseInsensitive) == true){
      row << line.replace("<mz>", "", Qt::CaseInsensitive).replace("</mz>", "", Qt::CaseInsensitive); // get the isotope mass
    }
    else if(line.contains("<abundance>", Qt::CaseInsensitive) == true){
      row << line.replace("<abundance>", "", Qt::CaseInsensitive).replace("</abundance>", "", Qt::CaseInsensitive); // get the isotope abundance
    }
    else if(line.contains("</sample>", Qt::CaseInsensitive) == true){
      if(row.size() > 0){
        QStringList new_row;
        new_row << feat_name << condition_name;
        new_row += row;
        table << new_row;
        row.clear();
      }
    }
  }
  file.close();

  header << "Feature Name" << "Condition Name" << "Sample Name" << "Adduct charge";

  if(table.size() > 0){
    int sz_row = table[0].size();
    for(int i = 1; i < table.size(); i++){
      if(table[i].size() > sz_row){
        sz_row = table[i].size();
      }
      else{
        continue;
      }
    }

    for(int i = 0; i < sz_row-4; i+=2){
      header << "Isotope" << "Abundance";
    }
  }
}

ProgenesisIsotopeInfo::ProgenesisIsotopeInfo(const QString &fileName)
{
  readFile(fileName);
}
