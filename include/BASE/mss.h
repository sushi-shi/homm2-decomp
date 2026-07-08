#ifndef HOMM2_BASE_MSS_H
#define HOMM2_BASE_MSS_H
// Miles Sound System (AIL) — external library imports used by soundmgr.cpp.
// These are NOT defined in our code (they come from the Miles .lib); declared
// here like the Win32 imports so the sound-manager TU can link. Extend as more
// AIL_* calls are recovered from the soundManager methods.
extern "C" void _AIL_serve_0(void);
extern "C" void _AIL_shutdown_0(void);
extern "C" struct _SAMPLE *_AIL_allocate_sample_handle_4(int);
extern "C" int _AIL_sample_status_4(struct _SAMPLE *);
extern "C" void _AIL_end_sample_4(struct _SAMPLE *);
struct _SEQUENCE;
extern "C" int _AIL_sequence_status_4(struct _SEQUENCE *);
struct _MDI_DRIVER;
extern "C" void _AIL_set_XMIDI_master_volume_8(struct _MDI_DRIVER *, int);
extern "C" void _AIL_stop_sequence_4(struct _SEQUENCE *);
extern "C" void _AIL_release_sequence_handle_4(struct _SEQUENCE *);
extern "C" void _AIL_midiOutClose_4(struct _MDI_DRIVER *);
extern "C" int _AIL_midiOutOpen_12(struct _MDI_DRIVER **, int, unsigned long);
extern "C" struct _SEQUENCE *_AIL_allocate_sequence_handle_4(struct _MDI_DRIVER *);
extern "C" int _AIL_init_sequence_12(struct _SEQUENCE *, void *, int);
extern "C" void _AIL_start_sequence_4(struct _SEQUENCE *);
extern "C" void _AIL_set_sequence_loop_count_8(struct _SEQUENCE *, int);
extern "C" void _AIL_resume_sequence_4(struct _SEQUENCE *);
#endif // HOMM2_BASE_MSS_H
