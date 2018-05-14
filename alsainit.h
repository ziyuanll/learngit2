#ifndef ALSAINIT_H
#define ALSAINIT_H


#include <QThread>
#include <QDebug>
#include <QObject>
#include <QUdpSocket>
#include <QTimer>
#include <QHostAddress>
#include <iostream>
#include <QMessageBox>
#include <QTextCodec>
#include <stdlib.h>
#include "audiofile/header.h"
#include "modbuscomply.h"
#include "audiofile/getlinkingmachine.h"
#include "audiofile/heartbeatreturn.h"

#define IPAdress "192.168.1.227" /*udp send IP address*/

class alsainit: public QObject
{
    Q_OBJECT
public:
    explicit alsainit();

    ~alsainit();

    void OpenTheMIC();

    void OpenTheUDPReceive();

    getLinkingMachine *userlinkbuffprocess;
private:
    bool isstart;
    headerinit *voicept;
    heartbeatreturn *heartdealwith;
    QThread capthread;

signals:
    void playthreadstart();
};

#endif // ALSAINIT_H
