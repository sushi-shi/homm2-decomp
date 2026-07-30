#ifndef HOMM2_PLATFORM_WIN32_DPLAY_H
#define HOMM2_PLATFORM_WIN32_DPLAY_H

#include <windows.h>

#define DP_OK 0L

#define DPERR_ALREADYINITIALIZED ((HRESULT)0x8878000CL)
#define DPERR_ACCESSDENIED ((HRESULT)0x8878000AL)
#define DPERR_ACTIVEPLAYERS ((HRESULT)0x8878000EL)
#define DPERR_BUFFERTOOSMALL ((HRESULT)0x88780010L)
#define DPERR_BUSY ((HRESULT)0x88780026L)
#define DPERR_CANTADDPLAYER ((HRESULT)0x88780014L)
#define DPERR_CANTCREATEGROUP ((HRESULT)0x88780016L)
#define DPERR_CANTCREATEPLAYER ((HRESULT)0x88780018L)
#define DPERR_CANTCREATESESSION ((HRESULT)0x8878001AL)
#define DPERR_CAPSNOTAVAILABLEYET ((HRESULT)0x8878001CL)
#define DPERR_EXCEPTION ((HRESULT)0x8878001EL)
#define DPERR_GENERIC ((HRESULT)0x80004005L)
#define DPERR_INVALIDFLAGS ((HRESULT)0x88780024L)
#define DPERR_INVALIDOBJECT ((HRESULT)0x88780028L)
#define DPERR_INVALIDPARAMS ((HRESULT)0x80070057L)
#define DPERR_INVALIDPLAYER ((HRESULT)0x8878002EL)
#define DPERR_NOCAPS ((HRESULT)0x88780032L)
#define DPERR_NOCONNECTION ((HRESULT)0x88780034L)
#define DPERR_NOMESSAGES ((HRESULT)0x8878003CL)
#define DPERR_NONAMESERVERFOUND ((HRESULT)0x8878003EL)
#define DPERR_NOPLAYERS ((HRESULT)0x88780040L)
#define DPERR_NOSESSIONS ((HRESULT)0x88780042L)
#define DPERR_OUTOFMEMORY ((HRESULT)0x8007000EL)
#define DPERR_SENDTOOBIG ((HRESULT)0x88780048L)
#define DPERR_TIMEOUT ((HRESULT)0x8878004AL)
#define DPERR_UNAVAILABLE ((HRESULT)0x8878004CL)
#define DPERR_UNSUPPORTED ((HRESULT)0x80004001L)
#define DPERR_USERCANCEL ((HRESULT)0x8878004EL)

#define DPESC_TIMEDOUT 0x00000001

#define DPOPEN_OPENSESSION 0x00000001
#define DPOPEN_CREATESESSION 0x00000002

typedef DWORD DPID;
typedef DPID* LPDPID;

struct DPSESSIONDESC {
    DWORD dwSize;
    GUID guidSession;
    DWORD dwSession;
    DWORD dwMaxPlayers;
    DWORD dwCurrentPlayers;
    DWORD dwFlags;
    char szSessionName[256];
    char szUserField[256];
    DWORD dwReserved1;
    char szPassword[256];
    DWORD dwReserved2;
    DWORD dwUser1;
    DWORD dwUser2;
    DWORD dwUser3;
    DWORD dwUser4;
};
typedef DPSESSIONDESC* LPDPSESSIONDESC;

struct DPNAME {
    DWORD dwSize;
    DWORD dwFlags;
    char* lpszShortName;
    char* lpszLongName;
};
typedef DPNAME* LPDPNAME;

struct IDirectPlay {
    virtual ULONG Release() = 0;
    virtual HRESULT Close() = 0;
    virtual HRESULT Open(LPDPSESSIONDESC session) = 0;
    virtual HRESULT CreatePlayer(LPDPID player, char* shortName, char* longName,
                                 HANDLE event) = 0;
    virtual HRESULT DestroyPlayer(DPID player) = 0;
    virtual HRESULT EnumSessions(LPDPSESSIONDESC session, DWORD timeout,
                                 void* callback, void* context, DWORD flags) = 0;
    virtual HRESULT Send(DPID from, DPID to, DWORD flags, void* message,
                         DWORD size) = 0;
    virtual HRESULT Receive(LPDPID from, LPDPID to, DWORD flags, void* message,
                            LPDWORD size) = 0;
    virtual HRESULT SaveSession(LPDPSESSIONDESC session) = 0;
};

typedef struct IDirectPlay* LPDIRECTPLAY;

typedef HRESULT (*DirectPlayCreateProc)(LPGUID guid, LPDIRECTPLAY* play,
                                        IUnknown* outer);

typedef BOOL (*LPDPENUMDPCALLBACK)(LPGUID guid, char* driverName, DWORD major,
                                   DWORD minor, void* context);
typedef LPDPENUMDPCALLBACK LPDPENUMDPCALLBACKA;

#endif
