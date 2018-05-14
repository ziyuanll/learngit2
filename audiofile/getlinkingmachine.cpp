#include "getlinkingmachine.h"

getLinkingMachine::getLinkingMachine(QObject *parent) :
    QObject(parent)
{
    qRegisterMetaType<QVector<linkuser> >("QVector<linkuser>");
    linkin=0;

    getstatustime = new QTimer;
    for(int i=0;i<NUMOFUSER;i++)
    {
        userlinkingbuff[i].addr.setAddress("");
        userlinkingbuff[i].num=0;
        userconfbuff[i].addr.setAddress("");
        userconfbuff[i].num=0;
        linkingbuff[i].addr.setAddress("");
        linkingbuff[i].num=0;
    }
    processLink = new QThread;
    this->moveToThread(processLink);
    processLink->start();
    getlinksig=new headerinit();
    connect(getstatustime,SIGNAL(timeout()),this,SLOT(processlinkbuff()));
    getstatustime->start(UIREFRESHTIME);
    qDebug() << "link head fun::: " << QThread::currentThreadId();
}

getLinkingMachine:: ~getLinkingMachine()
{
    processLink->quit();
    processLink->wait();
}

void getLinkingMachine::linkingmachine(unsigned int linkingnum,QString ipaddr)
{
//    printf("66666666666666=%s\n",ipaddr.toAscii().data());
    qDebug() << "linkingmachine::: " << QThread::currentThreadId();
    for(int i=0;i<NUMOFUSER;i++)
    {
        /*查找是否已经有链接,如果已经链接则设置状态*/
        if(userlinkingbuff[i].addr.toString()==ipaddr)
        {
            clockbuff[i]=clock();
            break;
        }
        /*如果没有链接则添加链接*/
        if(userlinkingbuff[i].addr.toString()==NULL)
        {
            for(int j=0;j<NUMOFUSER;j++)
            {
                if((userconfbuff[i].addr.toString()==NULL)||(userconfbuff[i].num==0x00))
                {
                    break;
                }
//                printf("usernum=%02x useraddr=%s \n",userlinkingbuff[i].num,userlinkingbuff[i].addr.toString().toAscii().data());
                if((ipaddr==userconfbuff[j].addr.toString())
                        &&(linkingnum==userconfbuff[j].num))
                {
                    userlinkingbuff[i].addr.setAddress(ipaddr);
                    userlinkingbuff[i].num=linkingnum;
                    clockbuff[i]=clock();
                    break;
                }
            }
            break;
        }
    }
}
void getLinkingMachine::processlinkbuff()
{
//    QVector<linkuser> linkingbuff(NUMOFUSER);
qDebug() << "get view desktop::: " << QThread::currentThreadId();
    endclock=clock();
    for(int i=0;i<NUMOFUSER;i++)
    {
        if(userlinkingbuff[i].addr.toString()==NULL||userlinkingbuff[i].num==0)
        {
            break;
        }
        if(endclock-clockbuff[i]>1000000)
        {
            printf("有一个用户断开链接\n");
            int j=i;
            if(userlinkingbuff[j].num==ipsingleclock)
            {
                ipsingleclock=0x00;
                controlval=0x00;
            }
            for(;;)
            {
                if(j==NUMOFUSER-1)
                {
                    userlinkingbuff[j].addr.setAddress("");
                    userlinkingbuff[j].num=0;
                    break;
                }
                if(j>(NUMOFUSER-1))
                {
                    break;
                }
                if(userlinkingbuff[j+1].addr.toString()==NULL)
                {
                    userlinkingbuff[j].addr.setAddress("");
                    userlinkingbuff[j].num=0;
                    break;
                }
                userlinkingbuff[j].addr.setAddress(userlinkingbuff[j+1].addr.toString());
                userlinkingbuff[j].num=userlinkingbuff[j+1].num;
                j++;
            }
        }
    }

    for(int i=0;i<NUMOFUSER;i++)
    {

        for(int j=0;j<NUMOFUSER;j++)
        {
            if((userlinkingbuff[i].addr.toString()==userconfbuff[j].addr.toString())
                    &&(userlinkingbuff[i].num==userconfbuff[j].num))
            {
                //                linkingbuff[j]=userlinkingbuff[i];
                linkingbuff[j].num=userlinkingbuff[i].num;
                linkingbuff[j].addr.setAddress(userlinkingbuff[i].addr.toString());
                break;
            }

        }

        if(linkingbuff[i].addr.toString()!=NULL)
        {
            for(int k=0;k<NUMOFUSER;k++)
            {
                if((userlinkingbuff[k].num!=0x00)
                        &&userlinkingbuff[k].addr.toString()!=NULL
                        &&(userlinkingbuff[k].num==linkingbuff[i].num)
                        &&(userlinkingbuff[k].addr.toString()==linkingbuff[i].addr.toString()))
                {
                    break;
                }
                if((userlinkingbuff[k].num==0x00)||
                        (userlinkingbuff[k].addr.toString()==NULL)||
                        (k>NUMOFUSER))
                {
                    linkingbuff[i].num=0x00;
                    linkingbuff[i].addr.setAddress("");
                }
            }
        }
    }

//    for(int i=0;i<NUMOFUSER;i++)
//    {
//        printf("%d++++ num= %d add= %s\n",i,linkingbuff[i].num,linkingbuff[i].addr.toString().toAscii().data());
//    }

    emit allLinksig(linkingbuff);
}
