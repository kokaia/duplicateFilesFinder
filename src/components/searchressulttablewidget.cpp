#include "searchressulttablewidget.h"
#include <QFile>

SearchRessultTableWidget::SearchRessultTableWidget(QWidget *parent)
    : QTableWidget(parent) {}

SearchRessultTableWidget::~SearchRessultTableWidget() {}

void SearchRessultTableWidget::keyPressEvent(QKeyEvent *event) {
    int row = -1;
    foreach (QTableWidgetItem *item, this->selectedItems()) {
      if (row == -1) {
        row = item->row();
      }
    }

  if (row == -1)  return;

  if (event->key() == Qt::Key_Space) {
      this->item(row, SearchRessultTableWidget::Checkbox)
          ->setCheckState(Qt::Checked);
      this->cellClicked(row, SearchRessultTableWidget::Checkbox);
  } else if (event->key() == Qt::Key_Up || event->key() == Qt::Key_Down){
      int nrow = (event->key() == Qt::Key_Up) ? row -1 : row + 1 ;
      if (nrow < 0 || nrow > this->rowCount() ) return ;
      this->selectRow( nrow );
  }
}


void SearchRessultTableWidget::removeAllRows() {
    while ( this->rowCount() >0) {
        this->removeRow(0);
    }
}

void SearchRessultTableWidget::cellClicked(int row, int column) {
  if (column != SearchRessultTableWidget::Checkbox) return;

  int group = this->model()
                  ->index(row, SearchRessultTableWidget::Group)
                  .data(Qt::DisplayRole)
                  .toInt();

  if (group == 0) return;

  QTableWidgetItem *item = this->item(row, SearchRessultTableWidget::Checkbox);

  if ((item->flags() & Qt::ItemIsEnabled) == 0) return;

  this->selectRow(row);

  for (int i = 0; i < this->columnCount(); ++i) {
    QTableWidgetItem *ritem = this->item(row, i);
    QFont font(item->font());
    if (item->checkState() == Qt::Checked) {
      ritem->setBackground(Qt::gray);
      font.setStrikeOut(true);
    } else {
      ritem->setBackground(Qt::white);
      font.setStrikeOut(false);
    }
    ritem->setFont(font);
  }

  for (int i = 0; i < this->rowCount(); ++i) {
    if (i != row && (group ==
                     this->model()
                         ->index(i, SearchRessultTableWidget::Group)
                         .data(Qt::DisplayRole)
                         .toInt())) {
      QTableWidgetItem *nitem =
          this->item(i, SearchRessultTableWidget::Checkbox);

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

void SearchRessultTableWidget::deleteSelectedItems() {
  int filesCount = 0;
  long long  fullSize = 0;
  QStringList sl;
  for (int i = 0; i < this->rowCount(); ++i) {
    QTableWidgetItem *item = this->item(i, SearchRessultTableWidget::Checkbox);
    if (item == nullptr) continue;
    if (item->checkState() == Qt::Checked) {
      filesCount++;
      fullSize += this->item(i, SearchRessultTableWidget::FileSize)->data(Qt::DisplayRole).toInt();

      sl << this->item(i, SearchRessultTableWidget::FullPath)->data(Qt::DisplayRole).toString();
    }
  }

  int ans = QMessageBox::critical(
      this, " Delete files ",
      QString("Are you sure delete %1 files \n Full size %2").arg(filesCount).arg(Common::sizeFormat(fullSize)), QMessageBox::Ok | QMessageBox::Cancel);

  if (ans != QMessageBox::Ok) return;

  qDebug() << "Deleting files";
  for (int i = 0; i < this->rowCount(); ++i) {
    QTableWidgetItem *item = this->item(i, SearchRessultTableWidget::Checkbox);
    if (item == nullptr) continue;
    if (item->checkState() == Qt::Checked) {
      filesCount++;
      QTableWidgetItem *fpath =
          this->item(i, SearchRessultTableWidget::FullPath);
      QFile file(fpath->data(Qt::DisplayRole).toString());
      qDebug() << fpath->data(Qt::DisplayRole).toString();
      file.remove();
      this->removeRow(i);
    }
  }
}

void SearchRessultTableWidget::displayResults(){
    QSqlQuery query(Common::db);
    query.exec(
        "SELECT file_size, file_name, file_ext, file_created_date, "
        "file_modifed_date, file_full_path, file_hash FROM results order by "
        "file_size desc, file_hash asc ");
    int i = 0;
    const int colsCount = SearchRessultTableWidget::FullPath +
                          1;  // ui->tableWidget->columnCount();
    long long prevSize = 0;
    QString prev_file_hash;
    int group = 0;
    int j = 0;
    while (query.next()) {
      long long file_size       = query.value(0).toLongLong();
      QString file_name         = query.value(1).toString();
      QString file_ext          = query.value(2).toString();
      QString file_created_date = query.value(3).toString();
      QString file_modifed_date = query.value(4).toString();
      QString file_full_path    = query.value(5).toString();
      QString file_hash         = query.value(6).toString();

      if (prevSize != file_size || file_hash.compare(prev_file_hash) !=0) {
        if (group > 0) {
          this->insertRow(i);
          for (j = 0; j < colsCount; ++j) {
            QTableWidgetItem *item = new QTableWidgetItem();
            item->setBackground(Qt::darkGray);
            this->setItem(i, j, item);
          }
          i++;
        }
        ++group;
      }
      this->insertRow(i);
      QTableWidgetItem *items[colsCount];

      for (j = 0; j < colsCount; ++j) {
        items[j] = new QTableWidgetItem();
      }

      items[SearchRessultTableWidget::Checkbox]->setCheckState(Qt::Unchecked);
      items[SearchRessultTableWidget::Group]->setText(QString::number(group));
      items[SearchRessultTableWidget::FileSize]->setText(
          QString::number(file_size));
      items[SearchRessultTableWidget::FileSizeMixed]->setText(
          Common::sizeFormat(file_size));
      items[SearchRessultTableWidget::FileName]->setText(file_name);
      items[SearchRessultTableWidget::Ext]->setText(file_ext);
      items[SearchRessultTableWidget::CreateDate]->setText(file_created_date);
      items[SearchRessultTableWidget::ModifedDate]->setText(file_modifed_date);
      items[SearchRessultTableWidget::FileHash]->setText(file_hash);
      items[SearchRessultTableWidget::FullPath]->setText(file_full_path);

      for (j = 0; j < colsCount; ++j) {
        this->setItem(i, j, items[j]);
      }
      ++i;
      prevSize = file_size;
      prev_file_hash = file_hash;
    }

}


void SearchRessultTableWidget::unSelectItems() {
  this->isAutoCheck = true;
  for (int i = 0; i < this->rowCount(); ++i) {
    QTableWidgetItem *item =
        this->item(i, SearchRessultTableWidget::Checkbox);
    if (item == nullptr) continue;
    if (item->checkState() == Qt::Checked) {
      item->setCheckState(Qt::Unchecked);
      this->cellClicked(i, SearchRessultTableWidget::Checkbox);
    }
  }
  this->isAutoCheck = false;
}

void SearchRessultTableWidget::selectItemsByPath() {
  this->isAutoCheck = true;
  bool ok;
  int row = 0;
  foreach (QTableWidgetItem *item, this->selectedItems()) {
    row = item->row();
  }

  QString text = QInputDialog::getText(
      this, tr("QInputDialog::getText()"), tr("User name:"), QLineEdit::Normal,
      this->item(row, SearchRessultTableWidget::FullPath)
          ->data(Qt::DisplayRole)
          .toString(),
      &ok);

  if (ok && !text.isEmpty()) {
  qDebug() << text;

  for (int i = 0; i < this->rowCount(); ++i) {
    QTableWidgetItem *fpath =
        this->item(i, SearchRessultTableWidget::FullPath);
    if (fpath == nullptr) continue;
    if (fpath->data(Qt::DisplayRole).toString().startsWith(text)) {
      QTableWidgetItem *item =
          this->item(i, SearchRessultTableWidget::Checkbox);
      item->setCheckState(Qt::Checked);
      this->cellClicked(i, SearchRessultTableWidget::Checkbox);
    }
  }
  }
  this->isAutoCheck = false;
}
