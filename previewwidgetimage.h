#ifndef PREVIEWWIDGETIMAGE_H
#define PREVIEWWIDGETIMAGE_H

#include <QWidget>
#include <QDebug>
#include <QFileInfo>
#include <QDateTime>
#include <QGraphicsPixmapItem>
#include <QMouseEvent>
#include <QApplication>
#include <QDesktopWidget>

namespace Ui {
class PreviewWidgetImage;
}

class PreviewWidgetImage : public QWidget
{
    Q_OBJECT

public:
    explicit PreviewWidgetImage(const QString &file_path, QWidget *parent = 0);
    ~PreviewWidgetImage();

protected:
    void mousePressEvent(QMouseEvent *event) override;

private:
    Ui::PreviewWidgetImage *ui;
};

#endif // PREVIEWWIDGETIMAGE_H
