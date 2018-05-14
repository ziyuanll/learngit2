#ifndef ENDECODE_H
#define ENDECODE_H

#include <QMainWindow>
#include <QTimer>
#include <QProcess>
#include <QButtonGroup>
#include <iostream>
#include <alsa/asoundlib.h>
#include <stdlib.h>
#include <stdio.h>
#include <QUdpSocket>
#include <QTimer>
#include <QHostAddress>
#include <stdint.h>
#include <QThread>

#define    SIGN_BIT     (0x80)           /* Sign bit for a A-law byte. */
#define    QUANT_MASK   (0xf)          /* Quantization field mask. */
#define    NSEGS        (8)             /* Number of A-law segments. */
#define    SEG_SHIFT    (4)             /* Left shift for segment number. */
#define    SEG_MASK     (0x70)           /* Segment field mask. */
#define    BLOCK_SIZE   160              /*read buffer size*/
#define    NUMOFUSER    20

struct udphead
{
    long long int datanum;
    int heartbeat;
    uint32_t ipclock;//maching num
    uint32_t contrlaudioandplay;//open and off
    uint8_t status; //maching status
};

struct udppack
{
    udphead sendheat;
    int datalong;
    char ipadress[40];
    char name[50];
    char idname[20];
    unsigned char databuff[BLOCK_SIZE/2];
};

class EDcode
{
    /*char to short or short to char*/
public:
    union shortbuffer//read PCM buff to eocde
    {
        unsigned char *point;               //the point is about "readi" data buffer head
        short samples_short[BLOCK_SIZE/2];  //out put short string in file
        unsigned char getchar[BLOCK_SIZE];  //get char string from hardware
    };

    union charbuffer// read compreass data to decode
    {
        unsigned char *point;               //the point is about "writei" data buffer head
        short samples_short[BLOCK_SIZE/2];  //read the file data in buffer
        unsigned char putchar[BLOCK_SIZE];  //write data in hardware
    };

    /*the function is G.711,please don't modify the follow function*/
    /***********************start*************************/
    void encode( shortbuffer inbufchar,unsigned char *compbuf );
    unsigned char linear2alaw( short pcm_val);
    short alaw2linear( unsigned char a_val);
    short swap_linear (short pcm_val);
    void decode(unsigned char inbuf[] , short *outbuf);
    /*************************end************************/
};

class AlsaCaptureAndPlay
{
public:
    struct udphead recheat;
    struct udppack datapack;
    unsigned char readbuff[BLOCK_SIZE/2];   /*read data from files about audio*/
    unsigned char writebuff[BLOCK_SIZE/2];  /*the buffer about data is conpressed,it will write in file*/
    /*bool值默认设置为true*/
    bool run;                           /*it can control the play audio run(true) or stop(false)*/
    //file point
    FILE *fp;                               /*file point for audio file*/
    //hardware variable
    snd_pcm_t *handle;                      /*set hardware*/
    snd_pcm_hw_params_t *params;            /*the point to an array of save the hardware parameter*/
    EDcode getdata;
    EDcode::charbuffer playdata;
    EDcode::shortbuffer capdata;

public:
    QUdpSocket *udpsocket;//套接字变量
    QHostAddress *recv_address;
    QHostAddress send_address;
    snd_pcm_uframes_t frames;
    int port;
    int rc;
    int j;
    int playcharnum;
    uint8_t selfIpAddress;

signals:
    void serverStatus();
};



#endif // ENDECODE_H
