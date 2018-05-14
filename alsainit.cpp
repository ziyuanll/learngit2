#include "alsainit.h"

//将语音代码创建connect，播放和录音。

alsainit::alsainit()
{
    bool result;
    play.run = false;
    play.j = 0;
    play.port = 5555;
    cap.port = 5555;
    play.frames = BLOCK_SIZE / 2;
    cap.udpsocket = play.udpsocket;
    cap.send_address.setAddress("192.168.1.227");//目的地址的ip

    voicept= new headerinit();
    heartdealwith= new heartbeatreturn();
    userlinkbuffprocess=new getLinkingMachine;
    result=play.udpsocket->bind(play.port);
    if(!result)
    {
        printf("udp socket create error\n");
        return;
    }

    voicept->initcap();
    voicept->initplay();

    QObject::connect(this,SIGNAL(playthreadstart()),voicept,SLOT(dataReceived()),Qt::BlockingQueuedConnection);
    QObject::connect(heartdealwith,SIGNAL(getIPLockNum(unsigned int,QString)),userlinkbuffprocess,SLOT(linkingmachine(unsigned int,QString)),Qt::BlockingQueuedConnection);
    QObject::connect(this,SIGNAL(playthreadstart()),heartdealwith,SLOT(getandputheart()),Qt::BlockingQueuedConnection);
    emit playthreadstart();//播放线程打开

    printf("play thread open\n");
    isstart =true;
    play.run=false;
    cap.run=false;
}

alsainit::~alsainit()
{
    voicept->end();
    delete play.udpsocket;
}

void alsainit::OpenTheMIC()
{
    /*open the record pthread*/
    if(pthread_create(&capturethread,NULL,caphwdata,NULL)<0)
    {
        cap.run=false;
    }
    printf("Open the capture is error\n");

}


