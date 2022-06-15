#ifndef COMMONLISTWIDGET_H
#define COMMONLISTWIDGET_H

#include <QWidget>
#include <QListWidget>
#include <QFileDialog>
#include <QClipboard>

namespace Ui {
class CommonListWidget;
}

class CommonListWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CommonListWidget(QWidget *parent = nullptr);
    ~CommonListWidget();
    QListWidget* getListWidget();

private slots:
    void on_addPathButton_clicked();
    void on_pastePathButton_clicked();

private:
    Ui::CommonListWidget *ui;
};

#endif // COMMONLISTWIDGET_H
