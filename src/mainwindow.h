#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLayoutItem>
#include <QGridLayout>
#include "common.h"
#include "filehashcalculatorthread.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
  Q_OBJECT

 public:
  enum DatabaseSavingMode { SaveToMemory, SaveToFile };


  explicit MainWindow(QWidget *parent = nullptr);
  ~MainWindow();

 private slots:
  void updateStartTime();
  void updateApproximateTime(int appTime);
  static void on_actionExit_triggered();
  void on_startSearchButton_clicked();
  void on_stopSearchButton_clicked();
  void correctFinishThread();
  void on_tableWidget_itemSelectionChanged();
  void on_actionInvalidate_Files_Hash_Database_triggered();

private:
  Ui::MainWindow *ui;
  FileHashCalculatorThread *hashCalculator = nullptr;
  QTimer *timerEverySecond;
  quint32 timeSinceStart;
  void deleteChildWidgets(QLayoutItem *item);
  void remove(QGridLayout *layout, int row, int column, bool deleteWidgets);
};

#endif  // MAINWINDOW_H
