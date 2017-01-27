#ifndef FILEHASHCALCULATORTHREAD_H
#define FILEHASHCALCULATORTHREAD_H

#include "common.h"

#include <QCryptographicHash>
#include <QDateTime>
#include <QDir>
#include <QFileInfo>
#include <QHeaderView>
#include <QLabel>
#include <QProgressBar>
#include <QSqlError>
#include <QThread>
#include <QWidget>


class FileHashCalculatorThread : public QThread {
  Q_OBJECT
 public:
  enum CompareMode { BySize = 1, Md5 = 2 };

  explicit FileHashCalculatorThread(QStringList dirs,
      CompareMode fileCompareMode = CompareMode::BySize);
  ~FileHashCalculatorThread();
  void stopWorking();

 protected:
  virtual void run();  // Q_DECL_OVERRIDE

 private:

  QStringList dirs;
  CompareMode fileCompareMode;
  bool stopWork = false;
  int getRandom();
  void calculateResultsMd5AndRemoveUniq() ;
  QString getFileHash(const QString &file_full_path, QFile *file);

  //    void

 signals:
  //    void resultReady(const QString &s);
  void setTopLabel(const QString &s);
  void setBottomLabel(const QString &s);
  void setProgressbarMaximumValue(int val);
  void setProgressbarValue(int val);
};

#endif  // FILEHASHCALCULATORTHREAD_H
