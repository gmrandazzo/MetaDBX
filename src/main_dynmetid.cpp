#include <QApplication>
#include <QtCore/QFileInfo>
#include <QtNetwork/QHostInfo>
#include <QDir>
#include <QFile>

#include "dynmetid.h"

int main(int argc, char *argv[])
{
  QApplication app(argc, argv);
  DynMetID dmid;
  dmid.show();
  return app.exec();
}
