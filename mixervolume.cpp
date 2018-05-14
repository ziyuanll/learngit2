#include "mixervolume.h"

mixervolume::mixervolume()
{
    long ll,lr;
    QString mix_headphone_ctrl="Headphone";
    alsa_mix_headphone_handle=NULL;
    alsa_mix_headphone_elem=NULL;

    alsa_mix_headphone_ctrl=mix_headphone_ctrl.toLatin1().data();
    volume_init(alsa_mix_headphone_ctrl);
    //获取音量值
    snd_mixer_selem_get_playback_volume(alsa_mix_headphone_elem,SND_MIXER_SCHN_FRONT_LEFT,&ll);
    snd_mixer_selem_get_playback_volume(alsa_mix_headphone_elem,SND_MIXER_SCHN_FRONT_RIGHT,&lr);

    volumeval=ll;
}
mixervolume::~mixervolume()
{
    free(alsa_mix_sid);
    volume_uninit(alsa_mix_headphone_handle);
}
void mixervolume::volume_init(char *alsa_mix_ctrl)
{
    int rt=0;
    int alsa_mix_index = 0;
    alsa_mix_sid = NULL;

    snd_mixer_selem_id_alloca(&alsa_mix_sid);
    snd_mixer_selem_id_set_index(alsa_mix_sid, alsa_mix_index);
    snd_mixer_selem_id_set_name(alsa_mix_sid, alsa_mix_ctrl);
    if ((rt=snd_mixer_open(&alsa_mix_headphone_handle, 0)) < 0)
       { DEBUG ("Failed to open mixer");}

    if ((rt=snd_mixer_attach(alsa_mix_headphone_handle, "default")) < 0)
       { DEBUG ("Failed to attach mixer");}
    if ((rt=snd_mixer_selem_register(alsa_mix_headphone_handle, NULL, NULL)) < 0)
    {DEBUG ("Failed to register mixer element");}
    if ((rt=snd_mixer_load(alsa_mix_headphone_handle)) < 0)
    {DEBUG ("Failed to load mixer element");}

    alsa_mix_headphone_elem= snd_mixer_first_elem(alsa_mix_headphone_handle);
    if ((rt=snd_mixer_selem_set_playback_volume_range (alsa_mix_headphone_elem, 0, 100)) < 0)
    {DEBUG("Failed to set playback volume range");}
}
void mixervolume::volume_uninit(snd_mixer_t *alsa_mixer_handle)
{
    if(alsa_mixer_handle)
    {
        snd_mixer_close(alsa_mixer_handle);
    }
}

void mixervolume::volume_change(long ll,long lr)
{
    //左音量
    snd_mixer_selem_set_playback_volume(alsa_mix_headphone_elem, SND_MIXER_SCHN_FRONT_LEFT, ll);
    //右音量
    snd_mixer_selem_set_playback_volume(alsa_mix_headphone_elem, SND_MIXER_SCHN_FRONT_RIGHT, lr);

    snd_mixer_selem_get_playback_volume(alsa_mix_headphone_elem,SND_MIXER_SCHN_FRONT_LEFT,&ll);
    snd_mixer_selem_get_playback_volume(alsa_mix_headphone_elem,SND_MIXER_SCHN_FRONT_RIGHT,&lr);
    volumeval=ll;
    MSpeak.volume=(uint8_t)volumeval;
}

void mixervolume::volumeset(long ll, long lr)
{
    QSettings *configIniRead;
    QVariant vol;
    QString volstr;
    configIniRead = new QSettings("confserver.ini", QSettings::IniFormat);
    volume_change(ll,lr);
    volstr=QString::number(volumeval,10);
    vol=QVariant(volstr);
    configIniRead->setValue("APPSet/Volume",vol);
    delete configIniRead;
}
