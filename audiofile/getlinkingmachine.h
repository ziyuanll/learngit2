#ifndef GETLINKINGMACHINE_H
#define GETLINKINGMACHINE_H

#include <QObject>
#include <QThread>
#include <QTimer>
#include <QList>
#include <QVector>
#include <QVariant>
#include "header.h"

#define UIREFRESHTIME 1000

struct linkuser
{
    uint32_t num;
    QHostAddress addr;
};
Q_DECLARE_METATYPE(linkuser)


static QVector<linkuser> linkingbuff(NUMOFUSER);

class getLinkingMachine : public QObject
{
    Q_OBJECT
public:
    explicit getLinkingMachine(QObject *parent = 0);
    ~getLinkingMachine();
    linkuser userlinkingbuff[NUMOFUSER];
    linkuser userconfbuff[NUMOFUSER];
private:
    QThread* processLink;
    int linkin;
    clock_t clockbuff[NUMOFUSER];
    clock_t endclock;
    QTimer *getstatustime;
    headerinit *getlinksig;

signals:
    void allLinksig(QVector<linkuser>);

public slots:
    void linkingmachine(unsigned int linkingnum,QString ipaddr);

    void processlinkbuff();

};

#endif // GETLINKINGMACHINE_H
