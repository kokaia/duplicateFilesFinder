#include "commonlistwidget.h"
#include "ui_commonlistwidget.h"

CommonListWidget::CommonListWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CommonListWidget)
{
    ui->setupUi(this);
}

CommonListWidget::~CommonListWidget()
{
    delete ui;
}

QListWidget* CommonListWidget::getListWidget(){
    return   ui->listWidget;
}

void CommonListWidget::on_addPathButton_clicked()
{
    QString dir = QFileDialog::getExistingDirectory(
        this, tr("Open Directory"), "",
        QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

    ui->listWidget->addItem(dir);
}

/*
void MainWindow::on_protectFoldersRemoveSelectedButton_clicked()
{
    qDeleteAll(ui->protectFoldersListWidget->selectedItems());
}
*/

void CommonListWidget::on_pastePathButton_clicked()
{
    const QClipboard *clipboard = QApplication::clipboard();

    if (!clipboard->text().isNull() && !clipboard->text().isEmpty())
    ui->listWidget->addItem(clipboard->text());
}
