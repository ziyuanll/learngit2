#ifndef MODBUSCLIENT_H
#define MODBUSCLIENT_H

#include "usernumtotype.h"
#include "testfivesecond.h"
#include "alsainit.h"
#include "audiofile/getlinkingmachine.h"
#include <QDialog>
#include <cmodbustcpslave.h>
#include <iostream>
#include <QTextCodec>
#include <stdio.h>
#include <mixervolume.h>
#include <QSettings>
#include <QVariant>
#include <QProcess>

class alsainit;

namespace Ui {
class ModbusClient;
}

class ModbusClient : public QDialog
{
    Q_OBJECT
    
public:
    explicit ModbusClient(QWidget *parent = 0);

    ~ModbusClient();
    
private slots:
    void on_btnStart_clicked();

    void ModbusDataChanged(unsigned int,int);

    void MobusDataRead(unsigned int,unsigned char *, int);

    void ModbusMoreDataChanged(unsigned int ,unsigned char* ,int);

    void DisplayStatus(QVector<linkuser>);

    void textChanged(int);

    void printfgetchar();

    void displayUser();

    void on_btnPlay_clicked();

    void on_pushButton_1_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_4_clicked();

    void on_pushButton_5_clicked();

    void on_pushButton_6_clicked();

    void on_pushButton_7_clicked();

    void on_pushButton_8_clicked();

    void on_pushButton_9_clicked();

    void on_pushButton_10_clicked();

    void on_pushButton_11_clicked();

    void on_pushButton_12_clicked();

    void on_pushButton_13_clicked();

    void on_pushButton_14_clicked();

    void on_pushButton_15_clicked();

    void on_pushButton_16_clicked();

    void on_pushButton_17_clicked();

    void on_pushButton_18_clicked();

    void on_pushButton_19_clicked();

    void on_pushButton_20_clicked();

    void on_volueAdd_clicked();

    void on_volueLess_clicked();

    void on_testbtnfivesecond_clicked();

    void setClientparameter(const uint32_t Num,const QString clinetIP);

    void testbtnlimit();

    void on_jiangzao_clicked();

signals:
//    void valueChanged(int);


private:
/*************************************设置变量************************************/
    Ui::ModbusClient *ui;
    CmodbusTcpSlave *mbtcpSlave;
    AlsaCaptureAndPlay *heartslot;
    alsainit *SetToModbus;
    mixervolume *alsamixer;
    testFiveSecond *testvoice;
    QPalette discon;
    QPalette enblecon;
    QString volstr;
    userNumToType *usernum;
    headerinit *voicept;
    QSettings *configIniRead;
    int voicevolume;
    bool testbtnrun;

    QPushButton *btnpoint[20];
    QString ClientIP[20];
    QString btntxt[20];

    headerinit *headthread;
    bool speexdnswitch;
/***************************************函数申请**********************************/
    void pointinit();

    void uisetinit();

public:
    CmodbusTcpSlave rmovechar;
    bool isStart;
    bool isStartcap;
    bool isStartplay;
    int apprun;
    QString modbusadd;

};

#endif // MODBUSCLIENT_H
