// Read the isotope info export from progenesis and annotate compounds
// according their m/z and isotope matching
#ifndef PROGENESIS_ISOTOPE_INFO_H
#define PROGENESIS_ISOTOPE_INFO_H

#include <QList>
#include <QString>
#include <QStringList>

class ProgenesisIsotopeInfo
{
public:
  ProgenesisIsotopeInfo(const QString &fileName);
  QList<QStringList> getTable(){ return table; }
  QStringList getHeader(){ return header; }

private:
  void readFile(const QString &fileName);
  QList<QStringList> table;
  QStringList header;

};

#endif
