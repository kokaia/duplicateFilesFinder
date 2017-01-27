#ifndef COMMON_H
#define COMMON_H

#include<QSet>
#include<QObject>
#include<QSqlDatabase>
#include<QMessageBox>
#include<QDebug>
#include<QSqlQuery>

class Common : public QObject
{
    Q_OBJECT
public:

    static const QSet<QString> archive;
    static const QSet<QString> audio;
    static const QSet<QString> excel;
    static const QSet<QString> executable;
    static const QSet<QString> javascript;
    static const QSet<QString> pdf_documents;
    static const QSet<QString> photo;
    static const QSet<QString> php;
    static const QSet<QString> powerpoint;
    static const QSet<QString> shellscript;
    static const QSet<QString> video;
    static const QSet<QString> word;
    static const QSet<QString> xml;
    static const QSet<QString> web;

    static QSqlDatabase db;
    static QSqlDatabase hashDb;


    static QString sizeFormat(long long val);
    static void init();

    static const QSqlDatabase getDb();

private:
    explicit Common();



signals:

public slots:
};

#endif // COMMON_H
