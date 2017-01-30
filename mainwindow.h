#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "common.h"
#include "filehashcalculatorthread.h"
#include "preview/previewwidgetaudio.h"
#include "preview/previewwidgetimage.h"
#include "preview/previewwidgetvideo.h"
#include "preview/previewwidgetweb.h"
#include "founditemwidget.h"

#include <QCheckBox>
#include <QFileDialog>
#include <QGraphicsView>
#include <QGridLayout>
#include <QInputDialog>
#include <QLayoutItem>
#include <QMainWindow>
#include <QMessageBox>
#include <QMetaObject>
#include <QTableWidget>
#include <QThread>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
  Q_OBJECT

 public:
  enum DatabaseSavingMode { SaveToMemory, SaveToFile };


  explicit MainWindow(QWidget *parent = 0);
  ~MainWindow();

 private slots:
  void on_actionExit_triggered();
  void on_startSearchButton_clicked();
  void on_stopSearchButton_clicked();
  void correctFinishThread();
  void on_tableWidget_itemSelectionChanged();
  void on_actionInvalidate_Files_Hash_Database_triggered();

private:
  Ui::MainWindow *ui;
  FileHashCalculatorThread *hashCalculator = 0;
  void deleteChildWidgets(QLayoutItem *item);
  void remove(QGridLayout *layout, int row, int column, bool deleteWidgets);
  QString sizeFormat(long long val);

};

#endif  // MAINWINDOW_H
