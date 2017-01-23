#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "filehashcalculatorthread.h"
#include "founditemwebwidget.h"
#include "founditemwidget.h"

#include <QCheckBox>
#include <QDebug>
#include <QFileDialog>
#include <QGraphicsView>
#include <QGridLayout>
#include <QInputDialog>
#include <QLayoutItem>
#include <QMainWindow>
#include <QMessageBox>
#include <QMetaObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QTableWidget>
#include <QThread>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
  Q_OBJECT

 public:
  enum DatabaseSavingMode { SaveToMemory, SaveToFile };
  enum ResultTableWidgetHeaders {
    Checkbox = 0,
    Group = 1,
    FileSize = 2,
    FileSizeMixed = 3,
    FileName = 4,
    Ext = 5,
    CreateDate = 6,
    ModifedDate = 7,
    FullPath = 8
  };

  explicit MainWindow(QWidget *parent = 0);
  ~MainWindow();

 private slots:
  void on_actionExit_triggered();
  void on_startSearchButton_clicked();
  void on_stopSearchButton_clicked();
  void correctFinishThread();
  void on_actionSave_To_File_changed();
  void setTopLabel(const QString &s);
  void setBottomLabel(const QString &s);
  void setProgressbarMaximumValue(int val);
  void setProgressbarValue(int val);
  void on_tableWidget_itemSelectionChanged();
  void on_searchLocationClearAll_clicked();
  void on_searchLocationRemovePathButton_clicked();
  void on_searchLocationAddPathButton_clicked();
  void cellClicked(int row, int column);

  void on_pushButton_3_clicked();

  void on_actionClearSelection_triggered();

  void on_actionSelectByPath_triggered();

 private:
  Ui::MainWindow *ui;
  QSqlDatabase db;
  FileHashCalculatorThread *hashCalculator = 0;
  void deleteChildWidgets(QLayoutItem *item);
  void remove(QGridLayout *layout, int row, int column, bool deleteWidgets);
  QString sizeFormat(long long val);
};

#endif  // MAINWINDOW_H
