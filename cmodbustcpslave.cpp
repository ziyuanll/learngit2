#include "cmodbustcpslave.h"
#include <QtDebug>

CmodbusTcpSlave::CmodbusTcpSlave()
{
    ctx=modbus_new_tcp("127.0.0.1",1502);

    recdata=(uint8_t*)malloc(MODBUS_TCP_MAX_ADU_LENGTH);

    query=(uint8_t*)malloc(MODBUS_TCP_MAX_ADU_LENGTH);

    memset(recdata,0,MODBUS_TCP_MAX_ADU_LENGTH);

    header_length = modbus_get_header_length(ctx);

    mb_mapping=modbus_mapping_new(0,0,100,0);

}

CmodbusTcpSlave::~CmodbusTcpSlave()
{
    free(query);

    query=NULL;

    free(recdata);

    modbus_mapping_free(mb_mapping);

    modbus_free(ctx);
}

void CmodbusTcpSlave::run()
{
    memset(query,0,MODBUS_TCP_MAX_ADU_LENGTH);
    socket=modbus_tcp_listen(ctx,2);
    modbus_tcp_accept(ctx,&socket); //等待tcp接入

    while(Enabled)
    {
         //等待接受信息
        rc=modbus_receive(ctx,query);
        if (rc==-1)
        {
            close(socket);
            socket=modbus_tcp_listen(ctx,1);
            modbus_tcp_accept(ctx,&socket);
            continue;
        }
        if(query[header_length-1]!=MACHINE_ADDRESS)
        {
            continue;
        }

        recdata=query;

        emit coutchar();

        switch(query[header_length])
        {
        case READ_HOLDING_REGISTER:
            //read single holding register
            iHRAddress=MODBUS_GET_INT16_FROM_INT8(query,header_length+1);

            iRegSize=MODBUS_GET_INT16_FROM_INT8(query,header_length+3);

            if(iRegSize>sizeof(MSpeak))
            {
                iRegSize=sizeof(MSpeak);
            }

            for(int i=0;i<(int)iRegSize;i++)
            {

                mb_mapping->tab_registers[i]=*(RegAdd+iHRAddress+i);
            }

            rc=modbus_reply(ctx,query,rc,mb_mapping); /*接收到信息后的返回信息*/
            if (rc==-1)
            {
                std::cout<<"modbus reply 0x03 is error, The return value is -1"<<std::endl;
            }
            break;

        case PRESET_SINGLE_REGISTER:/*write single holding register command*/
            /*四位的起始地址*/
            iHRAddress=(query[header_length+1])<<8 | query[header_length+2];
            if(iHRAddress>sizeof(MSpeak))
            {
               break;
            }

            iRegSize=MODBUS_GET_INT16_FROM_INT8(query,header_length+3); /*四位的数据长度*/

            emit HRValueOnChanged(iHRAddress,iRegSize);

            rc=modbus_reply(ctx,query,rc,mb_mapping);   /*接收到信息后的返回信息*/
            if (rc==-1)
            {
                std::cout<<"modbus reply 0x06 is error, The return value is "<<rc<<std::endl;
            }

            qDebug()<<"Add: "<<iHRAddress+1<<" Value: "<<iCmdValue;
            break;

        case READ_MULTI_REGISTER:/*write multi holding register command*/
            iHRAddress=(query[header_length+1])<<8 | query[header_length+2];
            iRegSize=query[header_length+5];
            if((iRegSize/2>(sizeof(MSpeak)-iHRAddress))&&(sizeof(MSpeak)<iHRAddress))
            {
                printf("buff is too long \n");
                break;
            }

            emit HRMoreValueOnChanged(iHRAddress,(recdata+header_length+6),iRegSize);

            qDebug()<<"com:16"<<iHRAddress;

            rc=modbus_reply(ctx,query,11,mb_mapping);
            if (rc==-1)
            {
                std::cout<<"modbus reply 0x10 is error, The return value is "<<rc<<std::endl;
            }
            break;

        default:
            printf("the error func NUM is  %02x \n",query[header_length]);
            break;
        }
        memset(query,0,MODBUS_TCP_MAX_ADU_LENGTH);
    }
    close(socket);
    printf("free over !\n");
}

void CmodbusTcpSlave::SetHRValuebyAdd(int iAdd, int iValue)
{
    if (QString("%1").arg((iAdd)).length()==5)
    {
        iAdd-=40001;
    }
    if (QString("%1").arg(iAdd).length()==6)
    {
        iAdd-=400001;
    }

    mb_mapping->tab_registers[0]=iValue;

    for(int i=1;i<(int)sizeof(RegAdd);i++)
    {
        mb_mapping->tab_registers[i]=*(RegAdd+i);
        printf("%04x ",query[i]);
    }

    std::cout<<std::endl;
}

//初始化modbus起始地址
void CmodbusTcpSlave::ModbusInit()
{
    int rc;
    ControlVar msn;

    if((fp=fopen("./set.txt","wt+"))<0)
    {
        printf("open the file is error!\n");
    }
    else
    {
         printf("结构体写入\n");
    }

    //machSwitch;
    RegAdd[0]=0x00;
    //micSwitch;默认关闭
    RegAdd[1]=0x00;
    //loudSwitch;默认关闭
    RegAdd[2]=0x00;
    //UDPsendSwitch;
    RegAdd[3]=0x00;
    //UDPgetSwitch;
    RegAdd[4]=0x46;
    //IPaddress;
    RegAdd[5]=0x00;
    RegAdd[6]=0x00;

    rc=fwrite(RegAdd,sizeof(ControlVar),1,fp);
    if(rc<0)
    {
        printf("写入文件错误!\n");
    }
    fclose(fp);

    if((fp=fopen("./set.txt","rt+"))<0)
    {
        printf("open the file is error!\n");
    }

    rc=fread(&msn,sizeof(msn),1,fp);
    if(rc<0)
    {
        printf("写入文件错误!\n");
    }

    fclose(fp);

    printf("msn结构体输出\n");
    printf("%04x ",msn.usernum);
    printf("%02x ",msn.machSwitch);
    printf("%02x ",msn.testfivesecond);
    printf("%02x ",msn.volume);
    printf("%02x ",msn.delniseSwitch);
    printf("%02x ",msn.space);
    printf("\n");

    printf("msn.usernum= %d\n",(int)msn.usernum);

}



