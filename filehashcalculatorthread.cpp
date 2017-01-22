#include "filehashcalculatorthread.h"

FileHashCalculatorThread::FileHashCalculatorThread(QSqlDatabase *db,
                                                   QStringList dirs,
                                                   CompareMode fileCompareMode)
    : db(db), dirs(dirs), fileCompareMode(fileCompareMode) {
  qsrand(QTime::currentTime().msec());
}

FileHashCalculatorThread::~FileHashCalculatorThread() {
  // delete label;
  //    delete dirs;
}

int FileHashCalculatorThread::getRandom() { return qrand() % 100 + 100; }

void FileHashCalculatorThread::run() {
  qDebug() << "Start Calculating Hashs for Directories";
  qDebug() << dirs;

  int dirsSize = dirs.size();
  int i = 0, filesCount = 0;
  QStringList files, curdirList, curFilesList;
  QString str;
  emit setTopLabel(tr("Scanning folders and files"));
  int nextPaint = 1;

  while (!this->stopWork && !isInterruptionRequested() && i < dirsSize) {
    QDir curDir(dirs.at(i++));
    curdirList.clear();
    curdirList << curDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot |
                                   QDir::Hidden | QDir::System);
    foreach (QString var, curdirList) {
      dirs << curDir.absolutePath() + "/" + var;
      dirsSize++;
    }
    //        dirsSize = dirs.length();
    curFilesList.clear();
    curFilesList << curDir.entryList(QDir::Files | QDir::NoDotAndDotDot |
                                     QDir::Hidden | QDir::System);
    foreach (QString var, curFilesList) {
      files << curDir.absolutePath() + "/" + var;
      filesCount++;
    }
    if (i > nextPaint) {
      emit setTopLabel(QString(tr("Found %1 folders and %2 files"))
                           .arg(dirsSize - 1)
                           .arg(filesCount - 1));
      nextPaint = nextPaint + getRandom();
    }
  }

  emit setTopLabel(QString(tr("Found %1 folders and %2 files"))
                       .arg(dirsSize)
                       .arg(filesCount));
  emit setProgressbarMaximumValue(filesCount);
  i = 0;
  QSqlQuery query(*db);
  //    QSqlQuery query1(*db);
  query.prepare(
      "INSERT INTO files( file_hash, file_full_path, file_name, file_size, "
      "file_ext, file_created_date, file_modifed_date, add_date)"
      " VALUES "
      "(:file_hash,:file_full_path,:file_name,:file_size,:file_ext,:file_"
      "created_date,:file_modifed_date, datetime('now') )");

  nextPaint = 1;
  while (!this->stopWork && !isInterruptionRequested() && i < filesCount) {
    QFile file(files[i++]);
    if (!file.exists()) {
      continue;
    }

    if (fileCompareMode == CompareMode::Md5) file.open(QIODevice::ReadOnly);

    QString hashData =
        (fileCompareMode == CompareMode::Md5)
            ? QCryptographicHash::hash(file.readAll(), QCryptographicHash::Md5)
                  .toHex()
            : "";

    if (i > nextPaint || i + 5 > filesCount) {
      emit setProgressbarValue(i);
      emit setBottomLabel(
          tr("Current processing #%1, file=%2").arg(i).arg(file.fileName()));
      nextPaint = nextPaint + getRandom();
    }

    QFileInfo fileInfo = QFileInfo(file);

    query.bindValue(":file_hash", hashData);
    query.bindValue(":file_full_path", fileInfo.absoluteFilePath());
    query.bindValue(":file_name", fileInfo.fileName());
    query.bindValue(":file_size", fileInfo.size());
    query.bindValue(":file_ext", fileInfo.suffix());
    query.bindValue(":file_created_date",
                    fileInfo.created());  //.toMSecsSinceEpoch()
    query.bindValue(":file_modifed_date",
                    fileInfo.lastModified());  //.toMSecsSinceEpoch()

    query.exec();
    if (query.lastError().isValid()) {
      qDebug() << query.lastError().text();
      // QMessageBox::critical(0, tr("Database Error"),
      // query.lastError().text(), QMessageBox::Cancel);
    }
  }
  emit setProgressbarValue(filesCount);

  query.exec(
      "INSERT INTO results( file_name, file_size, file_full_path, file_ext, "
      "file_created_date, file_modifed_date) "
      " select  file_name, file_size, file_full_path, file_ext, "
      "file_created_date, file_modifed_date from files"
      " where file_size in (select file_size from files group by file_size "
      "having count(*) >1)"
      " order by file_size desc ");

  //    qDebug() << dirs;
  //    qDebug() << files;

  //    while (!this->stopWork && !isInterruptionRequested()) {
  //        qDebug() << "Executed "<< i++ << " times";
  //        label->setText(QString("Executed %1 times").arg(i));
  //        this->sleep(1);
  //    }

  //        QString result;
  /* ... here is the expensive or blocking operation ... */
  //        emit resultReady(result);
}

void FileHashCalculatorThread::stopWorking() { this->stopWork = true; }
