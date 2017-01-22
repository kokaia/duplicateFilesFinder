#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
  ui->setupUi(this);
  if (!QSqlDatabase::drivers().contains("QSQLITE")) {
    QMessageBox::critical(this, "Unable to load database",
                          "This demo needs the SQLITE driver");
    return;
  }
  this->db = QSqlDatabase::addDatabase("QSQLITE");
  QString dbFileName = ":memory:";
  //    dbFileName = QDir::currentPath();
  //    dbFileName = dbFileName + "/files.sqlite";

  this->db.setDatabaseName(dbFileName);
  if (!this->db.open()) {
    QMessageBox::critical(
        0, qApp->tr("Cannot open database"),
        qApp->tr("Unable to establish a database connection.\n"
                 "This example needs SQLite support. Please read "
                 "the Qt SQL driver documentation for information how "
                 "to build it.\n\n"
                 "Click Cancel to exit."),
        QMessageBox::Cancel);
    return;
  }
  QStringList tables = this->db.tables();
  QSqlQuery query;
  qDebug() << "Found Tables";
  qDebug() << tables;

  if (!tables.contains("files")) {
    qDebug() << "Create table files";
    query.exec(
        "CREATE TABLE files ("
        "id	INTEGER PRIMARY KEY AUTOINCREMENT,"
        "file_hash	TEXT,"
        "file_name	TEXT,"
        "file_size	integer,"
        "file_full_path	TEXT,"
        "file_ext	TEXT,"
        "file_created_date	date,"
        "file_modifed_date	date,"
        " add_date datetime "
        ");");
  }
  if (!tables.contains("results")) {
    qDebug() << "Create table results";
    query.exec(
        "CREATE TABLE results ("
        "id	INTEGER PRIMARY KEY AUTOINCREMENT,"
        "group_ida    int,"
        "file_name	TEXT,"
        "file_size	integer,"
        "file_full_path	TEXT,"
        "file_ext	TEXT,"
        "file_created_date	date,"
        "file_modifed_date	date,"
        " add_date datetime "
        ");");
  }
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::on_actionExit_triggered() { QApplication::exit(0); }

void MainWindow::on_commandLinkButton_clicked() {
  QString dir = QFileDialog::getExistingDirectory(
      this, tr("Open Directory"), "",
      QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

  ui->listWidget->addItem(dir);
}

void MainWindow::on_startSearchButton_clicked() {
  ui->startSearchButton->setEnabled(false);
  ui->stopSearchButton->setEnabled(true);
  QStringList dirList;
  for (int i = 0; i < ui->listWidget->count(); ++i) {
    QListWidgetItem *item = ui->listWidget->item(i);
    if (item->text().length() > 0) {
      QFileInfo fi(item->text());
      if (fi.isDir()) {
        dirList << item->text();
      }
    }
  }
  if (dirList.size() == 0) {
    ui->startSearchButton->setEnabled(true);
    ui->stopSearchButton->setEnabled(false);
  }
  qDebug() << "Selected Directories";
  qDebug() << dirList;

  if (hashCalculator != NULL) {
    qDebug() << "";
  }

  hashCalculator = new FileHashCalculatorThread(&db, dirList);
  hashCalculator->start();
  connect(hashCalculator, SIGNAL(finished()), this,
          SLOT(correctFinishThread()));

  connect(hashCalculator, SIGNAL(setTopLabel(QString)), this,
          SLOT(setTopLabel(QString)));
  connect(hashCalculator, SIGNAL(setBottomLabel(QString)), this,
          SLOT(setBottomLabel(QString)));
  connect(hashCalculator, SIGNAL(setProgressbarMaximumValue(int)), this,
          SLOT(setProgressbarMaximumValue(int)));
  connect(hashCalculator, SIGNAL(setProgressbarValue(int)), this,
          SLOT(setProgressbarValue(int)));

  ui->operationsTabsWidget->setCurrentIndex(1);

  //    qDebug() << "hashCalculator";
  //    ui->startSearchButton->setEnabled(true);
}

void MainWindow::on_stopSearchButton_clicked() {
  hashCalculator->stopWorking();
}

void MainWindow::correctFinishThread() {
  hashCalculator->wait();
  hashCalculator->deleteLater();
  hashCalculator = 0;
  ui->startSearchButton->setEnabled(true);
  ui->stopSearchButton->setEnabled(false);
  ui->operationsTabsWidget->setCurrentIndex(0);

  QSqlQuery query(db);
  query.exec(
      "SELECT file_size, file_name, file_ext, file_created_date, "
      "file_modifed_date FROM results order by file_size desc");
  int i = 0;
  long long prevSize = 0;
  int group = 0;
  while (query.next()) {
    long long file_size = query.value(0).toLongLong();
    QString file_name = query.value(1).toString();
    QString file_ext = query.value(2).toString();
    QString file_created_date = query.value(3).toString();
    QString file_modifed_date = query.value(4).toString();
    if (prevSize != file_size) ++group;
    ui->tableWidget->insertRow(i);
    QTableWidgetItem *items[7];
    for (int j = 0; j < 7; ++j) {
      items[j] = new QTableWidgetItem();
    }
    items[0]->setText(QString::number(group));
    items[1]->setText(QString::number(file_size));
    items[2]->setText(sizeFormat(file_size));
    items[3]->setText(file_name);
    items[4]->setText(file_ext);
    items[5]->setText(file_created_date);
    items[6]->setText(file_modifed_date);
    for (int j = 0; j < 7; ++j) {
      ui->tableWidget->setItem(i, j, items[j]);
    }
    ++i;
    prevSize = file_size;
  }

  //  model1->setHeaderData(0, Qt::Horizontal, tr("file_size"));
  //  model1->setHeaderData(1, Qt::Horizontal, tr("file_name"));
  //  model1->setHeaderData(2, Qt::Horizontal, tr("file_ext"));
  //  model1->setHeaderData(3, Qt::Horizontal, tr("file_created_date"));
  //  model1->setHeaderData(4, Qt::Horizontal, tr("file_modifed_date"));

  //  ui->tableView->setModel(model1);
  //  ui->tableView->resizeColumnsToContents();
  //    ui->tableView->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
}

QString MainWindow::sizeFormat(long long val) {
  if (val / 1073741824 > 0)
    return QString("%1 GB").arg((val * 100 / 1073741824) * 0.01);
  if ((val / 1048576) > 0)
    return QString("%1 MB").arg((val * 100 / 1048576) * 0.01);
  if ((val / 1024) > 0) return QString("%1 KB").arg((val * 100 / 1024) * 0.01);
  return QString("%1 B").arg(val);
};

void MainWindow::on_actionSave_To_File_changed() {}

void MainWindow::on_pushButton_2_clicked() {
  ui->tableWidget->horizontalHeader()->setStretchLastSection(true);
}

void MainWindow::setTopLabel(const QString &s) {
  ui->progressLabel->setText(s);
}

void MainWindow::setBottomLabel(const QString &s) {
  ui->currentOperationLabel->setText(s);
}

void MainWindow::setProgressbarMaximumValue(int val) {
  ui->progressbarFiles->setMaximum(val);
}

void MainWindow::setProgressbarValue(int val) {
  ui->progressbarFiles->setValue(val);
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
  //     qDebug() <<
  int fileSize = 0;
  QList<QTableWidgetItem *> items = ui->tableWidget->selectedItems();
  foreach (const QTableWidgetItem *item, items) {
    if (item->column() == 1) {
      fileSize = item->data(0).toInt();
    }
  }
  if (fileSize == 0) {
    return;
  }

  QSqlQuery query(db);
  query.exec(QString("SELECT file_size, file_name, file_full_path, file_ext, "
                     "file_created_date, file_modifed_date FROM results WHERE "
                     "file_size = %1")
                 .arg(fileSize));

  QGridLayout *layout = (QGridLayout *)ui->tab_2->layout();

  for (int i = 0; i < layout->rowCount(); i++) {
    for (int j = 0; j < layout->columnCount(); j++) {
      remove(layout, i, j, true);
    }
  }

  int i = 0;
  while (query.next()) {
    //        int file_size = query.value(0).toInt();
    //        QString file_name = query.value(1).toString();
    QString file_full_path = query.value(2).toString();
    //        QString file_ext = query.value(3).toString();
    //        QFileInfo file(file_full_path);

    FoundItemWidget *w = new FoundItemWidget(file_full_path);
    //    w->setGeometry(0,0,400,400);
    layout->addWidget(w, 0, i++, Qt::AlignLeft | Qt::AlignTop);
  }
}
