#include "testfivesecond.h"

void testFiveSecond::run()
{
    long i=0;
    long j=0;
    int rc=1;
    int rsultcode,size;
    unsigned char buff[128];
    snd_pcm_uframes_t frames;
    FILE *fp;

    frames=BLOCK_SIZE/2;
    size=frames*2;

    if((fp = fopen("sound.wav","wb+")) < 0)
        fprintf(stderr,"open sound.wav fial\n");
    i=10000000/11025;
    fprintf(stderr, "________start________\n");
    while (i>0) {
        rsultcode = snd_pcm_readi(cap.handle, buff, frames); //读取128
        if (rsultcode == -EPIPE) {
            /* EPIPE means overrun */
            fprintf(stderr, "overrun occurred\n");
            snd_pcm_prepare(cap.handle);
        } else if (rsultcode < 0) {
            fprintf(stderr,"error from read: %s\n", snd_strerror(rsultcode));
        } else if (rsultcode != (int)frames) {
            fprintf(stderr, "short read, read %d %d frames\n", rsultcode,(int)frames);
        }
        rsultcode = fwrite(buff,1, size,fp);
        if (rsultcode != size)
            fprintf(stderr,  "short write: wrote %d bytes\n", rsultcode);
        else if(rsultcode<0)
        {
            fprintf(stderr, "fwrite is error \n");
        }
        else if((int)i%1000==0)
        {
            printf("fwrite buffer success\n%d\n",(int)i);
        }
            i--;
    }
    fclose(fp);
    printf("cap is over\n");
    sleep(1);
    printf("play is start\n");
    if((fp = fopen("sound.wav","rb+")) < 0)
        fprintf(stderr,"open sound.wav fial\n");
    while (rc>0)
    {
        j-- ;
        rc = fread(buff,1, size,fp);
        if (rc == 0) {
            fprintf(stderr, "end of file on input\n");
            break;
        } else if (rc != size) {
            fprintf(stderr,  "short read: read %d bytes\n", rc);
            break;
        } else if(rc<0){
            fprintf(stderr,  "read over: read %d bytes\n", rc);
            break;
        }
        rc = snd_pcm_writei(play.handle,buff, frames);
        if (rc == -EPIPE) {
            // EPIPE means underrun
            fprintf(stderr, "underrun occurred\n");
            snd_pcm_prepare(play.handle);
            rc=1;
        } else if (rc < 0) {
            fprintf(stderr,  "error from writei: %s\n",snd_strerror(rc));
        } else if (rc != (int)frames) {
            fprintf(stderr,  "short write, write %d frames\n", rc);
        }
    }
    fclose(fp);
    QFile::remove("sound.wav");
    exit(0);
    printf("play is over\n");
    emit testover();
}
