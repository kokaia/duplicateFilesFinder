#ifndef FILEHASHCALCULATORTHREAD_H
#define FILEHASHCALCULATORTHREAD_H

#include "common.h"
#include "components/tfilterwidget.h"
#include <QThread>


class FileHashCalculatorThread : public QThread {
Q_OBJECT
public:
    enum CompareMode {
        BySize = 1, Md5 = 2
    };

    explicit FileHashCalculatorThread(QStringList dirs, TFilterWidget *filtersWidget, CompareMode fileCompareMode = CompareMode::BySize);

    ~FileHashCalculatorThread() override;

    void stopWorking();

private:
    TFilterWidget *filtersWidget;
    QStringList dirs;
    CompareMode fileCompareMode;
    QTimer *timerEverySecond;
    qint32 timeSinceStart{};
    bool stopWork = false;

    static quint32 getRandom();

    void calculateResultsMd5AndRemoveUniq();

    static QString getFileHash(const QString &file_full_path, QFile *file, bool &fromCache);

protected:
    void run() override;  // Q_DECL_OVERRIDE


signals:

    //    void resultReady(const QString &s);
    void setTopLabel(const QString &s);

    void setBottomLabel(const QString &s);

    void setProgressbarMaximumValue(int val);

    void setProgressbarValue(int val);

    void setProgressBarSizeMaximumValue(int val);

    void setProgressBarSizeValue(int val);

    void updateApproximateTime(int val);

private slots:

    void updateTimeSinceStart();

    static QString shorten_str(const QString& qString);
};

#endif  // FILEHASHCALCULATORTHREAD_H
