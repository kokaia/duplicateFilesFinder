#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
  ui->setupUi(this);

  connect(ui->tableWidget, SIGNAL(cellClicked(int, int)), this,
          SLOT(cellClicked(int, int)));

  ui->markingButton->addAction(ui->actionClearSelection);
  ui->markingButton->addAction(ui->actionInvertSelection);
  ui->markingButton->addAction(ui->actionSelectByPath);

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
  ui->operationsTabsWidget->setCurrentIndex(0);
  hashCalculator->wait();
  hashCalculator->deleteLater();
  hashCalculator = 0;
  ui->startSearchButton->setEnabled(true);
  ui->stopSearchButton->setEnabled(false);

  QSqlQuery query(db);
  query.exec(
      "SELECT file_size, file_name, file_ext, file_created_date, "
      "file_modifed_date, file_full_path FROM results order by file_size desc");
  int i = 0;
  const int rowsCount = 9;
  long long prevSize = 0;
  int group = 0;
  int j = 0;
  while (query.next()) {
    long long file_size = query.value(0).toLongLong();
    QString file_name = query.value(1).toString();
    QString file_ext = query.value(2).toString();
    QString file_created_date = query.value(3).toString();
    QString file_modifed_date = query.value(4).toString();
    QString file_full_path = query.value(5).toString();

    if (prevSize != file_size) {
      if (group > 0) {
        ui->tableWidget->insertRow(i);
        for (j = 0; j < rowsCount; ++j) {
          QTableWidgetItem *item = new QTableWidgetItem();
          item->setBackgroundColor(Qt::darkGray);
          ui->tableWidget->setItem(i, j, item);
        }
        i++;
      }
      ++group;
    }
    ui->tableWidget->insertRow(i);
    QTableWidgetItem *items[rowsCount];

    for (j = 0; j < rowsCount; ++j) {
      items[j] = new QTableWidgetItem();
    }

    items[MainWindow::Checkbox]->setCheckState(Qt::Unchecked);
    items[MainWindow::Group]->setText(QString::number(group));
    items[MainWindow::FileSize]->setText(QString::number(file_size));
    items[MainWindow::FileSizeMixed]->setText(sizeFormat(file_size));
    items[MainWindow::FileName]->setText(file_name);
    items[MainWindow::Ext]->setText(file_ext);
    items[MainWindow::CreateDate]->setText(file_created_date);
    items[MainWindow::ModifedDate]->setText(file_modifed_date);
    items[MainWindow::FullPath]->setText(file_full_path);

    for (j = 0; j < rowsCount; ++j) {
      ui->tableWidget->setItem(i, j, items[j]);
    }
    ++i;
    prevSize = file_size;
  }

  ui->tableWidget->resizeColumnsToContents();
}

void MainWindow::cellClicked(int row, int column) {
  if (column != 0) return;

  int group = ui->tableWidget->model()
                  ->index(row, MainWindow::Group)
                  .data(Qt::DisplayRole)
                  .toInt();

  if (group == 0) return;

  QTableWidgetItem *item = ui->tableWidget->item(row, MainWindow::Checkbox);

  if ((item->flags() & Qt::ItemIsEnabled) == 0) return;

  ui->tableWidget->selectRow(row);

  for (int i = 0; i < ui->tableWidget->columnCount(); ++i) {
    QTableWidgetItem *ritem = ui->tableWidget->item(row, i);
    QFont font(item->font());
    if (item->checkState() == Qt::Checked) {
      ritem->setBackgroundColor(Qt::gray);
      font.setStrikeOut(true);
    } else {
      ritem->setBackgroundColor(Qt::white);
      font.setStrikeOut(false);
    }
    ritem->setFont(font);
  }

  for (int i = 0; i < ui->tableWidget->rowCount(); ++i) {
    if (i != row && (group ==
                     ui->tableWidget->model()
                         ->index(i, MainWindow::Group)
                         .data(Qt::DisplayRole)
                         .toInt())) {
      QTableWidgetItem *nitem = ui->tableWidget->item(i, MainWindow::Checkbox);

      if (item->checkState() == Qt::Checked) {
        nitem->setFlags(nitem->flags() ^ Qt::ItemIsEnabled ^
                        Qt::ItemIsEditable);
      } else {
        nitem->setFlags(nitem->flags() | Qt::ItemIsEnabled |
                        Qt::ItemIsEditable);
      }
    }
  }
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
  ui->tabWidget->setCurrentIndex(4);
  int fileSize = 0;
  QList<QTableWidgetItem *> items = ui->tableWidget->selectedItems();
  foreach (const QTableWidgetItem *item, items) {
    if (item->column() == 2) {
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
    if (FoundItemWidget::validExtentions.contains(file_ext.toLower())) {
      w = new FoundItemWidget(file_full_path);
    } else {
      w = new FoundItemWebWidget(file_full_path);
    }

    //    w->setGeometry(0,0,400,400);
    ui->resultPreviewTabLayout->addWidget(w, 0, i++,
                                          Qt::AlignLeft | Qt::AlignTop);
  }
}

void MainWindow::on_searchLocationRemovePathButton_clicked() {
  qDeleteAll(ui->listWidget->selectedItems());
}

void MainWindow::on_searchLocationAddPathButton_clicked() {
  QString dir = QFileDialog::getExistingDirectory(
      this, tr("Open Directory"), "",
      QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

  ui->listWidget->addItem(dir);
}

void MainWindow::on_searchLocationClearAll_clicked() {
  ui->listWidget->clear();
}

void MainWindow::on_pushButton_3_clicked() {
  for (int i = 0; i < ui->tableWidget->rowCount(); ++i) {
    QTableWidgetItem *item = ui->tableWidget->item(i, MainWindow::Checkbox);
    if (item == 0) continue;
    if (item->checkState() == Qt::Checked) {
      QTableWidgetItem *itemFullPath =
          ui->tableWidget->item(i, MainWindow::FullPath);
      qDebug() << itemFullPath->data(Qt::DisplayRole).toString();
    }
  }
}

void MainWindow::on_actionClearSelection_triggered() {
  for (int i = 0; i < ui->tableWidget->rowCount(); ++i) {
    QTableWidgetItem *item = ui->tableWidget->item(i, MainWindow::Checkbox);
    if (item == 0) continue;
    if (item->checkState() == Qt::Checked) {
      item->setCheckState(Qt::Unchecked);
      cellClicked(i, MainWindow::Checkbox);
    }
  }
}

void MainWindow::on_actionSelectByPath_triggered() {
  bool ok;
  int row = 0;
  foreach (QTableWidgetItem *item, ui->tableWidget->selectedItems()) {
    row = item->row();
  }

  QString text = QInputDialog::getText(
      this, tr("QInputDialog::getText()"), tr("User name:"), QLineEdit::Normal,
      ui->tableWidget->item(row, MainWindow::FullPath)
          ->data(Qt::DisplayRole)
          .toString(),
      &ok);

  if (ok && !text.isEmpty()) qDebug() << text;
}
