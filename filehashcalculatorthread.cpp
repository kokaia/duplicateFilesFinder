#include "filehashcalculatorthread.h"

FileHashCalculatorThread::FileHashCalculatorThread(QStringList dirs,
                                                   CompareMode fileCompareMode)
    : dirs(dirs), fileCompareMode(fileCompareMode) {
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
  i = 0;
  QSqlQuery query(Common::db);
  //    QSqlQuery query1(*db);

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

    if (i > nextPaint || i + 5 > filesCount) {
      emit setProgressbarValue(i);
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
  QSqlQuery query(Common::db);

  query.exec("SELECT count(*) FROM results ");

  query.first();
  int filesCount = query.value(0).toInt();

  qDebug() << "Found " << filesCount << " items";

  emit setProgressbarValue(0);
  emit setProgressbarMaximumValue(filesCount);

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

    QString hashData = this->getFileHash(file_full_path, &file);

    updateQuery.bindValue(":id", file_id);
    updateQuery.bindValue(":file_hash", hashData);
    updateQuery.exec();
    if (updateQuery.lastError().isValid()) {
      qDebug() << updateQuery.lastError().text();
    }
    emit setProgressbarValue(curfilesCount++);
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

QString FileHashCalculatorThread::getFileHash(const QString &file_full_path,
                                              QFile *file) {
  QSqlQuery query(Common::hashDb);
  query.prepare(
      "SELECT file_hash FROM files_hash where file_full_path like "
      ":file_full_path ");
  query.bindValue(":file_full_path", file_full_path);
  query.exec();

  // qDebug() << QString("SELECT file_hash FROM files_hash where file_full_path
  // like'%1'").arg(file_full_path);
  while (query.next()) {
    return query.value(0).toString();
  }

  file->open(QIODevice::ReadOnly);
  QString hashData =
      QCryptographicHash::hash(file->readAll(), QCryptographicHash::Md5)
          .toHex();

  query.prepare(
      "INSERT INTO files_hash(file_hash, file_full_path) "
      " VALUES(:file_hash, :file_full_path)");

  query.bindValue(":file_full_path", file_full_path);
  query.bindValue(":file_hash", hashData);
  query.exec();

  if (query.lastError().isValid()) {
    qDebug() << query.lastError().text();
  }
  return hashData;
}
