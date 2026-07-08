#ifndef HOMM2_BASE_MSS_H
#define HOMM2_BASE_MSS_H
// Miles Sound System (AIL) — external library imports used by soundmgr.cpp.
// These are NOT defined in our code (they come from the Miles .lib); declared
// here like the Win32 imports so the sound-manager TU can link. Extend as more
// AIL_* calls are recovered from the soundManager methods.
extern "C" __declspec(dllimport) void __stdcall _AIL_startup_0(void);
extern "C" __declspec(dllimport) void __stdcall _AIL_serve_0(void);
struct _DIG_DRIVER;
extern "C" __declspec(dllimport) void __stdcall _AIL_set_preference_8(int, int);
extern "C" __declspec(dllimport) int __stdcall _AIL_waveOutOpen_16(struct _DIG_DRIVER **, int, int, void *);
extern "C" __declspec(dllimport) char *__stdcall _AIL_last_error_0(void);
extern "C" __declspec(dllimport) void __stdcall _AIL_shutdown_0(void);
extern "C" __declspec(dllimport) struct _SAMPLE *__stdcall _AIL_allocate_sample_handle_4(int);
extern "C" __declspec(dllimport) int __stdcall _AIL_sample_status_4(struct _SAMPLE *);
extern "C" __declspec(dllimport) int __stdcall _AIL_sample_volume_4(struct _SAMPLE *);
extern "C" __declspec(dllimport) void __stdcall _AIL_set_sample_volume_8(struct _SAMPLE *, int);
extern "C" __declspec(dllimport) void __stdcall _AIL_start_sample_4(struct _SAMPLE *);
extern "C" __declspec(dllimport) void __stdcall _AIL_init_sample_4(struct _SAMPLE *);
extern "C" __declspec(dllimport) void __stdcall _AIL_set_sample_type_12(struct _SAMPLE *, int, int);
extern "C" __declspec(dllimport) void __stdcall _AIL_set_sample_playback_rate_8(struct _SAMPLE *, int);
extern "C" __declspec(dllimport) void __stdcall _AIL_set_sample_loop_count_8(struct _SAMPLE *, int);
extern "C" __declspec(dllimport) void __stdcall _AIL_set_sample_address_12(struct _SAMPLE *, int, int);
extern "C" __declspec(dllimport) void __stdcall _AIL_end_sample_4(struct _SAMPLE *);
struct _SEQUENCE;
extern "C" __declspec(dllimport) int __stdcall _AIL_sequence_status_4(struct _SEQUENCE *);
struct _MDI_DRIVER;
extern "C" __declspec(dllimport) void __stdcall _AIL_set_XMIDI_master_volume_8(struct _MDI_DRIVER *, int);
extern "C" __declspec(dllimport) void __stdcall _AIL_stop_sequence_4(struct _SEQUENCE *);
extern "C" __declspec(dllimport) void __stdcall _AIL_release_sequence_handle_4(struct _SEQUENCE *);
extern "C" __declspec(dllimport) void __stdcall _AIL_midiOutClose_4(struct _MDI_DRIVER *);
extern "C" __declspec(dllimport) int __stdcall _AIL_midiOutOpen_12(struct _MDI_DRIVER **, int, unsigned long);
extern "C" __declspec(dllimport) struct _SEQUENCE *__stdcall _AIL_allocate_sequence_handle_4(struct _MDI_DRIVER *);
extern "C" __declspec(dllimport) int __stdcall _AIL_init_sequence_12(struct _SEQUENCE *, void *, int);
extern "C" __declspec(dllimport) void __stdcall _AIL_start_sequence_4(struct _SEQUENCE *);
extern "C" __declspec(dllimport) void __stdcall _AIL_set_sequence_loop_count_8(struct _SEQUENCE *, int);
extern "C" __declspec(dllimport) void __stdcall _AIL_resume_sequence_4(struct _SEQUENCE *);
#endif // HOMM2_BASE_MSS_H
