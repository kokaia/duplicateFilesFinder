#include "previewwidgetimage.h"
#include "ui_previewwidgetimage.h"

PreviewWidgetImage::PreviewWidgetImage(const QString& file_path, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PreviewWidgetImage)
{
    qDebug() <<  QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") << "Call PreviewWidgetImage constructor";
    ui->setupUi(this);
    QPixmap pix(file_path);
    QGraphicsScene* scene = new QGraphicsScene();
    QGraphicsPixmapItem* item = new QGraphicsPixmapItem(pix);
    scene->clear();
    scene->addItem(item);
    ui->graphicsView->setRenderHint(QPainter::Antialiasing);
    // scene->addPixmap(pix);
    ui->graphicsView->setScene(scene);
    QRectF r = scene->itemsBoundingRect();
    r.setHeight(r.height() / 2);
    r.setWidth(r.width() / 2);
    ui->graphicsView->fitInView(r, Qt::KeepAspectRatioByExpanding);
    QFileInfo f(file_path);
    ui->label->setText(f.fileName());
}

PreviewWidgetImage::~PreviewWidgetImage()
{
    qDebug() <<  QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") << "Call PreviewWidgetImage destructor";
    qDeleteAll(ui->graphicsView->scene()->items());
    delete ui->graphicsView->scene();
    delete ui->graphicsView;
    delete ui->label;
    delete ui;
}

void PreviewWidgetImage::mousePressEvent(QMouseEvent* event) {
  if (event->button() == Qt::LeftButton &&
      event->type() == QEvent::MouseButtonDblClick) {
    QGraphicsView* view = new QGraphicsView();
    view->setScene(ui->graphicsView->scene());

    view->setGeometry(QStyle::alignedRect(
        Qt::LeftToRight, Qt::AlignCenter, qApp->desktop()->screen()->size(),
        qApp->desktop()->availableGeometry()));
    // view->fitInView( view->scene()->sceneRect(),
    // Qt::KeepAspectRatioByExpanding);
    view->show();


  }
}
