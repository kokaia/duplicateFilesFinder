#ifndef FOUNDITEMWEBWIDGET_H
#define FOUNDITEMWEBWIDGET_H

#include <QFileInfo>
#include <QWidget>
#include <QtWebKitWidgets/QWebView>

namespace Ui {
class FoundItemWebWidget;
}

class FoundItemWebWidget : public QWidget {
  Q_OBJECT

 public:
  explicit FoundItemWebWidget(const QString &file_path, QWidget *parent = 0);
  ~FoundItemWebWidget();

 private:
  Ui::FoundItemWebWidget *ui;
};

#endif  // FOUNDITEMWEBWIDGET_H
