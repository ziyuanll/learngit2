#include "header.h"

volatile uint32_t controlval;       /*当前的server工作状态,0x11通话,0x00空闲*/
uint32_t ipsingleclock=0x00;        /*正在通话的ip编号*/
uint8_t btncontrl=0x00;             /*确保关闭按钮一定能关闭通话*/
AlsaCaptureAndPlay play;
AlsaCaptureAndPlay cap;
pthread_t capturethread;
bool denoiseswitch=false;
//pthread_t playthread;

const char *device = "plughw:0,0";                  /* playback device */
const snd_pcm_format_t format = SND_PCM_FORMAT_S16_LE;    /* sample format */
const unsigned int rate = 11025;                          /* rate */
const unsigned int channels = 1;                          /* count of channels */
const int resample = 1;                                   /* enable alsa-lib resampling */

unsigned int buffer_time = 500000;
unsigned int period_time = 100000;
snd_pcm_sframes_t buffer_size;
snd_pcm_sframes_t period_size;

SpeexPreprocessState *st;
char capthreadendswitch=0x11;
void initalsaspeex();


headerinit::headerinit(QObject *parent):QObject(parent)
{
    playthread=new QThread();
    this->moveToThread(playthread);
    playthread->start();
}

headerinit::~headerinit()
{
    capthreadendswitch=0x00;
    playthread->quit();
    playthread->wait();
}

void* caphwdata(void*)
{
    int i=0,rsultcode;
    int length=0;
    int n=0;
    snd_pcm_uframes_t frames;
    char *pointt;

    std::cout<<"in caphwdata "<<std::endl;
    cap.datapack.sendheat.datanum=0x00;
    cap.datapack.sendheat.status=0x01;
    frames= BLOCK_SIZE/2;
    cap.datapack.sendheat.ipclock=0x226;
    initalsaspeex();

    while(1)
    {
        usleep(1);
        if(cap.run==true)
        {
            if(capthreadendswitch==0x00)//让线程退出
            {
                break;
            }
            i++;
            rsultcode = snd_pcm_readi(cap.handle, cap.capdata.getchar, frames); //读取64
            if (rsultcode == -EPIPE) {
                /* EPIPE means overrun */
                fprintf(stderr, "overrun occurred\n");
                snd_pcm_prepare(cap.handle);
            } else if (rsultcode < 0) {
                fprintf(stderr,"error from read: %s\n", snd_strerror(rsultcode));
            } else if (rsultcode != (int)frames) {
                fprintf(stderr, "short read, read %d %d frames\n", rsultcode,(int)frames);
            }

            /*消除噪音*/
            if(denoiseswitch)
            {
                try
                {
                    if((unsigned int)rsultcode==frames)
                    {
//                        printf("+++++++1\n");
                        speex_preprocess_run(st,cap.capdata.samples_short);
                    }
                }
                catch(...)
                {
                    printf("denoise error !\n");
                }
//                printf("+++++++2\n");
            }
            /*压缩语音数据*/
            cap.getdata.encode(cap.capdata,cap.datapack.databuff);
            cap.datapack.sendheat.datanum++;
            n=sizeof(cap.datapack);
            //        cap.datapack.datalong=strlen((char*)cap.datapack.databuff);
            if((length=cap.udpsocket->writeDatagram((char *)&cap.datapack,n,cap.send_address,cap.port))!=n)
            {
                printf("imformation send error !\n");
                printf("length=%d n=%d !\n",length,n);
            }
            memset(cap.capdata.getchar,0,BLOCK_SIZE);
            if(i%5000==0)
            {
                pointt=(char*)malloc(30);
                printf("capture data successful %d %p\n",i,pointt);
                free(pointt);
                pointt=NULL;
            }
        }
    }
    controlval=0x00;
    fprintf(stderr,  "......capture stop!......\n");
    pthread_exit(&capturethread);
}

//thread play
//void* dataReceived(void*)//接收数据
void headerinit::dataReceived()
{
    int n=0,k=0;
//    int n_heart=sizeof(udphead);
    int n_data=sizeof(udppack);
//    clock_t  clockopen=0,clockoff=0;
//    clock_t  heartfopone=0,heartfoptwo=0;
    snd_pcm_uframes_t playnum=BLOCK_SIZE/2;
    QHostAddress addressip;
    uint port;
    sleep(10);//等待开机启动时的网口初始化

    while(k<74)
    {
        k++;

        n=play.udpsocket->pendingDatagramSize();

        play.udpsocket->readDatagram((char*)&play.datapack,n,play.recv_address,(quint16*)&port);
    }
    printf("open the thread of paly audio \n");

    while(1)
    {
//        clockoff=clock();
//        if(clockoff-clockopen>2000000)
//        {
////            cap.run=false;
////            play.run=false;
//            clockopen=clock();
////            ipsingleclock=0x00;
////            controlval=0x00;
//            printf("超时断连\n");
//        }
        while(play.udpsocket->hasPendingDatagrams())
        {
            /*read the audio data and play. */
            n=play.udpsocket->pendingDatagramSize();
//            if(n==n_heart)
//            {
//                play.udpsocket->readDatagram((char*)&play.recheat,n,&addressip,(quint16*)&port);
//                //if(k==n_heart)//正在接听
////                printf("%s ",addressip.toString().toAscii().data());
//                emit getIPLockNum((unsigned int)play.recheat.ipclock,addressip.toString());
//                if((ipsingleclock==0x00)||(ipsingleclock==play.recheat.ipclock))
//                {
////                    if(ipsingleclock==play.recheat.ipclock)
////                    {
////                        clockopen=clock();
////                    }
//                    if(play.recheat.contrlaudioandplay==0x01)//打开通话
//                    {
//                        controlval=play.recheat.status;/*//0x11通话,0x00空闲*/
//                        ipsingleclock=play.recheat.ipclock;
//                    }
//                    if(play.recheat.contrlaudioandplay==0x10)//关闭通话
//                    {
//                        controlval=play.recheat.status;/*//0x11通话,0x00空闲*/
////                        ipsingleclock=0x00;       //不注释会出现client无法关闭自己
//                    }
//                }

//                play.recheat.status=controlval;
//                play.recheat.ipclock=ipsingleclock;

//                k=cap.udpsocket->writeDatagram((char *)&play.recheat,n,addressip,port);
//                if((k!=n))
//                {
//                    printf("imformation send error,k=%d n=%d !\n",k,n);
//                }
//                if(ipsingleclock==play.recheat.ipclock)
//                {
//                    if((play.recheat.contrlaudioandplay==0x10)||(btncontrl==0x11))//关闭通话
//                    {
//                        ipsingleclock=0x00;
//                        btncontrl=0x00;
//                        MSpeak.usernum=(uint16_t)ipsingleclock;
//                    }
//                }
//                continue;
//            }

            if(n==n_data)
            {
                k=play.udpsocket->readDatagram((char*)&play.datapack,n,&addressip,(quint16*)&port);
                if(play.run)
                {
                    play.getdata.decode(play.datapack.databuff,play.playdata.samples_short);
                    if ((play.rc=snd_pcm_writei(play.handle, play.playdata.putchar,playnum))== -EPIPE)
                    {
                        // EPIPE means underrun
                        fprintf(stderr, "underrun occurred %d\n",play.rc);
                        snd_pcm_prepare(play.handle);
                        /*缓冲区的等待计时一个数据祯的写入时间大约是5.9毫秒，
                    这里等待大约6个数据祯以便供readDatagram函数的读取，
                    否则会出现数据接受不完整，从而出现underrun；*/
                        usleep(30000);
                    } else if (play.rc < 0) {
                        fprintf(stderr,  "error from writei: %s\n",snd_strerror(play.rc));
                    } else if (play.rc != (int)playnum) {
                        fprintf(stderr,  "short write, write %d frames\n", play.rc);
                    } else if(play.j%5000==0){
                        printf("fread data is success %d\n",play.j);
                    }
                }
                play.j++;
            }
//            else
//            {
//                play.udpsocket->readDatagram((char*)&play.datapack,n,&addressip,(quint16*)&port);
//            }
        }
    }
    controlval=0x00;
    fprintf(stderr,  "......play stop!......\n");
//    pthread_exit(&playthread);
}


//init capture hardware parameter
void headerinit::initcap()
{
    int err;

    snd_pcm_hw_params_alloca(&cap.params);

    if ((err = snd_pcm_open(&cap.handle, device, SND_PCM_STREAM_CAPTURE, 0)) < 0)
    {
        printf("Playback open error: %s\n", snd_strerror(err));
    }

    if ((err = set_hwparams(cap.handle, cap.params, SND_PCM_ACCESS_RW_INTERLEAVED)) < 0)
    {
        printf("Setting of hwparams failed: %s\n", snd_strerror(err));
        exit(EXIT_FAILURE);
    }
}

//init play hardware parameter
void headerinit::initplay()
{
    int err;

    snd_pcm_hw_params_alloca(&play.params);

    if ((err = snd_pcm_open(&play.handle, device, SND_PCM_STREAM_PLAYBACK, 0)) < 0)
    {
        printf("Playback open error: %s\n", snd_strerror(err));
        //return 0;
    }

    if ((err = set_hwparams(play.handle, play.params, SND_PCM_ACCESS_RW_INTERLEAVED)) < 0)
    {
        printf("Setting of hwparams failed: %s\n", snd_strerror(err));
        exit(EXIT_FAILURE);
    }
}

void headerinit::end()
{
    snd_pcm_drain(cap.handle);
    snd_pcm_close(cap.handle);

    snd_pcm_drain(play.handle);
    snd_pcm_close(play.handle);
}

int headerinit::set_hwparams(snd_pcm_t *handles,
                 snd_pcm_hw_params_t *params,
                 snd_pcm_access_t access)
{
    unsigned int rrate;
    snd_pcm_uframes_t size;
    int err, dir;

    /* choose all parameters */
    err = snd_pcm_hw_params_any(handles, params);

    if (err < 0) {
        printf("Broken configuration for playback: no configurations available: %s\n", snd_strerror(err));
        return err;
    }

    /* set hardware resampling */
    err = snd_pcm_hw_params_set_rate_resample(handles, params, resample);
    if (err < 0) {
        printf("Resampling setup failed for playback: %s\n", snd_strerror(err));
        return err;
    }
    /* set the interleaved read/write format */
    err = snd_pcm_hw_params_set_access(handles, params, access);
    if (err < 0) {
        printf("Access type not available for playback: %s\n", snd_strerror(err));
        return err;
    }
    /* set the sample format */
    err = snd_pcm_hw_params_set_format(handles, params, format);
    if (err < 0) {
        printf("Sample format not available for playback: %s\n", snd_strerror(err));
        return err;
    }
    /* set the count of channels */
    err = snd_pcm_hw_params_set_channels(handles, params, channels);
    if (err < 0) {
        printf("Channels count (%i) not available for playbacks: %s\n", channels, snd_strerror(err));
        return err;
    }
    /* set the stream rate */
    rrate = rate;
    err = snd_pcm_hw_params_set_rate_near(handles, params, &rrate, 0);
    if (err < 0) {
        printf("Rate %iHz not available for playback: %s\n", rate, snd_strerror(err));
        return err;
    }
    if (rrate != rate) {
        printf("Rate doesn't match (requested %iHz, get %iHz)\n", rate, err);
        return -EINVAL;
    }
    /* set the buffer time */
    err = snd_pcm_hw_params_set_buffer_time_near(handles, params, &buffer_time, &dir);
    if (err < 0) {
        printf("Unable to set buffer time %i for playback: %s\n", buffer_time, snd_strerror(err));
        return err;
    }
    err = snd_pcm_hw_params_get_buffer_size(params, &size);
    if (err < 0) {
        printf("Unable to get buffer size for playback: %s\n", snd_strerror(err));
        return err;
    }
    buffer_size = size;
            fprintf(stderr,  "-----------------buffer_size= %d\n", (int)buffer_size);
    /* set the period time */
    err = snd_pcm_hw_params_set_period_time_near(handles, params, &period_time, &dir);
    if (err < 0) {
        printf("Unable to set period time %i for playback: %s\n", period_time, snd_strerror(err));
        return err;
    }
    err = snd_pcm_hw_params_get_period_size(params, &size, &dir);
    if (err < 0) {
        printf("Unable to get period size for playback: %s\n", snd_strerror(err));
        return err;
    }
    period_size = size;
    fprintf(stderr,  "-----------------periodsize= %d\n", (int)period_size);
    /* write the parameters to device */
    err = snd_pcm_hw_params(handles, params);
    if (err < 0) {
        printf("Unable to set hw params for playback: %s\n", snd_strerror(err));
        return err;
    }
    return 0;
}

/*speex 初始化*/
void initalsaspeex()
{
    int denoise=1;
    int noisesuppress=-100;

    st=speex_preprocess_state_init(BLOCK_SIZE/2,rate);
    speex_preprocess_ctl(st,SPEEX_PREPROCESS_SET_DENOISE,&denoise);
    speex_preprocess_ctl(st,SPEEX_PREPROCESS_SET_NOISE_SUPPRESS,&noisesuppress);
    printf("speex init end !\n");
}
