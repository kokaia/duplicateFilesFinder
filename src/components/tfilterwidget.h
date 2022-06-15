#ifndef TFILTERWIDGET_H
#define TFILTERWIDGET_H

#include "src/common.h"

#include <QDir>

namespace Ui {
class TFilterWidget;
}

class TFilterWidget : public QWidget
{
    Q_OBJECT

public:
    explicit TFilterWidget(QWidget *parent = 0);
    ~TFilterWidget();

    qint64 getSizeFrom()  const ;
    qint64 getSizeTo()  const;
    QDir::Filters getFilters()  const;
    QSet<QString> getFilterExtentions() const;


private slots:
    void on_fileSizeRangeCheckBox_toggled(bool checked);

private:
    Ui::TFilterWidget *ui;
};

#endif // TFILTERWIDGET_H
