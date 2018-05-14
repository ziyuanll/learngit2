#ifndef HEARTBEATRETURN_H
#define HEARTBEATRETURN_H

#include <QObject>
#include <QThread>
#include "header.h"


class heartbeatreturn : public QObject
{
    Q_OBJECT
public:
    explicit heartbeatreturn(QObject *parent = 0);
    ~heartbeatreturn();
private:
    QThread* heartthread;
signals:
    void getIPLockNum(unsigned int,QString);
public slots:
    void getandputheart();
    
};

#endif // HEARTBEATRETURN_H
