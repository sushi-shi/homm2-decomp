#ifndef HOMM2_PLATFORM_WIN32_DDRAW_H
#define HOMM2_PLATFORM_WIN32_DDRAW_H

#include <windows.h>

#define DD_OK 0L

#define DDERR_GENERIC ((HRESULT)0x80004005L)
#define DDERR_INVALIDOBJECT ((HRESULT)0x88760001L)
#define DDERR_INVALIDPARAMS ((HRESULT)0x80070057L)
#define DDERR_INVALIDRECT ((HRESULT)0x8876016CL)
#define DDERR_OUTOFMEMORY ((HRESULT)0x8007000EL)
#define DDERR_UNSUPPORTED ((HRESULT)0x80004001L)
#define DDERR_SURFACEBUSY ((HRESULT)0x887601B5L)
#define DDERR_SURFACELOST ((HRESULT)0x887601C2L)
#define DDERR_WRONGMODE ((HRESULT)0x887601CEL)
#define DDERR_NOEXCLUSIVEMODE ((HRESULT)0x887601BFL)
#define DDERR_NOPALETTEATTACHED ((HRESULT)0x887601BBL)
#define DDERR_NOPALETTEHW ((HRESULT)0x887601BCL)
#define DDERR_NOCLIPLIST ((HRESULT)0x887601A3L)
#define DDERR_NOCLIPPERATTACHED ((HRESULT)0x887601A2L)
#define DDERR_INVALIDCLIPLIST ((HRESULT)0x88760169L)
#define DDERR_CLIPPERISUSINGHWND ((HRESULT)0x88760138L)
#define DDERR_IMPLICITLYCREATED ((HRESULT)0x88760168L)
#define DDERR_INCOMPATIBLEPRIMARY ((HRESULT)0x88760167L)
#define DDERR_LOCKEDSURFACES ((HRESULT)0x88760196L)
#define DDERR_NOALPHAHW ((HRESULT)0x8876019CL)
#define DDERR_NOBLTHW ((HRESULT)0x8876019EL)
#define DDERR_NODDROPSHW ((HRESULT)0x887601A8L)
#define DDERR_NOMIRRORHW ((HRESULT)0x887601AAL)
#define DDERR_NORASTEROPHW ((HRESULT)0x887601C0L)
#define DDERR_NOROTATIONHW ((HRESULT)0x887601C1L)
#define DDERR_NOSTRETCHHW ((HRESULT)0x887601C4L)
#define DDERR_NOZBUFFERHW ((HRESULT)0x887601C6L)
#define DDERR_NOT8BITCOLOR ((HRESULT)0x887601AFL)
#define DDERR_NOTFOUND ((HRESULT)0x887601C8L)
#define DDERR_PRIMARYSURFACEALREADYEXISTS ((HRESULT)0x887601CBL)

#define DDSD_CAPS 0x00000001L
#define DDSD_HEIGHT 0x00000002L
#define DDSD_WIDTH 0x00000004L
#define DDSD_PITCH 0x00000008L

#define DDSCAPS_PRIMARYSURFACE 0x00000200L
#define DDSCAPS_OFFSCREENPLAIN 0x00000040L
#define DDSCAPS_SYSTEMMEMORY 0x00000800L
#define DDSCAPS_VIDEOMEMORY 0x00004000L

#define DDSCL_FULLSCREEN 0x00000001L
#define DDSCL_ALLOWREBOOT 0x00000002L
#define DDSCL_NORMAL 0x00000008L
#define DDSCL_EXCLUSIVE 0x00000010L

#define DDPCAPS_8BIT 0x00000004L
#define DDBLT_WAIT 0x01000000L
#define DDLOCK_WAIT 0x00000001L

struct DDSCAPS {
    DWORD dwCaps;
};

struct DDPIXELFORMAT {
    DWORD dwSize;
    DWORD dwFlags;
    DWORD dwFourCC;
    DWORD dwRGBBitCount;
    DWORD dwRBitMask;
    DWORD dwGBitMask;
    DWORD dwBBitMask;
    DWORD dwRGBAlphaBitMask;
};

struct DDSURFACEDESC {
    DWORD dwSize;
    DWORD dwFlags;
    DWORD dwHeight;
    DWORD dwWidth;
    LONG lPitch;
    DWORD dwBackBufferCount;
    DWORD dwMipMapCount;
    DWORD dwAlphaBitDepth;
    DWORD dwReserved;
    LPVOID lpSurface;
    DDPIXELFORMAT ddpfPixelFormat;
    DDSCAPS ddsCaps;
};
typedef DDSURFACEDESC* LPDDSURFACEDESC;

struct DDBLTFX {
    DWORD dwSize;
    DWORD dwDDFX;
    DWORD dwFillColor;
};
typedef DDBLTFX* LPDDBLTFX;

struct IUnknown;
struct IDirectDrawSurface;
struct IDirectDrawPalette;
struct IDirectDrawClipper;

struct IDirectDrawClipper {
    virtual ULONG Release() = 0;
    virtual HRESULT SetHWnd(DWORD flags, HWND window) = 0;
};

struct IDirectDrawPalette {
    virtual ULONG Release() = 0;
    virtual HRESULT SetEntries(DWORD flags, DWORD start, DWORD count,
                               PALETTEENTRY* entries) = 0;
};

struct IDirectDrawSurface {
    virtual ULONG Release() = 0;
    virtual HRESULT Blt(RECT* destination, IDirectDrawSurface* source,
                        RECT* sourceRect, DWORD flags, LPDDBLTFX effects) = 0;
    virtual HRESULT Lock(RECT* rect, LPDDSURFACEDESC description, DWORD flags,
                         HANDLE event) = 0;
    virtual HRESULT Unlock(LPVOID surface) = 0;
    virtual HRESULT Restore() = 0;
    virtual HRESULT SetClipper(IDirectDrawClipper* clipper) = 0;
    virtual HRESULT SetPalette(IDirectDrawPalette* palette) = 0;
};

struct IDirectDraw {
    virtual ULONG Release() = 0;
    virtual HRESULT CreateClipper(DWORD flags, IDirectDrawClipper** clipper,
                                  IUnknown* outer) = 0;
    virtual HRESULT CreatePalette(DWORD flags, PALETTEENTRY* entries,
                                  IDirectDrawPalette** palette,
                                  IUnknown* outer) = 0;
    virtual HRESULT CreateSurface(LPDDSURFACEDESC description,
                                  IDirectDrawSurface** surface,
                                  IUnknown* outer) = 0;
    virtual HRESULT SetCooperativeLevel(HWND window, DWORD flags) = 0;
    virtual HRESULT SetDisplayMode(DWORD width, DWORD height, DWORD bpp) = 0;
    virtual HRESULT RestoreDisplayMode() = 0;
};

struct _GUID;

#endif
