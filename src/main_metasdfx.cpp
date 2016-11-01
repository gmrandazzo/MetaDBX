#include <QApplication>
#include <QtCore/QFileInfo>
#include <QtNetwork/QHostInfo>
#include <QDir>
#include <QFile>

#include "metasdfx.h"

int main(int argc, char *argv[])
{
  QApplication app(argc, argv);
  MetaSDFX metasdfx;
  metasdfx.show();
  return app.exec();
}
