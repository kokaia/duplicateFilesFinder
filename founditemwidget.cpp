#include "founditemwidget.h"
#include "ui_founditemwidget.h"

FoundItemWidget::FoundItemWidget(const QString &file_path, QWidget* parent)
    : QWidget(parent), ui(new Ui::FoundItemWidget) {
  ui->setupUi(this);
  qDebug() << file_path;
}

FoundItemWidget::~FoundItemWidget() { delete ui; }


