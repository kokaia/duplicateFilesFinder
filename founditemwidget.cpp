#include "founditemwidget.h"
#include "ui_founditemwidget.h"

FoundItemWidget::FoundItemWidget(const QString& file_path, QWidget* parent)
    : QWidget(parent), ui(new Ui::FoundItemWidget) {
  ui->setupUi(this);
  QPixmap pix(file_path);
  QGraphicsScene* scene = new QGraphicsScene();
  QGraphicsPixmapItem* item = new QGraphicsPixmapItem(pix);
  scene->clear();
  scene->addItem(item);
  // scene->addPixmap(pix);
  ui->graphicsView->setScene(scene);
  QRectF r = scene->itemsBoundingRect();
  r.setHeight(r.height() / 2);
  r.setWidth(r.width() / 2);
  ui->graphicsView->fitInView(r, Qt::KeepAspectRatioByExpanding);
  QFileInfo f(file_path);
  ui->label->setText(f.fileName());
}

FoundItemWidget::~FoundItemWidget() { delete ui; }
