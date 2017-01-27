#ifndef SEARCHRESSULTTABLEWIDGET_H
#define SEARCHRESSULTTABLEWIDGET_H

#include "common.h"

#include <QTableWidget>
#include <QDebug>
#include <QKeyEvent>
#include <QMessageBox>


class SearchRessultTableWidget : public QTableWidget
{
    Q_OBJECT

public:
    explicit SearchRessultTableWidget(QWidget *parent = 0);
    ~SearchRessultTableWidget();

    enum ResultTableWidgetHeaders {
      Checkbox = 0,
      Group = 1,
      FileSize = 2,
      FileSizeMixed = 3,
      FileName = 4,
      Ext = 5,
      CreateDate = 6,
      ModifedDate = 7,
      FileHash = 8,
      FullPath = 9
    };

   void cellClicked(int row, int column);

   void removeAllRows();

protected:
  void keyPressEvent(QKeyEvent *event) override;

signals:

public slots:
 void deleteSelectedItems();

};

#endif // SEARCHRESSULTTABLEWIDGET_H
