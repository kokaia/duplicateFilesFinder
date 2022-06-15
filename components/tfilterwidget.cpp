#include "tfilterwidget.h"
#include "ui_tfilterwidget.h"

TFilterWidget::TFilterWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TFilterWidget)
{
    ui->setupUi(this);

    ui->filterFileMaskComboBox->addItem("All Files\t| *.*", Common::allFilesGroup);
    ui->filterFileMaskComboBox->addItem("Images\t| *." + QString(Common::photo.values().join(", *.")), Common::photoGroup);
    ui->filterFileMaskComboBox->addItem("Audio\t| *." + QString(Common::audio.values().join(", *.")), Common::audioGroup);
    ui->filterFileMaskComboBox->addItem("Video\t| *." + QString(Common::video.values().join(", *.")), Common::videoGroup);

}

TFilterWidget::~TFilterWidget()
{
    delete ui;
}

qint64 TFilterWidget::getSizeFrom() const{
    if (!ui->fileSizeRangeCheckBox->isChecked()) return -1;
    int multipleIdx = ui->filterFileSizeRangeCombo->currentIndex();
    qint64 sizeFrom = ui->filterFileSizeFrom->value();

    while (--multipleIdx >= 0) {
            sizeFrom *= 1024;
    }
    return sizeFrom;
}

qint64 TFilterWidget::getSizeTo()  const{
    if (!ui->fileSizeRangeCheckBox->isChecked()) return -1;

    int multipleIdx = ui->filterFileSizeRangeCombo->currentIndex();
    qint64 sizeTo   = ui->filterFileSizeTo  ->value();

    while (--multipleIdx >= 0) {
            sizeTo   *= 1024;
    }
    return sizeTo;
}

QDir::Filters TFilterWidget::getFilters()  const{
    QDir::Filters filter = QDir::NoDotAndDotDot;
    if (ui->filterHiddenFilesAndFolders->isChecked()) {
        filter |= QDir::Hidden;
    }
    if (!ui->filterSystemFilesAndFolders->isChecked()) {
        filter |= QDir::System;
    }
    return filter;
}

QSet<QString>  TFilterWidget::getFilterExtentions()  const{
    auto search = ui->filterFileMaskComboBox->currentData();

    if (search==Common::photoGroup){
        return Common::photo;
    }

    if (search==Common::audioGroup){
        return Common::audio;
    }

    if (search==Common::videoGroup){
        return Common::video;
    }
    return  QSet<QString>();
}




void TFilterWidget::on_fileSizeRangeCheckBox_toggled(bool checked)
{
    ui->filterFileSizeFrom      ->setEnabled(checked);
    ui->filterFileSizeTo        ->setEnabled(checked);
    ui->filterFileSizeRangeCombo->setEnabled(checked);
}
