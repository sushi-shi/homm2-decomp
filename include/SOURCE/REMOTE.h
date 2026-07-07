#ifndef HOMM2_REMOTE_H
#define HOMM2_REMOTE_H
// Declarations of the free functions DEFINED in REMOTE.cpp — the single home for these
// symbols. Other TUs call them by including this header (no local externs).
#include <_all.h>

void RemoteCleanup(void);
void RemoteMain(int);
void UnloadRemoteDriver(short int);
int calc_crc_long(unsigned char *, int);
void calc_crc(unsigned short int *, unsigned char *, int);
int EncodePacket(unsigned char *, char, char, int);
int DecodePacket(unsigned char *, int);
int SendRemoteData(unsigned char *, unsigned char *, int, int);
int ReceiveRemoteData(unsigned char *, unsigned char *, int);
int TransmitRemoteData(char *, int, int, signed char, signed char, signed char, signed char);
char * GetRemoteData(signed char);
void PollRemote(void);
int TransmitAndWait(char *, int, int, signed char, signed char, char * *);

#endif // HOMM2_REMOTE_H
