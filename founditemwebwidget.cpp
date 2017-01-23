#include "founditemwebwidget.h"
#include "ui_founditemwebwidget.h"

FoundItemWebWidget::FoundItemWebWidget(const QString& file_path,
                                       QWidget* parent)
    : QWidget(parent), ui(new Ui::FoundItemWebWidget) {
  ui->setupUi(this);
  QUrl url(file_path);
  ui->webView->load(url);
  QFileInfo f(file_path);
  ui->label->setText(f.fileName());
}

FoundItemWebWidget::~FoundItemWebWidget() { delete ui; }
