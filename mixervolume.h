#ifndef MAIXERVOLUME_H
#define MAIXERVOLUME_H
#include <iostream>
#include <string>
#include <QString>
#include <alsa/asoundlib.h>
#include <alsa/asoundef.h>
#include <sys/soundcard.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <alsa/mixer.h>
#include <QVariant>
#include <QSettings>
#include <modbuscomply.h>

class mixervolume
{
public:
    mixervolume();
    ~mixervolume();
    void volume_init(char *alsa_mix_ctrl);
    void volume_uninit(snd_mixer_t *alsa_mixer_handle);
    void volume_change(long ll,long lr);
    void volumeset(long ll,long lr);

    long volumeval;
    char *alsa_mix_dev;
    char *alsa_mix_headphone_ctrl;
    char *alsa_mix_digital_ctrl;
    snd_mixer_elem_t *alsa_mix_headphone_elem;
    snd_mixer_t *alsa_mix_headphone_handle;
    snd_mixer_selem_id_t *alsa_mix_sid;
};

#define DEBUG(x,y...)	//{printf("[ %s : %s : %d] ",__FILE__, __func__, __LINE__); printf(x,##y); printf("\n");}
#define ERROR(x,y...)	{printf("[ %s : %s : %d] ",__FILE__, __func__, __LINE__); printf(x,##y); printf("\n");}


#endif // MAIXERVOLUME_H
