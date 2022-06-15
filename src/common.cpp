#include "common.h"

const QSet<QString> Common::systemFiles = {"dll", "sys", "ocx", "drv", "dat"};
const QSet<QString> Common::photo = {"bmp", "gif", "jpg", "jpeg", "png", "tiff"};
const QSet<QString> Common::audio = {"mp3", "ogg"};
const QSet<QString> Common::video = {"avi", "wmv", "mp4"};

const QSet<QString> Common::archive = {"zip", "rar", "tar", "tar.gz", "jar", "war", "ear"};
const QSet<QString> Common::excel = {"xlsx", "xlsm"};
const QSet<QString> Common::executable = {"exe"};
const QSet<QString> Common::javascript = {"js"};
const QSet<QString> Common::pdf_documents = {"pdf"};
const QSet<QString> Common::php = {"php"};
const QSet<QString> Common::powerpoint = {"ppx", "pptx", "ppsx"};
const QSet<QString> Common::shellscript = {"bat", "sh"};
const QSet<QString> Common::word = {"doc", "docx", "rtf"};
const QSet<QString> Common::xml = {"xml", "xhtml"};
const QSet<QString> Common::web = {"html", "htm"};

QString Common::sizeFormat(long long val) {
    if (val / 1073741824 > 0)
        return QString("%1 GB").arg((val * 100 / 1073741824) * 0.01);
    if ((val / 1048576) > 0)
        return QString("%1 MB").arg((val * 100 / 1048576) * 0.01);
    if ((val / 1024) > 0) return QString("%1 KB").arg((val * 100 / 1024) * 0.01);
    return QString("%1 B").arg(val);
};

QSqlDatabase Common::db = QSqlDatabase::addDatabase("QSQLITE");
QSqlDatabase Common::hashDb = QSqlDatabase::addDatabase("QSQLITE", "hash_database_connection");

void Common::init() {
    /*
    Common::db = QSqlDatabase::addDatabase("QSQLITE");
    Common::hashDb = QSqlDatabase::addDatabase("QSQLITE",
    "hash_database_connection");
  */

    QString dbFileName = ":memory:";
    //    dbFileName = QDir::currentPath();
    //    dbFileName = dbFileName + "/files.sqlite";

    Common::db.setDatabaseName(dbFileName);
    Common::hashDb.setDatabaseName("files_hash.sqlite");

    if (!Common::db.open() || !Common::hashDb.open()) {
        QMessageBox::critical(
                nullptr, tr("Cannot open database"),
                tr("Unable to establish a database connection.\n"
                   "This example needs SQLite support. Please read "
                   "the Qt SQL driver documentation for information how "
                   "to build it.\n\n"
                   "Click Cancel to exit."),
                QMessageBox::Cancel);
        return;
    }
    QStringList tables = Common::db.tables();
    QSqlQuery query(Common::db);
    qDebug() << "Found Tables";
    qDebug() << tables;

    if (!Common::hashDb.tables().contains("files_hash")) {
        qDebug() << "Create table files_hash";
        QSqlQuery hquery(Common::hashDb);
        hquery.exec(
                "CREATE TABLE files_hash ("
                "id	INTEGER PRIMARY KEY AUTOINCREMENT,"
                "should_check	INTEGER default 0,"
                "file_hash	TEXT,"
                "file_full_path	TEXT"
                ");");

        hquery.exec(
                "CREATE UNIQUE INDEX files_full_path_uidx ON files_hash "
                "(file_full_path ASC);");
    }

    if (!tables.contains("files")) {
        qDebug() << "Create table files";
        query.exec(
                "CREATE TABLE files ("
                "id	INTEGER PRIMARY KEY AUTOINCREMENT,"
                "file_hash	TEXT,"
                "file_name	TEXT,"
                "file_size	integer,"
                "file_full_path	TEXT,"
                "file_ext	TEXT,"
                "file_created_date	date,"
                "file_modifed_date	date,"
                " add_date datetime "
                ");");
    }
    if (!tables.contains("results")) {
        qDebug() << "Create table results";
        query.exec(
                "CREATE TABLE results ("
                "id	INTEGER PRIMARY KEY AUTOINCREMENT,"
                "file_hash	TEXT,"
                "group_ida    int,"
                "file_name	TEXT,"
                "file_size	integer,"
                "file_full_path	TEXT,"
                "file_ext	TEXT,"
                "file_created_date	date,"
                "file_modifed_date	date,"
                " add_date datetime "
                ");");
    }
}
