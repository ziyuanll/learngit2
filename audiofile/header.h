#ifndef HEADER_H
#define HEADER_H

#include "endecode.h"
#include "../modbuscomply.h"
#include <QThread>
#include <QDebug>
#include <QObject>
#include <speex/speex_preprocess.h>

extern volatile uint32_t controlval;
extern uint32_t ipsingleclock;
extern uint8_t btncontrl;
extern AlsaCaptureAndPlay play;
extern AlsaCaptureAndPlay cap;
extern pthread_t capturethread;
extern bool denoiseswitch;
void* caphwdata(void*);

//void* dataReceived(void*);

class headerinit:public QObject
{
    Q_OBJECT
public:
    explicit headerinit(QObject *parent=0);
    ~headerinit();
    void initcap();
    void initplay();
    void initalsaspeex();
    void end();


signals:
    void getIPLockNum(unsigned int,QString);
//    void getemit();
public slots:
    void dataReceived();

private:
    int set_hwparams(snd_pcm_t *handles,
                     snd_pcm_hw_params_t *params,
                     snd_pcm_access_t access);
    QThread *playthread;

};

#endif // HEADER_H
