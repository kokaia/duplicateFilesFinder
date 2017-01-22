#ifndef FOUNDITEMWIDGET_H
#define FOUNDITEMWIDGET_H

#include <QWidget>
#include <QGraphicsPixmapItem>
#include <QFileInfo>
#include <QtDebug>

namespace Ui {
class FoundItemWidget;
}

class FoundItemWidget : public QWidget
{
    Q_OBJECT

public:
    explicit FoundItemWidget(const QString &file_path, QWidget *parent = 0);
    ~FoundItemWidget();

private:
    Ui::FoundItemWidget *ui;
};

#endif // FOUNDITEMWIDGET_H
