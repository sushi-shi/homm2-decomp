#ifndef HOMM2_PLATFORM_WIN32_MSS_H
#define HOMM2_PLATFORM_WIN32_MSS_H

typedef int S32;
typedef unsigned int U32;
typedef short S16;
typedef unsigned short U16;
typedef signed char S8;
typedef unsigned char U8;

typedef struct _DIG_DRIVER* HDIGDRIVER;
typedef struct _MDI_DRIVER* HMDIDRIVER;
typedef struct _SAMPLE* HSAMPLE;
typedef struct _SEQUENCE* HSEQUENCE;

#define SMP_FREE 1
#define SMP_DONE 2
#define SMP_PLAYING 4
#define SMP_STOPPED 8
#define SMP_PLAYINGBUTRELEASED 16

#define SEQ_FREE 1
#define SEQ_DONE 2
#define SEQ_PLAYING 4
#define SEQ_STOPPED 8
#define SEQ_PLAYINGBUTRELEASED 16

#define DIG_F_MONO_8 0
#define DIG_F_MONO_16 1
#define DIG_F_STEREO_8 2
#define DIG_F_STEREO_16 3

#define AIL_MAX_SAMPLE_VOLUME 127

#define MIDI_MAPPER ((U32)-1)

S32 AIL_waveOutOpen(HDIGDRIVER* driver, void* wave, S32 deviceId, const void* format);
void AIL_waveOutClose(HDIGDRIVER driver);
S32 AIL_midiOutOpen(HMDIDRIVER* driver, void* midi, S32 deviceId);
void AIL_midiOutClose(HMDIDRIVER driver);

S32 AIL_startup();
void AIL_shutdown();
void AIL_serve();
char* AIL_last_error();
S32 AIL_set_preference(U32 number, S32 value);
S32 AIL_get_preference(U32 number);

HSAMPLE AIL_allocate_sample_handle(HDIGDRIVER dig);
void AIL_release_sample_handle(HSAMPLE sample);
void AIL_init_sample(HSAMPLE sample);
void AIL_start_sample(HSAMPLE sample);
void AIL_end_sample(HSAMPLE sample);
U32 AIL_sample_status(HSAMPLE sample);
S32 AIL_sample_volume(HSAMPLE sample);
void AIL_set_sample_address(HSAMPLE sample, void* start, U32 length);
void AIL_set_sample_volume(HSAMPLE sample, S32 volume);
void AIL_set_sample_loop_count(HSAMPLE sample, S32 loopCount);
void AIL_set_sample_playback_rate(HSAMPLE sample, S32 rate);
void AIL_set_sample_type(HSAMPLE sample, S32 format, U32 flags);

HSEQUENCE AIL_allocate_sequence_handle(HMDIDRIVER mdi);
void AIL_release_sequence_handle(HSEQUENCE sequence);
S32 AIL_init_sequence(HSEQUENCE sequence, void* start, S32 sequenceNumber);
void AIL_start_sequence(HSEQUENCE sequence);
void AIL_stop_sequence(HSEQUENCE sequence);
void AIL_resume_sequence(HSEQUENCE sequence);
U32 AIL_sequence_status(HSEQUENCE sequence);
void AIL_set_sequence_loop_count(HSEQUENCE sequence, S32 loopCount);
void AIL_set_XMIDI_master_volume(HMDIDRIVER driver, S32 volume);
S32 AIL_XMIDI_master_volume(HMDIDRIVER driver);

#endif
