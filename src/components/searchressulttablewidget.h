#ifndef SEARCHRESSULTTABLEWIDGET_H
#define SEARCHRESSULTTABLEWIDGET_H

#include "src/common.h"

#include <QTableWidget>
#include <QDebug>
#include <QKeyEvent>
#include <QMessageBox>
#include <QInputDialog>


class SearchRessultTableWidget : public QTableWidget
{
    Q_OBJECT

public:
    explicit SearchRessultTableWidget(QWidget *parent = nullptr);
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

   bool isAutoCheck;
   void cellClicked(int row, int column);
   void removeAllRows();
   void displayResults();

protected:
  void keyPressEvent(QKeyEvent *event) override;

signals:

public slots:
  void selectItemsByPath();
  void unSelectItems();
  void deleteSelectedItems();

};

#endif // SEARCHRESSULTTABLEWIDGET_H
