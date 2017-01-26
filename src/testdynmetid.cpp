#include <iostream>
#include <string>
#include "progenesis_isotope_info.h"
#include <QTextStream>

QTextStream& qStdOut()
{
    static QTextStream ts( stdout );
    return ts;
}

int main(int argc, char **argv)
{
  ProgenesisIsotopeInfo piinfo(argv[1]);

  QStringList header = piinfo.getHeader();
  QList<QStringList> table = piinfo.getTable();

  for(int j = 0; j < header.size()-1; j++){
    printf("%s;", header[j].toStdString().c_str());
  }
  printf("%s\n", header.last().toStdString().c_str());

  for(int i = 0; i < table.size(); i++){
    for(int j = 0; j < table[i].size()-1; j++){
      printf("%s;", table[i][j].toStdString().c_str());
    }
    printf("%s\n", table[i].last().toStdString().c_str());
  }

  return 0;
}
