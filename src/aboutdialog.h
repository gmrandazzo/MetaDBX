#ifndef ABOUTDIALOG_H
#define ABOUTDIALOG_H

#include <QDialog>
#include "ui_aboutdialog.h"

class AboutDialog: public QDialog
{
  Q_OBJECT

public:
  AboutDialog(QString text, int major, int minor, int patch);

private:
  Ui::AboutDialog ui;
};

#endif
