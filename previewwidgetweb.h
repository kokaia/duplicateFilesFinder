#ifndef PREVIEWWIDGETWEB_H
#define PREVIEWWIDGETWEB_H

#include <QWidget>
#include <QFileInfo>
#include <QtWebKitWidgets/QWebView>

namespace Ui {
class PreviewWidgetWeb;
}

class PreviewWidgetWeb : public QWidget
{
    Q_OBJECT

public:
    explicit PreviewWidgetWeb(const QString& file_path, QWidget *parent = 0);
    ~PreviewWidgetWeb();

private:
    Ui::PreviewWidgetWeb *ui;
};

#endif // PREVIEWWIDGETWEB_H
