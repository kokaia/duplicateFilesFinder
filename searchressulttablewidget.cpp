#include "searchressulttablewidget.h"

SearchRessultTableWidget::SearchRessultTableWidget(QWidget *parent)
    : QTableWidget(parent) {}

SearchRessultTableWidget::~SearchRessultTableWidget() {}

void SearchRessultTableWidget::keyPressEvent(QKeyEvent *event) {
  if (event->key() == Qt::Key_Space) {
    int row = -1;
    foreach (QTableWidgetItem *item, this->selectedItems()) {
      if (row == -1) {
        row = item->row();
      }
    }

    if (row > -1) {
      this->item(row, SearchRessultTableWidget::Checkbox)
          ->setCheckState(Qt::Checked);
      this->cellClicked(row, SearchRessultTableWidget::Checkbox);
    }
  }
}


void SearchRessultTableWidget::removeAllRows() {
    while ( this->rowCount() >1) {
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
      ritem->setBackgroundColor(Qt::gray);
      font.setStrikeOut(true);
    } else {
      ritem->setBackgroundColor(Qt::white);
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
    if (item == 0) continue;
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
    if (item == 0) continue;
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
