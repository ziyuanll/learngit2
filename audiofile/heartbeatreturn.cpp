#include "heartbeatreturn.h"

heartbeatreturn::heartbeatreturn(QObject *parent) :
    QObject(parent)
{
    heartthread=new QThread;
    this->moveToThread(heartthread);
    heartthread->start();
}

heartbeatreturn::~heartbeatreturn()
{
    heartthread->quit();
    heartthread->wait();
}

void heartbeatreturn::getandputheart()
{
    int n=0,k=0;
    QHostAddress addressip;
    uint port;
    int n_heart=sizeof(udphead);

    printf("++++++++++heart will be handle++++++++++++\n");
    qDebug() << "心跳::: " << QThread::currentThreadId();
    while(1)
    {
        while(play.udpsocket->hasPendingDatagrams())
        {
            printf("++++ \n");
            /*read the audio data and play. */
            n=play.udpsocket->pendingDatagramSize();
            if(n==n_heart)
            {
                play.udpsocket->readDatagram((char*)&play.recheat,n,&addressip,(quint16*)&port);
                //if(k==n_heart)//正在接听
                printf("%s ",addressip.toString().toAscii().data());
                emit getIPLockNum((unsigned int)play.recheat.ipclock,addressip.toString());
                if((ipsingleclock==0x00)||(ipsingleclock==play.recheat.ipclock))
                {

                    if(play.recheat.contrlaudioandplay==0x01)//打开通话
                    {
                        controlval=play.recheat.status;/*//0x11通话,0x00空闲*/
                        ipsingleclock=play.recheat.ipclock;
                    }
                    if(play.recheat.contrlaudioandplay==0x10)//关闭通话
                    {
                        controlval=play.recheat.status;/*//0x11通话,0x00空闲*/
                        //                        ipsingleclock=0x00;       //不注释会出现client无法关闭自己
                    }
                }

                play.recheat.status=controlval;
                play.recheat.ipclock=ipsingleclock;

                k=cap.udpsocket->writeDatagram((char *)&play.recheat,n,addressip,port);
                if((k!=n))
                {
                    printf("imformation send error,k=%d n=%d !\n",k,n);
                }
                if(ipsingleclock==play.recheat.ipclock)
                {
                    if((play.recheat.contrlaudioandplay==0x10)||(btncontrl==0x11))//关闭通话
                    {
                        ipsingleclock=0x00;
                        btncontrl=0x00;
                        MSpeak.usernum=(uint16_t)ipsingleclock;
                    }
                }
//                continue;
            }
            else if(!play.run)
            {
                printf("++\n");
                play.udpsocket->readDatagram((char*)&play.datapack,n,&addressip,(quint16*)&port);
            }
        }
    }
    controlval=0x00;
}
