#ifndef TESTFIVESECOND_H
#define TESTFIVESECOND_H

#include <QObject>
#include <QThread>
#include <QFile>
#include "audiofile/header.h"
class testFiveSecond : public QThread
{
    Q_OBJECT
protected:
    void run();
signals:
    void testover();
};

#endif // TESTFIVESECOND_H
