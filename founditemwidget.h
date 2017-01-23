#ifndef FOUNDITEMWIDGET_H
#define FOUNDITEMWIDGET_H

#include <QFileInfo>
#include <QGraphicsPixmapItem>
#include <QList>
#include <QSet>
#include <QWidget>
#include <QtDebug>
#include <set>

namespace Ui {
class FoundItemWidget;
}

class FoundItemWidget : public QWidget {
  Q_OBJECT

 public:
  explicit FoundItemWidget(const QString &file_path, QWidget *parent = 0);
  ~FoundItemWidget();

  static const QSet<QString> validExtentions;

 private:
  Ui::FoundItemWidget *ui;
};

#endif  // FOUNDITEMWIDGET_H
