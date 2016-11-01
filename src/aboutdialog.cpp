#include "aboutdialog.h"
#include <QString>

AboutDialog::AboutDialog(QString text, int major, int minor, int patch)
{
  ui.setupUi(this);
  ui.label_text->setText(text);
  ui.label_version->setText(QString("Version: %1.%2.%3").arg(QString::number(major)).arg(QString::number(minor)).arg(QString::number(patch)));
  connect(ui.closeButton, SIGNAL(clicked(bool)), SLOT(accept()));
}
