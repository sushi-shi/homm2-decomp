#ifndef MSS_H
#define MSS_H


#include <windows.h>
#include <mmsystem.h>


#ifndef U8
#define U8  unsigned char
#endif
#ifndef S8
#define S8  signed char
#endif
#ifndef U16
#define U16 unsigned short
#endif
#ifndef S16
#define S16 signed short
#endif
#ifndef U32
#define U32 unsigned long
#endif
#ifndef S32
#define S32 signed long
#endif

#ifndef FAR
#define FAR
#endif


#define AILCALL __stdcall
#ifdef __cplusplus
  #define MSS_EXTERN extern "C"
#else
  #define MSS_EXTERN extern
#endif
#define DXDEC MSS_EXTERN __declspec(dllimport)


typedef struct _DIG_DRIVER FAR* HDIGDRIVER;
typedef struct _MDI_DRIVER FAR* HMDIDRIVER;
typedef struct _SAMPLE     FAR* HSAMPLE;
typedef struct _SEQUENCE   FAR* HSEQUENCE;
typedef struct _REDBOOK    FAR* HREDBOOK;

#ifdef __cplusplus
extern "C" {
#endif


DXDEC S32       AILCALL AIL_startup                 (void);
DXDEC void      AILCALL AIL_shutdown                (void);
DXDEC void      AILCALL AIL_serve                   (void);
DXDEC char FAR* AILCALL AIL_last_error              (void);
DXDEC S32       AILCALL AIL_set_preference          (U32 number, S32 value);
DXDEC S32       AILCALL AIL_get_preference          (U32 number);


DXDEC S32       AILCALL AIL_waveOutOpen             (HDIGDRIVER FAR* drvr,
                                                     LPHWAVEOUT FAR* lphWaveOut,
                                                     S32             dwDeviceID,
                                                     LPWAVEFORMAT    lpFormat);


DXDEC HSAMPLE   AILCALL AIL_allocate_sample_handle  (HDIGDRIVER dig);
DXDEC void      AILCALL AIL_init_sample             (HSAMPLE S);
DXDEC void      AILCALL AIL_start_sample            (HSAMPLE S);
DXDEC void      AILCALL AIL_end_sample              (HSAMPLE S);
DXDEC U32       AILCALL AIL_sample_status           (HSAMPLE S);
DXDEC void      AILCALL AIL_set_sample_address      (HSAMPLE S, void FAR* start, U32 len);
DXDEC void      AILCALL AIL_set_sample_type         (HSAMPLE S, S32 format, U32 flags);
DXDEC void      AILCALL AIL_set_sample_playback_rate(HSAMPLE S, S32 playback_rate);
DXDEC void      AILCALL AIL_set_sample_volume       (HSAMPLE S, S32 volume);
DXDEC S32       AILCALL AIL_sample_volume           (HSAMPLE S);
DXDEC void      AILCALL AIL_set_sample_loop_count   (HSAMPLE S, S32 loop_count);


DXDEC S32       AILCALL AIL_midiOutOpen             (HMDIDRIVER FAR* drvr,
                                                     LPHMIDIOUT FAR* lphMidiOut,
                                                     S32             dwDeviceID);
DXDEC void      AILCALL AIL_midiOutClose            (HMDIDRIVER mdi);
DXDEC void      AILCALL AIL_set_XMIDI_master_volume (HMDIDRIVER mdi, S32 master_volume);
DXDEC HSEQUENCE AILCALL AIL_allocate_sequence_handle(HMDIDRIVER mdi);
DXDEC void      AILCALL AIL_release_sequence_handle (HSEQUENCE S);
DXDEC S32       AILCALL AIL_init_sequence           (HSEQUENCE S, void FAR* start, S32 sequence_num);
DXDEC void      AILCALL AIL_start_sequence          (HSEQUENCE S);
DXDEC void      AILCALL AIL_stop_sequence           (HSEQUENCE S);
DXDEC void      AILCALL AIL_resume_sequence         (HSEQUENCE S);
DXDEC U32       AILCALL AIL_sequence_status         (HSEQUENCE S);
DXDEC void      AILCALL AIL_set_sequence_loop_count (HSEQUENCE S, S32 loop_count);


DXDEC HREDBOOK  AILCALL AIL_redbook_open            (U32 which);
DXDEC void      AILCALL AIL_redbook_close           (HREDBOOK hand);
DXDEC U32       AILCALL AIL_redbook_tracks          (HREDBOOK hand);
DXDEC void      AILCALL AIL_redbook_track_info      (HREDBOOK hand, U32 tracknum,
                                                     U32 FAR* startmsec, U32 FAR* endmsec);

#ifdef __cplusplus
}
#endif

#endif
