#ifndef COMMON_UTIL_H
#define COMMON_UTIL_H

#include <QObject>

class CommonUtil : public QObject
{
    Q_OBJECT
public:
    explicit CommonUtil(QObject *parent = nullptr);

signals:

public slots:
};

#endif // COMMON_UTIL_H
