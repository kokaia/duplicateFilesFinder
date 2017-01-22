#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QDebug>
#include <QSqlDatabase>
#include <QMessageBox>
#include <QSqlQuery>
#include <QThread>
#include <QGraphicsView>
#include <QGridLayout>
#include <QLayoutItem>

#include "filehashcalculatorthread.h"
#include "founditemwidget.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    enum DatabaseSavingMode { SaveToMemory, SaveToFile };

    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_actionExit_triggered();
    void on_commandLinkButton_clicked();
    void on_startSearchButton_clicked();
    void on_stopSearchButton_clicked();
    void correctFinishThread();
    void on_actionSave_To_File_changed();
    void on_pushButton_2_clicked();
    void setTopLabel(const QString &s);
    void setBottomLabel(const QString &s);
    void setProgressbarMaximumValue(int val);
    void setProgressbarValue(int val);
    void on_tableWidget_itemSelectionChanged();

private:
    Ui::MainWindow *ui;
    QSqlDatabase db;
    FileHashCalculatorThread* hashCalculator = 0;
    void deleteChildWidgets(QLayoutItem *item);
    void remove(QGridLayout *layout, int row, int column, bool deleteWidgets);
    QString sizeFormat(long long val);

};

#endif // MAINWINDOW_H
