#ifndef CMODBUSTCPSLAVE_H
#define CMODBUSTCPSLAVE_H

#include <iostream>
#include <QThread>
#include <modbus/modbus.h>
#include <modbus/modbus-tcp.h>
#include <modbus/modbus-version.h>
#include "modbuscomply.h"
#include "alsainit.h"

#define READ_HOLDING_REGISTER 0x03
#define PRESET_SINGLE_REGISTER 0x06
#define READ_MULTI_REGISTER 0x10
#define MACHINE_ADDRESS 0X01

class CmodbusTcpSlave:public QThread
{
    Q_OBJECT
public:
    CmodbusTcpSlave();

    ~CmodbusTcpSlave();

    bool Enabled;

    void ModbusInit();

protected:
    void run();

private:
    int socket;
    modbus_t *ctx;
    modbus_mapping_t *mb_mapping;
    int rc;
    uint8_t *query;
    const uint8_t *mapingadd;
    int header_length;
    unsigned int iHRAddress;
    int iCmdValue;
    unsigned int iRegSize;
    FILE *fp;

signals:
    void HRValueOnRead(unsigned int iHAddOffset, unsigned char *iRecAdd, int iSize);

    void HRValueOnChanged(unsigned int iHRAddOffset, int iNewValue);

    void HRMoreValueOnChanged(unsigned int iHAddOffset, unsigned char *iNewValue, int iSize);

    void coutchar();

public slots:
    void SetHRValuebyAdd(int iHRAddOffset, int iValue);
};


#endif // CMODBUSTCPSLAVE_H
