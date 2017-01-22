#ifndef FILEHASHCALCULATORTHREAD_H
#define FILEHASHCALCULATORTHREAD_H

#include <QThread>
#include <QDebug>
#include <QLabel>
#include <QFileInfo>
#include <QDir>
#include <QWidget>
#include <QSqlError>
#include <QtSql/QSqlQuery>
#include <QMessageBox>
#include <QSqlDatabase>
#include <QCryptographicHash>
#include <QDateTime>
#include <QProgressBar>
#include <QSqlTableModel>
#include <QHeaderView>


class FileHashCalculatorThread : public QThread
{
    Q_OBJECT
public:

    enum CompareMode {
        BySize = 0x0001,
        Md5 = 0x0002
    };

    explicit FileHashCalculatorThread(QSqlDatabase *db, QStringList dirs, CompareMode fileCompareMode = CompareMode::BySize);
    ~FileHashCalculatorThread();
    void stopWorking();



protected:
    virtual void run();//Q_DECL_OVERRIDE

private:
    QSqlDatabase *db;
    QStringList dirs;
    CompareMode fileCompareMode ;
    bool stopWork = false;
    int getRandom();

//    void

signals:
//    void resultReady(const QString &s);
    void setTopLabel(const QString &s);
    void setBottomLabel(const QString &s);
    void setProgressbarMaximumValue(int val);
    void setProgressbarValue(int val);

};

#endif // FILEHASHCALCULATORTHREAD_H