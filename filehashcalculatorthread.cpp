#include "filehashcalculatorthread.h"
#include "ui_mainwindow.h"

FileHashCalculatorThread::FileHashCalculatorThread(const QStringList &dirs, TFilterWidget *filtersWidget,
                                                   CompareMode fileCompareMode)
    : filtersWidget(filtersWidget), dirs(dirs), fileCompareMode(fileCompareMode) {
  qsrand(static_cast<uint>(QTime::currentTime().msec()));
  timerEverySecond = new QTimer(this);
  QObject::connect(timerEverySecond, SIGNAL(timeout()), this, SLOT(updateTimeSinceStart()));
  timerEverySecond->start(1000);
}

FileHashCalculatorThread::~FileHashCalculatorThread() {
    delete timerEverySecond;
//    delete &dirs;
//    delete &fileCompareMode;
//    delete &stopWork;
}

int FileHashCalculatorThread::getRandom() { return qrand() % 100 + 100; }

void FileHashCalculatorThread::run() {
  qDebug() << "Start Calculating Hashs for Directories";
  qDebug() << dirs;

  int dirsSize = dirs.size();
  int i = 0, filesCount = 0;
  timeSinceStart = 0;
  qint64 filesSumSize = 0;
  int filesSumSizeInt = 0;
  qint64 filesCurSize = 0;
  int filesCurSizeInt = 0;
  QStringList files, curdirList;
  QFileInfoList curFilesList;
  emit setTopLabel(tr("Scanning folders and files"));
  int nextPaint = 1;

  qint64 sizeFrom = filtersWidget->getSizeFrom();
  if (sizeFrom > -1) qDebug() << "File Size limit from " << sizeFrom << "Bytes ";
  qint64 sizeTo = filtersWidget->getSizeTo();
  if (sizeTo > -1) qDebug() << "File Size limit   to " << sizeTo   << "Bytes ";

  QDir::Filters filter = filtersWidget->getFilters();

  QSet<QString> currentSearchExt =  filtersWidget->getFilterExtentions();



  if (currentSearchExt.size()>0) {
      qDebug() << "Filter by extentions " << currentSearchExt;
  }



  while (!this->stopWork && !isInterruptionRequested() && i < dirsSize) {
    QDir curDir(dirs.at(i++));
    curdirList.clear();
    curdirList << curDir.entryList(QDir::Dirs | filter );
    foreach (QString var, curdirList) {
      dirs << curDir.absolutePath() + "/" + var;
      dirsSize++;
    }
    //        dirsSize = dirs.length();
    curFilesList.clear();
    curFilesList << curDir.entryInfoList(QDir::Files | filter );
    foreach (QFileInfo var, curFilesList) {
      bool addIt = true;
      if (sizeFrom> -1) addIt &= var.size() >= sizeFrom;
      if (addIt && sizeTo > -1)  addIt &= var.size() <= sizeTo;


      if (currentSearchExt.size()>0) {
          addIt &= currentSearchExt.contains(var.suffix().toLower());
      }
      if (addIt) {
        files << var.absoluteFilePath();
        filesCount++;
        filesSumSize+=var.size();
      }
    }
    if (i > nextPaint) {
      emit setBottomLabel(QString(tr("Found %1 folders and %2 files"))
                              .arg(dirsSize - 1)
                              .arg(filesCount - 1));
      nextPaint = nextPaint + getRandom();
    }
  }

  emit setTopLabel(QString(tr("Found %1 folders and %2 files"))
                       .arg(dirsSize)
                       .arg(filesCount));
  emit setProgressbarMaximumValue(filesCount);
  filesSumSizeInt = static_cast<int> (filesSumSize >> 8 );
  emit setProgressBarSizeMaximumValue( filesSumSizeInt );
  i = 0;
  QSqlQuery query(Common::db);

  query.exec("delete from files");
  query.exec("delete from results");
  query.prepare(
      "INSERT INTO files( file_full_path, file_name, file_size, "
      "file_ext, file_created_date, file_modifed_date, add_date)"
      " VALUES "
      "(:file_full_path,:file_name,:file_size,:file_ext,:file_"
      "created_date,:file_modifed_date, datetime('now') )");

  nextPaint = 1;
  while (!this->stopWork && !isInterruptionRequested() && i < filesCount) {
    QFile file(files[i++]);
    if (!file.exists()) {
      continue;
    }

    filesCurSize += file.size();
    if (i > nextPaint || i + 5 > filesCount) {
      emit setProgressbarValue(i);
      emit updateApproximateTime((filesCount  * timeSinceStart) / i - timeSinceStart);
      filesCurSizeInt = static_cast<int> (filesCurSize >> 8 );
      emit setProgressBarSizeValue(filesCurSizeInt);

      emit setBottomLabel(tr("Current processing #%1, file=%2")
                              .arg(i)
                              .arg(file.fileName().size() > 120
                                       ? "... " + file.fileName().right(120)
                                       : file.fileName()));
      nextPaint = nextPaint + getRandom();
    }

    QFileInfo fileInfo = QFileInfo(file);

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
  emit setProgressBarSizeValue( filesSumSizeInt );

  query.exec(
      "INSERT INTO results( file_name, file_size, file_full_path, file_ext, "
      "file_created_date, file_modifed_date) "
      " select  file_name, file_size, file_full_path, file_ext, "
      "file_created_date, file_modifed_date from files"
      " where file_size in (select file_size from files group by file_size "
      "having count(*) >1)"
      " order by file_size desc ");

  if (fileCompareMode & Md5) {
    calculateResultsMd5AndRemoveUniq();
  }
}

void FileHashCalculatorThread::calculateResultsMd5AndRemoveUniq() {
  timeSinceStart = 0;
  QSqlQuery query(Common::db);

  query.exec("SELECT count(*) cou, sum(file_size) ssize FROM results ");

  query.first();
  int filesCount = query.value(0).toInt();
  qint64 filesCurSize = 0, filesCurSizeDelta = 0;
  int filesCurSizeInt = 0;
  qint64 filesSumSize = query.value(1).toLongLong();
  int filesSumSizeInt = static_cast<int> ( filesSumSize >> 8 );

  qDebug() << "Found " << filesCount << " items";
  qDebug() << "Found files sum size " << filesSumSize << " Byte";

  emit setProgressbarValue(0);
  emit setProgressBarSizeValue(0);
  emit setProgressbarMaximumValue(filesCount);
  emit setProgressBarSizeMaximumValue(filesSumSizeInt);

  query.exec("SELECT id, file_full_path FROM results ");
  QSqlQuery updateQuery(Common::db);

  updateQuery.prepare("UPDATE results SET file_hash =:file_hash WHERE id=:id");

  int curfilesCount = 0;
  while (!this->stopWork && !isInterruptionRequested() && query.next()) {
    int file_id = query.value(0).toInt();
    QString file_full_path = query.value(1).toString();

    QFile file(file_full_path);
    if (!file.exists()) {
      continue;
    }

    bool fromCache = false;
    QString hashData = this->getFileHash(file_full_path, &file, fromCache);
    if (fromCache){
      filesCurSizeDelta += file.size();
      filesSumSize -= file.size();
    }

    updateQuery.bindValue(":id", file_id);
    updateQuery.bindValue(":file_hash", hashData);
    updateQuery.exec();
    if (updateQuery.lastError().isValid()) {
      qDebug() << updateQuery.lastError().text();
    }
    emit setProgressbarValue(curfilesCount++);
    filesCurSize += file.size();
    filesCurSizeInt = static_cast<int> (filesCurSize >> 8 );
    emit setProgressBarSizeValue(filesCurSizeInt);
    if (filesCurSize > filesCurSizeDelta) {
      emit updateApproximateTime(static_cast<int> ((filesSumSize  * timeSinceStart) / (filesCurSize - filesCurSizeDelta)) - timeSinceStart);
    }
    emit setBottomLabel(tr("Current processing #%1, from %2 file=%3")
                            .arg(curfilesCount)
                            .arg(filesCount)
                            .arg(file.fileName().size() > 120
                                     ? "... " + file.fileName().right(120)
                                     : file.fileName()));
  }
  query.exec("delete from results where file_hash is null");
  query.exec(
      "delete from results where file_hash in (select file_hash from results "
      "group by file_hash having count(*) =1 )");
}

void FileHashCalculatorThread::stopWorking() { this->stopWork = true; }

QString FileHashCalculatorThread::getFileHash(const QString &file_full_path, QFile *file, bool &fromCache) {
  QSqlQuery query(Common::hashDb);
  query.prepare(
      "SELECT file_hash FROM files_hash where file_full_path = "
      ":file_full_path ");
  query.bindValue(":file_full_path", file_full_path);
  query.exec();

  while (query.next()) {
    fromCache = true;
    return query.value(0).toString();
  }

  file->open(QIODevice::ReadOnly);
  QString hashData =
      QCryptographicHash::hash(file->readAll(), QCryptographicHash::Md5)
          .toHex();

  query.prepare("INSERT INTO files_hash(file_hash, file_full_path) VALUES(:file_hash, :file_full_path)");
  query.bindValue(":file_full_path", file_full_path);
  query.bindValue(":file_hash", hashData);
  query.exec();

  if (query.lastError().isValid()) {
    qDebug() << query.lastError().text();
  }
  fromCache = false;
  return hashData;
}

void FileHashCalculatorThread::updateTimeSinceStart() { ++timeSinceStart; }
