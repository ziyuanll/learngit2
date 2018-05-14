#ifndef MUDBUSCOMPLY_H
#define MUDBUSCOMPLY_H

#include <iostream>
#include <stdint.h>
#include <QThread>
#include <modbus/modbus.h>
#include <modbus/modbus-tcp.h>
#include <modbus/modbus-version.h>

#define BYTE unsigned char

typedef struct
{
    uint16_t usernum;     /*0001 用户编号*/
    uint8_t machSwitch;     /*02 语音开关*/
    uint8_t testfivesecond; /*03 五秒测试*/
    uint8_t volume;         /*04 音量设置0~100(0x64)*/
    uint8_t delniseSwitch;  /*05 噪音消除控制*/
    uint8_t space;          /*06 空位防止地址越界赋值*/

}ControlVar;

class RtuFuncVariable
{
private:
    RtuFuncVariable();

public:
    FILE *fp;
};

extern uint8_t *recdata;
extern ControlVar MSpeak;
extern unsigned char *RegAdd;
#endif // MUDBUSCOMPLY_H
