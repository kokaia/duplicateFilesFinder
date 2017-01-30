#include "previewwidgetweb.h"
#include "ui_previewwidgetweb.h"

PreviewWidgetWeb::PreviewWidgetWeb(const QString& file_path, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PreviewWidgetWeb)
{
    ui->setupUi(this);
    QUrl url(file_path);
    ui->webView->load(url);
    QFileInfo f(file_path);
    ui->label->setText(f.fileName());
}

PreviewWidgetWeb::~PreviewWidgetWeb()
{
    delete ui;
}
