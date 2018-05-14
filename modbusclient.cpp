#include "modbusclient.h"
#include "ui_modbusclient.h"
#include <stdlib.h>


#include <QMessageBox>

ModbusClient::ModbusClient(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ModbusClient)
{
    QString versionnum="1.0.0.0:0218";
    QString versionfont="版本号:";
    bool ok;
    setWindowFlags(Qt::FramelessWindowHint);//去除标题条
    QTextCodec::setCodecForTr(QTextCodec::codecForName("utf8"));
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("utf8"));
    QProcess *setipaddr=new QProcess();
    //setipaddr->start("ifconfig",QStringList()<<"eth0"<<"192.168.1.225");//设置IP地址
    ui->setupUi(this);
    ui->label_versionnum->setText(tr((versionfont+versionnum).toLatin1().data()));

    //各个类的实例化
    heartslot=new AlsaCaptureAndPlay();
    mbtcpSlave=new CmodbusTcpSlave();
    play.udpsocket=new QUdpSocket(this);
    SetToModbus=new alsainit();
    alsamixer =new mixervolume();
    headthread=new headerinit();
    usernum=new userNumToType();
    testvoice=new testFiveSecond();
    configIniRead = new QSettings("confserver.ini", QSettings::IniFormat);
    qRegisterMetaType<linkuser>("linkuser");
    qRegisterMetaType<linkuser*>("linkuser[20]");

    //将读取到的ini文件保存在QString中，先取值，然后通过toString()函数转换成QString类型
    this->voicevolume=configIniRead->value("/APPSet/Volume").toString().toInt();
    modbusadd=configIniRead->value("/APPSet/ServerAddress").toString();
    setipaddr->start("ifconfig",QStringList()<<"eth0"<<modbusadd.toLocal8Bit().data());//设置IP地址

    pointinit();//初始化指针

    for(int i=0;i<NUMOFUSER;i++)
    {
        usernum->numBuff[i]=configIniRead->value("/ClientNum/Num"+QString::number(i+1,10)).toString().toInt(&ok,16);
        this->ClientIP[i]=configIniRead->value("/ClientAddr/Add"+QString::number(i+1,10)).toString();
        this->btntxt[i]=configIniRead->value("/ClintBtnTxt/BtnTxT"+QString::number(i+1,10)).toString();
        if((usernum->numBuff[i]==0x00)||(this->ClientIP[i]==NULL)||(this->btntxt[i]==NULL))
        {
            for(;i<NUMOFUSER;i++)
            {
                btnpoint[i]->setVisible(false);
            }
            break;
        }
        this->SetToModbus->userlinkbuffprocess->userconfbuff[i].addr.setAddress(this->ClientIP[i]);
        this->SetToModbus->userlinkbuffprocess->userconfbuff[i].num=usernum->numBuff[i];
    }


    connect(mbtcpSlave,SIGNAL(HRValueOnRead(unsigned int,unsigned char*,int)),
            this,SLOT(MobusDataRead(unsigned int,unsigned char *, int)));
    connect(mbtcpSlave,SIGNAL(HRValueOnChanged(unsigned int,int)),
            this,SLOT(ModbusDataChanged(unsigned int,int)));
    connect(mbtcpSlave,SIGNAL(HRMoreValueOnChanged(unsigned int,unsigned char*,int)),
            this,SLOT(ModbusMoreDataChanged(unsigned int,unsigned char*,int)));
    connect(ui->spinBox,SIGNAL(valueChanged(int)),this,SLOT(textChanged(int)));
    connect(mbtcpSlave,SIGNAL(coutchar()),this,SLOT(printfgetchar()));
    connect(SetToModbus->userlinkbuffprocess,SIGNAL(allLinksig(QVector<linkuser>)),this,SLOT(DisplayStatus(QVector<linkuser>)));
    connect(testvoice,SIGNAL(testover()),this,SLOT(testbtnlimit()));

    mbtcpSlave->ModbusInit();
    mbtcpSlave->Enabled=true;
    isStart=true;
    isStartcap=true;
    isStartplay=true;
    testbtnrun=false;
    speexdnswitch=true;

    if(voicevolume==0)
    {
        QVariant vol;
        volstr=QString::number(alsamixer->volumeval,10);
        ui->label->setText(volstr.toAscii());
        vol=QVariant(volstr);
        configIniRead->setValue("APPSet/Volume",vol);
        delete configIniRead;
        MSpeak.volume=volstr.toInt(&ok,16);
    }
    else if(voicevolume>0)
    {
        alsamixer->volume_change(voicevolume,voicevolume);
        volstr=QString::number(alsamixer->volumeval,10);
        ui->label->setText(volstr.toAscii());
        delete configIniRead;
    }
    else
    {
        ui->volueAdd->setEnabled(false);
        ui->volueLess->setEnabled(false);
        delete configIniRead;
    }
    mbtcpSlave->start();
    ui->btnStart->setText(tr("停止"));

    /*you must open the udp*/
    if(!MSpeak.machSwitch)
    {
        ui->btnPlay->setText(tr("现在\n静音"));
    }

    //打开录音
    cap.run=true;
    isStartcap=false;
    SetToModbus->OpenTheMIC();

    apprun=0;
    discon.setColor(QPalette::ButtonText,Qt::red);
    enblecon.setColor(QPalette::ButtonText,Qt::black);

    uisetinit();
    ui->jiangzao->setVisible(false);
}

ModbusClient::~ModbusClient()
{
    printf("******* %p\n",&mbtcpSlave);
    printf("******* %p\n",&heartslot);
    printf("******* %p\n",&SetToModbus);
    delete SetToModbus;
    delete heartslot;

    testvoice->terminate();
    testvoice->wait();
    mbtcpSlave->terminate();
    mbtcpSlave->wait();
    delete ui;
}

void ModbusClient::on_btnStart_clicked()
{
    if(!isStart)
    {
        isStart=true;
        mbtcpSlave->Enabled=true;
        mbtcpSlave->start();
        ui->btnStart->setText(tr("开始"));
    }
    else
    {
        isStart=false;
        mbtcpSlave->Enabled=false;
        ui->btnStart->setText(tr("停止"));
    }
}

void ModbusClient::MobusDataRead(unsigned int iAdd, unsigned char *iRecAdd,int iValue)
{
    *iRecAdd = 2*iValue;
    for(int i=0; i<iValue; i++)
    {
        *(iRecAdd+1+2*i)=*(RegAdd+iAdd+2*i);
        *(iRecAdd+2+2*i)=*(RegAdd+iAdd+2*i+1);
        printf("%02x ",*(RegAdd+iAdd+2*i));
        printf("%02x ",*(RegAdd+iAdd+2*i+1));
    }
    printf("\n");
}

/********************************modbus control fun***************************/
void ModbusClient::ModbusDataChanged(unsigned int iAdd, int iValue)
{
    int temp=*(RegAdd+iAdd);
    *(RegAdd+iAdd)=iValue;
    ui->lineEditAdd->setText(QString("4000%1").arg(iAdd+1));
    ui->lineEditValue->setText(QString("%1").arg(iValue));
    ui->spinBox->setValue(iValue);

    printf("结构体输出2\n");
    for(uint i=0;i<sizeof(MSpeak);i++)
    {
        printf("%02x ",*(RegAdd+i));
    }
    printf("\n");

    if(temp!=iValue)
    {

        switch(iAdd)
        {
        case 0x02:
            if(*(RegAdd+iAdd)==0x11)
            {
                if((MSpeak.usernum==0x00)&&(MSpeak.testfivesecond==0x00))
                {
                    MSpeak.machSwitch=0x00;
                    break;
                }
                uint32_t runnum=(uint32_t)MSpeak.usernum;
                for(int i=0;i<NUMOFUSER;i++)
                {
                    if((usernum->numBuff[i]==runnum)&&(usernum->numBuff[i]!=0x00))
                    {
                        setClientparameter(usernum->numBuff[i],ClientIP[i]);
                        break;
                    }
                    if(i==NUMOFUSER)
                    {
                        MSpeak.machSwitch=0x00;
                    }
                }
            }
            else if(*(RegAdd+iAdd)==0x00)
            {
                on_btnPlay_clicked();
                MSpeak.machSwitch=(uint8_t)controlval;
                MSpeak.usernum=ipsingleclock;
            }
            break;
        case 0x03:
            if((*(RegAdd+iAdd)==0x11)&&(controlval==0x00))
            {
                on_testbtnfivesecond_clicked();
            }
            break;
        case 0x04:
            if(*(RegAdd+iAdd)!=alsamixer->volumeval)
            {
                if(*(RegAdd+iAdd)>(uint8_t)100)
                {
                    *(RegAdd+iAdd)=100;
                }
                if(*(RegAdd+iAdd)<(uint8_t)1)
                {
                    *(RegAdd+iAdd)=0;
                }
                alsamixer->volumeset((long)*(RegAdd+iAdd),(long)*(RegAdd+iAdd));
                volstr=QString::number(alsamixer->volumeval,10);
                ui->label->setText(volstr.toAscii());
            }
            break;
        case 0x05:
            if(*(RegAdd+iAdd)==0x11)
            {
                speexdnswitch=true;
                on_jiangzao_clicked();
            }
            else if(*(RegAdd+iAdd)==0x00)
            {
                speexdnswitch=false;
                on_jiangzao_clicked();
            }

            break;
        case 0x06: break;
        default:
            break;
        }
    }
}
/********************************modbus control end***************************/

void ModbusClient::ModbusMoreDataChanged(unsigned int iAdd,unsigned char *iValue,int iSize)
{
    for(int i=0;i<iSize;i+=2)
    {
        if(MSpeak.machSwitch==0x11)
        {
            break;
        }
//        printf("isize=%d val=%02x\n",i,MODBUS_GET_INT16_FROM_INT8(iValue,i));
        if((RegAdd+iAdd+i/2)==(unsigned char *)&(MSpeak.machSwitch))
        {
            continue;
        }
        if((RegAdd+iAdd+i/2)==(unsigned char *)&(MSpeak.volume))
        {
            continue;
        }
        if((RegAdd+iAdd+i/2)==(unsigned char *)&(MSpeak.testfivesecond))
        {
            continue;
        }
        if((RegAdd+iAdd+i/2)==(unsigned char *)&(MSpeak.delniseSwitch))
        {
            continue;
        }
        *(RegAdd+iAdd+i/2)=MODBUS_GET_INT16_FROM_INT8(iValue,i);
    }
}

void ModbusClient::textChanged(int iValue)
{
    std::cout<<"*******"<<iValue<<"*******"<<std::endl;
    if (mbtcpSlave != NULL)
    {
//        *RegAdd=iValue;
        mbtcpSlave->SetHRValuebyAdd(ui->lineEditAdd->text().toInt(),iValue);
    }
}

void ModbusClient::printfgetchar()
{
    ui->textEdit->setText("");
    for(int i=0;i<12;i++)
    {
        //以16进制输出到控件中
        ui->textEdit->insertPlainText(QString("%1 ").arg(recdata[i]&0xFF,2,16,QLatin1Char('0')));
    }
}

void ModbusClient::displayUser()
{
//    printf("%s \n",*(char*)play.recv_address);
}

/**********************************client function****************************/
void ModbusClient::on_btnPlay_clicked()
{
    btncontrl=0x11;
    controlval=0x00;
    MSpeak.machSwitch=controlval;
    ui->btnPlay->setText(tr("现在\n静音"));
//    ui->pushButton->setText(tr("user1"));
    for(int i=0;i<NUMOFUSER;i++)
    {
        btnpoint[i]->setText(tr(btntxt[i].toLocal8Bit().data()));
        if(btntxt[i]==NULL)
        {
            btnpoint[i]->setVisible(false);
        }
    }
    printf("-----------\n");

}

void ModbusClient::on_pushButton_1_clicked()
{
    setClientparameter(usernum->numBuff[0],ClientIP[0]);
}

void ModbusClient::on_pushButton_2_clicked()
{
    setClientparameter(usernum->numBuff[1],ClientIP[1]);
}

void ModbusClient::on_pushButton_3_clicked()
{
    setClientparameter(usernum->numBuff[2],ClientIP[2]);
}

void ModbusClient::on_pushButton_4_clicked()
{
    setClientparameter(usernum->numBuff[3],ClientIP[3]);
}

void ModbusClient::on_pushButton_5_clicked()
{
    setClientparameter(usernum->numBuff[4],ClientIP[4]);
}

void ModbusClient::on_pushButton_6_clicked()
{
    setClientparameter(usernum->numBuff[5],ClientIP[5]);
}

void ModbusClient::on_pushButton_7_clicked()
{
    setClientparameter(usernum->numBuff[6],ClientIP[6]);
}

void ModbusClient::on_pushButton_8_clicked()
{
    setClientparameter(usernum->numBuff[7],ClientIP[7]);
}

void ModbusClient::on_pushButton_9_clicked()
{
    setClientparameter(usernum->numBuff[8],ClientIP[8]);
}

void ModbusClient::on_pushButton_10_clicked()
{
    setClientparameter(usernum->numBuff[9],ClientIP[9]);
}

void ModbusClient::on_pushButton_11_clicked()
{
    setClientparameter(usernum->numBuff[10],ClientIP[10]);
}

void ModbusClient::on_pushButton_12_clicked()
{
    setClientparameter(usernum->numBuff[11],ClientIP[11]);
}

void ModbusClient::on_pushButton_13_clicked()
{
    setClientparameter(usernum->numBuff[12],ClientIP[12]);
}

void ModbusClient::on_pushButton_14_clicked()
{
    setClientparameter(usernum->numBuff[13],ClientIP[13]);
}

void ModbusClient::on_pushButton_15_clicked()
{
    setClientparameter(usernum->numBuff[14],ClientIP[14]);
}

void ModbusClient::on_pushButton_16_clicked()
{
    setClientparameter(usernum->numBuff[15],ClientIP[15]);
}

void ModbusClient::on_pushButton_17_clicked()
{
    setClientparameter(usernum->numBuff[16],ClientIP[16]);
}

void ModbusClient::on_pushButton_18_clicked()
{
    setClientparameter(usernum->numBuff[17],ClientIP[17]);
}

void ModbusClient::on_pushButton_19_clicked()
{
    setClientparameter(usernum->numBuff[18],ClientIP[18]);
}

void ModbusClient::on_pushButton_20_clicked()
{
    setClientparameter(usernum->numBuff[19],ClientIP[19]);
}

void ModbusClient::on_volueAdd_clicked()
{
    if(alsamixer->volumeval<100)
    {

        alsamixer->volumeval++;
        alsamixer->volumeset(alsamixer->volumeval,alsamixer->volumeval);
        volstr=QString::number(alsamixer->volumeval,10);
        ui->label->setText(volstr.toAscii());
    }
}

void ModbusClient::on_volueLess_clicked()
{
    if(alsamixer->volumeval>0)
    {
        alsamixer->volumeval--;
        alsamixer->volumeset(alsamixer->volumeval,alsamixer->volumeval);
        volstr=QString::number(alsamixer->volumeval,10);
        ui->label->setText(volstr.toAscii());
    }
}

void ModbusClient::on_testbtnfivesecond_clicked()
{
    MSpeak.testfivesecond=0x11;
    ui->testbtnfivesecond->setEnabled(false);
    testbtnrun=true;
    for(int i=0;i<NUMOFUSER;i++)
    {
        btnpoint[i]->setEnabled(false);
    }
    on_btnPlay_clicked();
    testvoice->start();
}
/************************************end**************************************/

/***********************************slots function****************************/
void ModbusClient::DisplayStatus(QVector<linkuser> linkingbuff)
{
    static uint32_t conname=0x00;
    for(int i=0;i<NUMOFUSER;i++)
    {
        if((ClientIP[i]!=NULL)&&(usernum->numBuff[i]!=0x00))
        {
            if(linkingbuff[i].addr.toString()==NULL)
            {
                btnpoint[i]->setPalette(discon);
                btnpoint[i]->setEnabled(false);
            }
            else
            {
                btnpoint[i]->setPalette(enblecon);
                btnpoint[i]->setEnabled(true);
            }
        }

        if(ipsingleclock!=0x00)
        {
            if(usernum->numBuff[i]==ipsingleclock)
            {
                if(controlval==0x11)
                {
                    btnpoint[i]->setText(tr("通话"));
                    ui->btnPlay->setText(tr("点击\n关闭"));
                    cap.send_address.setAddress(ClientIP[i]);
                    play.run=true;
                    cap.run=true;
                    conname=ipsingleclock;
                }
            }
            else
            {
                btnpoint[i]->setEnabled(false);
            }
        }
        else
        {
            if(controlval==0x00)
            {
                play.run=false;
                cap.run=false;
                if((conname==usernum->numBuff[i])&&(conname!=0x00))
                {
                    btnpoint[i]->setText(tr(btntxt[i].toLocal8Bit().data()));
                    ui->btnPlay->setText(tr("现在\n静音"));
                    conname=0x00;
                    for(int j=0;j<NUMOFUSER;j++)
                    {
                        btnpoint[j]->setEnabled(true);
                    }
                }
            }
            else
            {
                controlval=0x00;
            }
        }
    }
/***************************程序运行时动态显示的一个数*****************************/
    if(apprun==100)
    {
        apprun=0;
    }
    ui->label_2->setText(QString("%1").arg(apprun));
    apprun++;
/**************************************end*************************************/
/*********************************避免和5秒录音检测时冲突**************************/
    if(controlval)
    {
        ui->testbtnfivesecond->setEnabled(false);
    }
    else if(testbtnrun==false)
    {
        ui->testbtnfivesecond->setEnabled(true);
    }
/**************************************end*************************************/
}

void ModbusClient::setClientparameter(const uint32_t Num,const QString clinetIP)
{
    ipsingleclock=Num;
    cap.send_address.setAddress(clinetIP);
    controlval=0x11;

    MSpeak.usernum=(uint16_t)Num;
    MSpeak.machSwitch=(uint8_t)controlval;
}

void ModbusClient::testbtnlimit()
{
    ui->testbtnfivesecond->setEnabled(true);
    for(int i=0;i<NUMOFUSER;i++)
    {
        btnpoint[i]->setEnabled(true);
    }
    MSpeak.testfivesecond=0x00;
}

/********************************slots functiong end**************************/

void ModbusClient::pointinit()
{
    btnpoint[0]=ui->pushButton_1;
    btnpoint[1]=ui->pushButton_2;
    btnpoint[2]=ui->pushButton_3;
    btnpoint[3]=ui->pushButton_4;
    btnpoint[4]=ui->pushButton_5;
    btnpoint[5]=ui->pushButton_6;
    btnpoint[6]=ui->pushButton_7;
    btnpoint[7]=ui->pushButton_8;
    btnpoint[8]=ui->pushButton_9;
    btnpoint[9]=ui->pushButton_10;
    btnpoint[10]=ui->pushButton_11;
    btnpoint[11]=ui->pushButton_12;
    btnpoint[12]=ui->pushButton_13;
    btnpoint[13]=ui->pushButton_14;
    btnpoint[14]=ui->pushButton_15;
    btnpoint[15]=ui->pushButton_16;
    btnpoint[16]=ui->pushButton_17;
    btnpoint[17]=ui->pushButton_18;
    btnpoint[18]=ui->pushButton_19;
    btnpoint[19]=ui->pushButton_20;
}

void ModbusClient::uisetinit()
{
    for(int i=0;i<NUMOFUSER;i++)
    {
        btnpoint[i]->setText(tr(btntxt[i].toLocal8Bit().data()));
    }
}

void ModbusClient::on_jiangzao_clicked()
{
    if(speexdnswitch)
    {
        ui->jiangzao->setText("降噪开");
        denoiseswitch=true;
        speexdnswitch=false;
        MSpeak.delniseSwitch=0x11;
    }
    else
    {
        ui->jiangzao->setText("降噪关");
        denoiseswitch=false;
        speexdnswitch=true;
        MSpeak.delniseSwitch=0x00;
    }
}
