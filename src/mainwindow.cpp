#include "mainwindow.h"
#include "ui_mainwindow.h"

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

MainWindow::MainWindow(QWidget *parent)
        : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);

    ui->markingButton->addAction(ui->actionClearSelection);
    ui->markingButton->addAction(ui->actionInvertSelection);
    ui->markingButton->addAction(ui->actionSelectByPath);
    timerEverySecond = new QTimer(this);

}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::on_actionExit_triggered() { QApplication::exit(0); }

void MainWindow::on_startSearchButton_clicked() {
    ui->startSearchButton->setEnabled(false);
    ui->stopSearchButton->setEnabled(true);
    ui->labelAproximateTime->setText("");
    ui->labelExecutedTime->setText("");
    ui->labelStartDate->setText(QString("Start time %1").arg(QDateTime::currentDateTime().toString()));
    QStringList dirList;
    for (int i = 0; i < ui->searchDirsWidget->getListWidget()->count(); ++i) {
        QListWidgetItem *item = ui->searchDirsWidget->getListWidget()->item(i);
        if (item->text().length() > 0) {
            QFileInfo fi(item->text());
            if (fi.isDir()) {
                dirList << item->text();
            }
        }
    }
    if (dirList.empty()) {
        ui->startSearchButton->setEnabled(true);
        ui->stopSearchButton->setEnabled(false);
    }
    qDebug() << "Selected Directories";
    qDebug() << dirList;

    if (hashCalculator != nullptr) {
        qDebug() << "";
    }

    FileHashCalculatorThread::CompareMode fileCompareMode =
            FileHashCalculatorThread::BySize;
    if (ui->sizeThenMd5RadioButton->isChecked()) {
        fileCompareMode = FileHashCalculatorThread::Md5;
    }

    hashCalculator = new FileHashCalculatorThread(dirList, ui->filtersWidget, fileCompareMode);
    hashCalculator->start();

    connect(hashCalculator, SIGNAL(finished()), this, SLOT(correctFinishThread()));
    connect(hashCalculator, SIGNAL(updateApproximateTime(int)), this, SLOT(updateApproximateTime(int)));
    connect(hashCalculator, SIGNAL(setTopLabel(QString)), ui->progressLabel, SLOT(setText(QString)));
    connect(hashCalculator, SIGNAL(setBottomLabel(QString)), ui->currentOperationLabel, SLOT(setText(QString)));
    connect(hashCalculator, SIGNAL(setProgressbarMaximumValue(int)), ui->progressbarFiles, SLOT(setMaximum(int)));
    connect(hashCalculator, SIGNAL(setProgressbarValue(int)), ui->progressbarFiles, SLOT(setValue(int)));
    connect(hashCalculator, SIGNAL(setProgressBarSizeMaximumValue(int)), ui->progressBarFileSize, SLOT(setMaximum(int)));
    connect(hashCalculator, SIGNAL(setProgressBarSizeValue(int)), ui->progressBarFileSize, SLOT(setValue(int)));

    timeSinceStart = 0;
    QObject::connect(timerEverySecond, SIGNAL(timeout()), this, SLOT(updateStartTime()));
    timerEverySecond->start(1000);
    ui->operationsTabsWidget->setCurrentIndex(1);
}

void MainWindow::on_stopSearchButton_clicked() {
    hashCalculator->stopWorking();
}

void MainWindow::correctFinishThread() {
    ui->tableWidget->isAutoCheck = true;
    ui->operationsTabsWidget->setCurrentIndex(0);
    ui->tableWidget->removeAllRows();
    hashCalculator->wait();
    hashCalculator->deleteLater();
    hashCalculator = nullptr;
    ui->startSearchButton->setEnabled(true);
    ui->stopSearchButton->setEnabled(false);

    ui->tableWidget->displayResults();

    ui->tableWidget->resizeColumnsToContents();
    ui->tableWidget->isAutoCheck = false;
    ui->labelEndDate->setText(QString("End Date %1").arg(QDateTime::currentDateTime().toString()));
    timerEverySecond->stop();
}

void MainWindow::remove(QGridLayout *layout, int row, int column,
                        bool deleteWidgets) {
    // We avoid usage of QGridLayout::itemAtPosition() here to improve
    // performance.
    for (int i = layout->count() - 1; i >= 0; i--) {
        int r, c, rs, cs;
        layout->getItemPosition(i, &r, &c, &rs, &cs);
        if ((r <= row && r + rs - 1 >= row) ||
            (c <= column && c + cs - 1 >= column)) {
            // This layout item is subject to deletion.
            QLayoutItem *item = layout->takeAt(i);
            if (deleteWidgets) {
                deleteChildWidgets(item);
            }
            delete item;
        }
    }
}

void MainWindow::deleteChildWidgets(QLayoutItem *item) {
    if (item->layout()) {
        // Process all child items recursively.
        for (int i = 0; i < item->layout()->count(); i++) {
            deleteChildWidgets(item->layout()->itemAt(i));
        }
    }
    delete item->widget();
}

void MainWindow::on_tableWidget_itemSelectionChanged() {
    if (ui->tableWidget->isAutoCheck) return;
    qDebug() << "call tableWidget itemSelectionChanged";
    ui->tabWidget->setCurrentIndex(4);
    int fileSize = 0;
    QList<QTableWidgetItem *> items = ui->tableWidget->selectedItems();
            foreach (const QTableWidgetItem *item, items) {
            if (item->column() == SearchRessultTableWidget::FileSize) {
                fileSize = item->data(Qt::DisplayRole).toInt();
            }
        }

    if (fileSize == 0) {
        return;
    }

    QSqlQuery query(Common::db);

    query.exec(QString("SELECT file_size, file_name, file_full_path, file_ext, "
                       "file_created_date, file_modifed_date FROM results WHERE "
                       "file_size = %1")
                       .arg(fileSize));

    for (int i = 0; i < ui->resultPreviewTabLayout->rowCount(); i++) {
        for (int j = 0; j < ui->resultPreviewTabLayout->columnCount(); j++) {
            remove(ui->resultPreviewTabLayout, i, j, true);
        }
    }

    int i = 0;
    while (query.next()) {
        //        int file_size = query.value(0).toInt();
        //        QString file_name = query.value(1).toString();
        QString file_full_path = query.value(2).toString();
        QString file_ext = query.value(3).toString();
        //        QFileInfo file(file_full_path);

        QWidget *w;
        if (Common::photo.contains(file_ext.toLower())) {
            w = new PreviewWidgetImage(file_full_path);
        } else if (Common::video.contains(file_ext.toLower())) {
//       w = new PreviewWidgetVideo(file_full_path);
        } else if (Common::audio.contains(file_ext.toLower())) {
            w = new PreviewWidgetAudio(file_full_path);
        } else if (Common::web.contains(file_ext.toLower())) {
//        w = new PreviewWidgetWeb(file_full_path);
        } else {
            w = new FoundItemWidget(file_full_path);
        }

        //    w->setGeometry(0,0,400,400);
        ui->resultPreviewTabLayout->addWidget(w, 0, i++,
                                              Qt::AlignLeft | Qt::AlignTop);
    }
}

void MainWindow::on_actionInvalidate_Files_Hash_Database_triggered() {
    QSqlQuery query(Common::hashDb);
    query.exec("DELETE FROM files_hash ");
}

void MainWindow::updateStartTime() {
    timeSinceStart++;
    if (timeSinceStart < 60) {
        ui->labelExecutedTime->setText(QString("Executed time %1 sec").arg(timeSinceStart));
    } else if (timeSinceStart < 3600) {
        ui->labelExecutedTime->setText(QString("Executed time %1 min, %2 sec").arg(timeSinceStart / 60).arg(timeSinceStart % 60));
    } else if (timeSinceStart < 216000) {
        ui->labelExecutedTime->setText(QString("Executed time %1 hour, %2 min, %3 sec").arg(timeSinceStart / 3600).arg((timeSinceStart % 3600) / 60).arg(timeSinceStart % 60));
    } else {
        ui->labelExecutedTime->setText(QString("Executed time %1 hour, %2 min, %3 sec").arg(timeSinceStart / 3600).arg((timeSinceStart % 3600) / 60).arg(timeSinceStart % 60));
    }
}

void MainWindow::updateApproximateTime(int appTime) {
    if (appTime < 60) {
        ui->labelAproximateTime->setText(QString("Expected time time %1 sec").arg(appTime));
    } else if (appTime < 3600) {
        ui->labelAproximateTime->setText(QString("Expected time %1 min, %2 sec").arg(appTime / 60).arg(appTime % 60));
    } else if (appTime < 216000) {
        ui->labelAproximateTime->setText(QString("Expected time %1 hour, %2 min, %3 sec").arg(appTime / 3600).arg((appTime % 3600) / 60).arg(appTime % 60));
    } else {
        ui->labelAproximateTime->setText(QString("Expected time %1 hour, %2 min, %3 sec").arg(appTime / 3600).arg((appTime % 3600) / 60).arg(appTime % 60));
    }
}
