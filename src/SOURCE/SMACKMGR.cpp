// Reconstructed from CodeView NB09 of HEROES2W.EXE - NOT original source.
// compiland: .\Win32_Re\SMACKMGR.OBJ   from: (directly linked into exe)

#include <va.h>
#include <_types.h>
#include <_carcass_types.h>
#include <BASE/Misc.h>
#include <BASE/bitmap.h>
#include <BASE/bmap2.h>
#include <BASE/font.h>
#include <BASE/heroWindowManager.h>
#include <BASE/icon.h>
#include <BASE/inputManager.h>
#include <BASE/mouseManager.h>
#include <BASE/palette.h>
#include <BASE/resourceManager.h>
#include <BASE/soundManager.h>
#include <SOURCE/KB.h>
#include <SOURCE/SMACKMGR.h>
#include <SOURCE/X_GLOBAL.h>
#include <SOURCE/kbwin.h>
#include <SOURCE/wingraph.h>
#include <stdio.h>
#include <string.h>

DATA(0x00522f7c) static signed char bExpansionSmackNum = 0;

VA(0x00401000, 0x4e)
void ConvertSmackerPalette(unsigned char *paletteData) {
    int i;

    for (i = 0; i < SMACK_PALETTE_SIZE; ++i)
        paletteData[i] = static_cast<unsigned char>(
            static_cast<int>(paletteData[i]) >> 2);
}

VA(0x0040104e, 0x21f)
void DoAdvance(Smack *smack, int drawFrame, int advanceFrame, int updatePalette,
               int skipPalette) {
    if (drawFrame && smack->NewPalette && !skipPalette) {
        memcpy(gPalette->m_data, smack->Palette, SMACK_PALETTE_SIZE);
        ConvertSmackerPalette(reinterpret_cast<unsigned char *>(gPalette->m_data));
        if (updatePalette)
            UpdatePalette(gPalette->m_data);
    }

    SmackDoFrame(smack);
    if (drawFrame) {
        while (SmackToBufferRect(smack, SMACKSURFACESLOW)) {
            if (bSmackNum == SMACK_CHOOSE_CAMPAIGN) {
                if (gbCampaignSideChoice == 1) {
                    brotherIcon->DrawToBuffer(0, 0, 0, 0);
                    brotherIcon->DrawToBuffer(0, 0, 3, 0);
                } else {
                    brotherIcon->DrawToBuffer(0, 0, 1, 0);
                    brotherIcon->DrawToBuffer(0, 0, 2, 0);
                }
            }
            if (bSmackNum == SMACK_EXPANSION_CAMPAIGN && xLastChoice != -1)
                backImage->DrawToBuffer(0, 0, 1, 0);
            if (bSmackNum == SMACK_CONGRATS && smack->FrameNum >= 36) {
                smallFont->DrawBoundedString(congratsText, 409, 98, 134, 217, 4, 5);
                if (smack->FrameNum == 36) {
                    BlitBitmapToScreen(gpWindowManager->m_screen, 0, 0, 639, 479, 0, 0);
                    continue;
                }
            }
            BlitBitmapToScreen(gpWindowManager->m_screen, smack->LastRectx,
                               smack->LastRecty, smack->LastRectw, smack->LastRecth,
                               smack->LastRectx, smack->LastRecty);
        }
    }
    if (advanceFrame)
        SmackNextFrame(smack);
}

// @semantic
// Complete 98.95699% coverage checkpoint: retail/base use the same 0x510 frame,
// stack slots, CFG, event jump table, and 334 relocation occurrences.  Base is
// 0x118b bytes versus retail's 0x118c.  The first code-shape residual is +0xb0b:
// retail loads campaignChoice4 and compares gbCampaignSideChoice, while base
// loads the global and compares the same stack value; the condition is identical
// but the latter encoding is one byte shorter.  Equality operand reversal was
// canonicalized to the same base code; an explicit inequality/two-arm spelling
// added a non-retail five-byte jump.  The broad relocation audit's five base-only
// reports are gConfig owner references at retail synthetic data-alias sites, not
// extra calls or globals.  Revisit after later SMACKMGR/header TU-state changes.
VA(0x0040126d, 0x118c)
void SmackManagerMain(void) {
    int soundFlags4;
    int preloadFlags26;
    int playing16;
    int unusedPlaybackState4;
    int musicStarted36;
    int companionStarted1;
    int primaryStarted9;
    int unusedOne1 = 1;
    char path7[352];
    signed char savedPalette9[SMACK_PALETTE_SIZE];
    int unusedFrameHead36;

    gbLastFramePlayed = 0;
    musicStarted36 = 0;
    if (bSmackNum == SMACK_CHOOSE_CAMPAIGN) {
        int initialMouseY29;
        int initialMouseX3;
        int unusedInitialMouseState11;

        brotherIcon = gpResourceManager->GetIcon("brothers.icn");
        gpMouseManager->MouseCoords(initialMouseX3, initialMouseY29);
        gbCampaignSideChoice = initialMouseX3 < SMACK_SCREEN_WIDTH / 2;
    }

    KBChangeMenu(hmnuDflt);
    gpMouseManager->HideColorPointer();
    bMainDone = 1;
    memcpy(savedPalette9, gPalette->m_data, SMACK_PALETTE_SIZE);

    if (gbNoSound || !gpSoundManager->m_digitalDriver || !gConfig.soundVolume ||
        bSmackNum == SMACK_CREDITS) {
        bSmackSound = 0;
    } else {
        bSmackSound = 1;
        if (AIL_get_preference(15)) {
            SmackSoundUseMSS(reinterpret_cast<void *>(gpSoundManager->m_digitalDriver));
            LogStr("SSSS 1");
        } else {
            SmackSoundUseDirectSound(
                reinterpret_cast<SmackMilesDigitalDriver *>(
                    gpSoundManager->m_digitalDriver)->directSound);
            LogStr("SSSS 2");
        }
    }

    if (xSmackFromNetwork && bSmackNum > SMACK_FIRST_NETWORK - 1)
        strcpy(path7, "i:\\projects\\heroes\\art\\fin3d\\");
    else if (bSmackNum == SMACK_EXPANSION_CAMPAIGN)
        strcpy(path7, ".\\DATA\\");
    else
        strcpy(path7, gcAnimPath);

    if (gConfig.slowVideo)
        sprintf(gText, "%s%s.SMK", path7, SmackOptions[bSmackNum].slowFileName);
    else
        sprintf(gText, "%s%s.SMK", path7, SmackOptions[bSmackNum].fileName);
    soundFlags4 = bSmackSound ? SMACK_AUDIO_OPEN_FLAGS : 0;
    preloadFlags26 = SmackOptions[bSmackNum].preload ? SMACK_PRELOAD_OPEN_FLAG : 0;
    if (gbLowMemory && bSmackNum == SMACK_LOW_MEMORY_MOVIE && !gConfig.slowVideo)
        preloadFlags26 = 0;

    smk1 = 0;
    if (bSmackNum != SMACK_EXPANSION_CAMPAIGN) {
        while (!smk1) {
            smk1 = SmackOpen(gText, preloadFlags26 + soundFlags4, SMACKAUTOEXTRA);
            if (!smk1) {
                gpWindowManager->FadeScreen(0, SMACK_NORMAL_FADE, 0);
                NormalDialog("Error reading the Heroes 2 Expansion CD.  Retry?", 2,
                             -1, -1, -1, 0, -1, 0, -1, 0);
                if (gpWindowManager->m_dialogResult == SMACK_DIALOG_RETRY)
                    ShutDown("CDROM drive error.  Exiting.");
            }
        }
        SmackToBuffer(smk1, 0, 0, SMACK_SCREEN_WIDTH, SMACK_SCREEN_HEIGHT,
                      gpWindowManager->m_screen->m_pixels, 0);
    }

    if (strlen(SmackOptions[bSmackNum].companionFileName) > 1) {
        if (gConfig.slowVideo)
            sprintf(gText, "%s%s.SMK", path7,
                    SmackOptions[bSmackNum].slowCompanionFileName);
        else
            sprintf(gText, "%s%s.SMK", path7,
                    SmackOptions[bSmackNum].companionFileName);
        smk2 = SmackOpen(gText, bSmackSound ? SMACK_AUDIO_OPEN_FLAGS : 0,
                         SMACKAUTOEXTRA);
        if (SmackOptions[bSmackNum].drawCompanion && !gConfig.slowVideo &&
            bSmackNum != SMACK_EXPANSION_CAMPAIGN) {
            SmackToBuffer(smk2, SmackOptions[bSmackNum].companionX,
                          SmackOptions[bSmackNum].companionY,
                          SMACK_SCREEN_WIDTH, SMACK_SCREEN_HEIGHT,
                          gpWindowManager->m_screen->m_pixels, 0);
        }
    }

    FillBitmapArea(gpWindowManager->m_screen, 0, 0, SMACK_SCREEN_WIDTH,
                   SMACK_SCREEN_HEIGHT, 0);
    BlitBitmapToScreen(gpWindowManager->m_screen, 0, 0, SMACK_SCREEN_WIDTH,
                       SMACK_SCREEN_HEIGHT, 0, 0);
    if (SmackOptions[bSmackNum].fadeIn)
        gpWindowManager->FadeScreen(1, SMACK_NORMAL_FADE, 0);

    playing16 = 1;
    primaryStarted9 = 0;
    companionStarted1 = 0;

    if (bSmackNum == SMACK_CHOOSE_CAMPAIGN) {
        int eventSink;

        Process1WindowsMessage();
        while (gpInputManager->GetEvent().type != 0) {
            eventSink = 0;
        }
    }

    while (playing16) {
        if (bSmackNum == SMACK_EXPANSION_CAMPAIGN) {
            if (!primaryStarted9) {
                gpMouseManager->SetPointer("advmice.mse", SMACK_POINTER_ID,
                                           SMACK_POINTER_HIDDEN_HOTSPOT);
                gpMouseManager->ReallyShowPointer();
                gpSoundManager->PlayAmbientMusic(SMACK_MAIN_MUSIC, 0, -1);
                backImage = gpResourceManager->GetIcon("x_ivy.icn");
                if (!backImage)
                    MemError();
                backImage->DrawToBuffer(0, 0, 0, 0);
                backImage->DrawToBuffer(0, 0, 1, 0);
                sprintf(gText, "%s%s.SMK", path7, "IVYPOL");
                smk2 = SmackOpen(gText, 0, SMACKAUTOEXTRA);
                memcpy(gPalette->m_data, smk2->Palette, SMACK_PALETTE_SIZE);
                SmackClose(smk2);
                smk2 = 0;
                ConvertSmackerPalette(reinterpret_cast<unsigned char *>(gPalette->m_data));
                UpdatePalette(gPalette->m_data);
                memcpy(gpBufferPalette->m_data, gPalette->m_data, SMACK_PALETTE_SIZE);
                gpWindowManager->FadeScreen(0, SMACK_FAST_FADE, 0);
                primaryStarted9 = 1;
            }
        } else if (!SmackWait(smk1)) {
            if (bSmackNum == SMACK_INTRO_MUSIC && !musicStarted36) {
                musicStarted36 = 1;
                gpSoundManager->PlayAmbientMusic(SMACK_INTRO_SECOND_MUSIC, 0, -1);
            }
            if ((!primaryStarted9 || smk1->Frames > 1) &&
                (bSmackNum != SMACK_CONGRATS || smk1->Frames - 1 != smk1->FrameNum)) {
                DoAdvance(smk1, 1, 1,
                          !primaryStarted9 || !SmackOptions[bSmackNum].fadeIn, 0);
            }
            if (smk1->FrameNum > 0 || smk1->Frames <= 1) {
                if (!primaryStarted9) {
                    if (bSmackNum == SMACK_CHOOSE_CAMPAIGN) {
                        gpMouseManager->SetPointer("advmice.mse", SMACK_POINTER_ID,
                                                   SMACK_POINTER_HIDDEN_HOTSPOT);
                        gpMouseManager->ReallyShowPointer();
                    }
                    if (SmackOptions[bSmackNum].fadeIn) {
                        memcpy(gpBufferPalette->m_data, gPalette->m_data,
                               SMACK_PALETTE_SIZE);
                        gpWindowManager->FadeScreen(0, SMACK_FAST_FADE, 0);
                    }
                    if (bSmackNum == SMACK_CREDITS)
                        gpSoundManager->PlayAmbientMusic(SMACK_MAIN_MUSIC, 0, -1);
                    if (bSmackNum == SMACK_SPECIAL_MUSIC)
                        gpSoundManager->PlayAmbientMusic(SMACK_MAIN_MUSIC, 0, -1);
                }
                primaryStarted9 = 1;
            }
        }

        if (smk2 && primaryStarted9 && !SmackWait(smk2)) {
            if (companionStarted1 && smk2->Frames - 1 == smk2->FrameNum) {
                int drawLastFrame;
                int advanceLastFrame;

                advanceLastFrame = 0;
                if (SmackOptions[bSmackNum].drawCompanion && !gConfig.slowVideo) {
                    drawLastFrame = 1;
                } else if (bSmackNum == SMACK_EXPANSION_CAMPAIGN) {
                    drawLastFrame = 1;
                    advanceLastFrame = 1;
                } else {
                    drawLastFrame = 0;
                }
                DoAdvance(smk2, drawLastFrame, advanceLastFrame, 0, 1);
                gbLastFramePlayed = 1;
                while (SmackWait(smk2))
                    Process1WindowsMessage();
            } else {
                if (bSmackNum == SMACK_EXPANSION_CAMPAIGN)
                    DoAdvance(smk2, 1, 1, 0, 1);
                else
                    DoAdvance(smk2, SmackOptions[bSmackNum].drawCompanion,
                              1, 0, 1);
            }
            if (smk2 && smk2->FrameNum > 0)
                companionStarted1 = 1;
        }

        Process1WindowsMessage();
        tag_message message;
        message = gpInputManager->GetEvent();
        switch (message.type) {
        case SMACK_EVENT_MOUSE_MOVE:
            if (bSmackNum == SMACK_CHOOSE_CAMPAIGN) {
                int campaignMouseX5;
                int campaignMouseY3;
                int campaignChoice4;

                gpMouseManager->MouseCoords(campaignMouseX5, campaignMouseY3);
                campaignChoice4 = campaignMouseX5 < SMACK_SCREEN_WIDTH / 2;
                if (campaignChoice4 == gbCampaignSideChoice)
                    break;
                gbCampaignSideChoice = campaignChoice4;
                if (gbCampaignSideChoice == 1) {
                    brotherIcon->DrawToBuffer(0, 0, 0, 0);
                    brotherIcon->DrawToBuffer(0, 0, 3, 0);
                } else {
                    brotherIcon->DrawToBuffer(0, 0, 1, 0);
                    brotherIcon->DrawToBuffer(0, 0, 2, 0);
                }
                BlitBitmapToScreen(gpWindowManager->m_screen, 49, 78, 538, 258,
                                   49, 78);
            } else if (bSmackNum == SMACK_EXPANSION_CAMPAIGN) {
                int expansionMouseX6;
                int expansionMouseY4;
                int expansionChoice1;

                gpMouseManager->MouseCoords(expansionMouseX6, expansionMouseY4);
                expansionChoice1 = ExpansionCampaignRect(expansionMouseX6,
                                                          expansionMouseY4);
                if (expansionChoice1 != xLastChoice) {
                    backImage->DrawToBuffer(0, 0, 0, 0);
                    BlitBitmapToScreen(gpWindowManager->m_screen, 0, 0,
                                       SMACK_SCREEN_WIDTH, SMACK_SCREEN_HEIGHT, 0, 0);
                    xLastChoice = expansionChoice1;
                    if (smk2) {
                        SmackClose(smk2);
                        smk2 = 0;
                    }
                    if (expansionChoice1 != -1) {
                        bExpansionSmackNum = static_cast<signed char>(
                            expansionChoice1 + SMACK_EXPANSION_FIRST_MOVIE);
                        sprintf(gText, "%s%s.SMK", path7,
                                SmackOptions[bExpansionSmackNum].fileName);
                        smk2 = SmackOpen(gText,
                                         bSmackSound ? SMACK_AUDIO_OPEN_FLAGS : 0,
                                         SMACKAUTOEXTRA);
                        SmackToBuffer(smk2,
                                      SmackOptions[bExpansionSmackNum].companionX,
                                      SmackOptions[bExpansionSmackNum].companionY,
                                      SMACK_SCREEN_WIDTH,
                                      SMACK_SCREEN_HEIGHT,
                                      gpWindowManager->m_screen->m_pixels, 0);
                        backImage->DrawToBuffer(0, 0, 1, 0);
                    }
                }
            }
            break;
        case SMACK_EVENT_KEY:
            if (message.payload.keyboard.keyCode == SMACK_ALT_KEY)
                break;
        case SMACK_EVENT_QUIT:
            if (bSmackNum == SMACK_CHOOSE_CAMPAIGN)
                break;
            if (bSmackNum == SMACK_EXPANSION_CAMPAIGN)
                break;
        case SMACK_EVENT_MOUSE_CLICK:
            if (bSmackNum == SMACK_EARTH)
                break;
            if (bSmackNum == SMACK_EXPANSION_CAMPAIGN && xLastChoice == -1)
                break;
            goto playbackDone;
            break;
        default:
            break;
        }

        if (bSmackNum == SMACK_CONGRATS && smk1->FrameNum + 1 == smk1->Frames &&
            !musicStarted36) {
            musicStarted36 = 1;
            gpSoundManager->PlayAmbientMusic(SMACK_LOSE_MUSIC, 0, -1);
        }

        if (!SmackOptions[bSmackNum].waitForInput &&
            (gbLastFramePlayed ||
             (smk2 && (smk2->Frames - 1 <= smk2->FrameNum ||
                       (!smk2->FrameNum && companionStarted1))) ||
             (!smk2 && (smk1->Frames <= smk1->FrameNum ||
                        (!smk1->FrameNum && primaryStarted9))))) {
            playing16 = 0;
            gbPlayedThrough = 1;
        }
    }

playbackDone:
    if (bSmackNum == SMACK_CHOOSE_CAMPAIGN) {
        gpMouseManager->HideColorPointer();
        gpMouseManager->SetPointer("advmice.mse", SMACK_POINTER_DEFAULT,
                                   SMACK_POINTER_HIDDEN_HOTSPOT);
    }

    if (SmackOptions[bSmackNum].fadeOut) {
        memcpy(gpBufferPalette->m_data, gPalette->m_data, SMACK_PALETTE_SIZE);
        gpWindowManager->FadeScreen(1, SMACK_SHORT_FADE, 0);
        FillBitmapArea(gpWindowManager->m_screen, 0, 0, SMACK_SCREEN_WIDTH,
                       SMACK_SCREEN_HEIGHT, SMACK_BACKGROUND_COLOR);
        BlitBitmapToScreen(gpWindowManager->m_screen, 0, 0, SMACK_SCREEN_WIDTH,
                           SMACK_SCREEN_HEIGHT, 0, 0);
    } else if (!gbPlayedThrough && bSmackNum != SMACK_CONGRATS) {
        memcpy(gpBufferPalette->m_data, gPalette->m_data, SMACK_PALETTE_SIZE);
        gpWindowManager->FadeScreen(1, SMACK_NORMAL_FADE, 0);
        FillBitmapArea(gpWindowManager->m_screen, 0, 0, SMACK_SCREEN_WIDTH,
                       SMACK_SCREEN_HEIGHT, SMACK_BACKGROUND_COLOR);
        BlitBitmapToScreen(gpWindowManager->m_screen, 0, 0, SMACK_SCREEN_WIDTH,
                           SMACK_SCREEN_HEIGHT, 0, 0);
    }

    if (bTesting)
        SmackSummary(smk1, &smksum);
    if (smk1)
        SmackClose(smk1);
    smk1 = 0;
    if (smk2)
        SmackClose(smk2);
    smk2 = 0;
    if (bSmackNum != SMACK_CONGRATS) {
        memcpy(gPalette->m_data, savedPalette9, SMACK_PALETTE_SIZE);
        UpdatePalette(gPalette->m_data);
    }
    gpMouseManager->ShowColorPointer();
    if (brotherIcon)
        gpResourceManager->Dispose(static_cast<resource *>(brotherIcon));
    brotherIcon = 0;
    if (backImage)
        gpResourceManager->Dispose(static_cast<resource *>(backImage));
    backImage = 0;
}

VA(0x004023f9, 0x56)
void ShutDownSmacker(void) {
    if (smk1)
        SmackClose(smk1);
    smk1 = 0;
    if (smk2)
        SmackClose(smk2);
    smk2 = 0;
}

// @early-stop
// reloc-masked: code bytes are identical; delinked gConfig/SmackSum field symbols differ.
VA(0x0040244f, 0x17f)
int PlaySmacker(int smackNumber) {
    signed char savedPalette[SMACK_PALETTE_SIZE];
    int savedUpdateFlags;

    xLastChoice = -1;
    if (gbNoCDRom)
        return 0;

    gbInSmackMgr = 1;
    gbPlayedThrough = 0;
    memcpy(savedPalette, gpBufferPalette->m_data, SMACK_PALETTE_SIZE);
    savedUpdateFlags = gpWindowManager->m_updateFlags;
    gpWindowManager->m_updateFlags = 0;
    if (smackNumber != SMACK_EXPANSION_CAMPAIGN) {
        gpSoundManager->m_samplesReady = 1;
        gpSoundManager->PlayAmbientMusic(-1, 0, -1);
    }
    if (gConfig.slowVideo == 3) {
        gConfig.slowVideo = 0;
        WritePrefs();
        bSmackNum = SMACK_EARTH;
        bTesting = 1;
        SmackManagerMain();
        bTesting = 0;
        if (smksum.TotalReadTime + smksum.TotalOpenTime >= 2000 ||
            smksum.TotalDecompTime >= 1300 || gbLowMemory) {
            gConfig.slowVideo = 1;
            WritePrefs();
        }
        PrintSummaryInfo(&smksum);
    }
    bSmackNum = static_cast<signed char>(smackNumber);
    SmackManagerMain();
    memcpy(gpBufferPalette->m_data, savedPalette, SMACK_PALETTE_SIZE);
    gpWindowManager->m_updateFlags = savedUpdateFlags;
    gbInSmackMgr = 0;
    return gbPlayedThrough;
}

DATA(0x004ec040) int bSmackSound = 0;
DATA(0x004ec044) icon *brotherIcon = 0;
static tag_rect expansionCampaignRects[SMACK_EXPANSION_RECT_COUNT] = {
    {215, 49, 230, 150}, {217, 275, 230, 150},
    {475, 132, 120, 180}, {41, 132, 120, 180}
};

VA(0x004025ce, 0x65)
int ExpansionCampaignRect(int x, int y) {
    int i;
    for (i = 0; i < SMACK_EXPANSION_RECT_COUNT; ++i) {
        if (PointInRect(x, y, &expansionCampaignRects[i]))
            return i;
    }
    return -1;
}

VA(0x00402633, 0x89)
signed char PointInRect(int x, int y, tag_rect *rect) {
    if (x < rect->x)
        return 0;
    if (x >= rect->x + rect->width)
        return 0;
    if (y < rect->y)
        return 0;
    if (y >= rect->y + rect->height)
        return 0;
    return 1;
}

VA(0x004026bc, 0x251)
void PrintSummaryInfo(SmackSum *summary) {
    sprintf(gText, "                                        Name - %s",
            SmackOptions[bSmackNum].fileName);
    LogStr(gText);
#define LOG_SUMMARY_VALUE(format, value) \
    sprintf(gText, format, value);       \
    LogStr(gText)
    LOG_SUMMARY_VALUE("                                    total time - %8d", summary->TotalTime);
    LOG_SUMMARY_VALUE("MS*100 per frame (100000/MS100PerFrame=Frames/Sec) - %8d",
                      summary->MS100PerFrame);
    LOG_SUMMARY_VALUE("        Time to open and prepare for decompression - %8d",
                      summary->TotalOpenTime);
    LOG_SUMMARY_VALUE("                            Total Frames displayed - %8d",
                      summary->TotalFrames);
    LOG_SUMMARY_VALUE("                    Total number of skipped frames - %8d",
                      summary->SkippedFrames);
    LOG_SUMMARY_VALUE("                           Total time spent blitting - %8d",
                      summary->TotalBlitTime);
    LOG_SUMMARY_VALUE("                            Total time spent reading - %8d",
                      summary->TotalReadTime);
    LOG_SUMMARY_VALUE("                    Total time spent decompressing - %8d",
                      summary->TotalDecompTime);
    LOG_SUMMARY_VALUE("                   Total io speed (sbytes/second) - %8d",
                      summary->TotalReadSpeed);
    LOG_SUMMARY_VALUE("                           Slowest single frame time - %8d",
                      summary->SlowestFrameTime);
    LOG_SUMMARY_VALUE("                  Second slowest single frame time - %8d",
                      summary->Slowest2FrameTime);
    LOG_SUMMARY_VALUE("                         Slowest single frame number - %8d",
                      summary->SlowestFrameNum);
    LOG_SUMMARY_VALUE("                Second slowest single frame number - %8d",
                      summary->Slowest2FrameNum);
    LOG_SUMMARY_VALUE("                           Average size of the frame - %8d",
                      summary->AverageFrameSize);
    LOG_SUMMARY_VALUE("                Highest amount of memory allocated - %8d",
                      summary->HighestExtraUsed);
#undef LOG_SUMMARY_VALUE
}

// ---- globals (definitions, RVA order) ----
DATA(0x004ec068) icon *backImage = 0;
DATA(0x004ec070) SSmackOptions SmackOptions[73] = {
    {"H2INTRO", "", "SH2INTRO", "", 1, 1, 0, 0, 0, 0, 0},
    {"NWCLOGO", "", "SNWCLOGO", "", 1, 1, 0, 0, 0, 0, 0},
    {"WIN", "", "SWINS", "", 1, 0, 0, 1, 0, 0, 0},
    {"LOSE", "", "SLOSE", "", 1, 1, 1, 1, 0, 0, 0},
    {"INTRO", "", "SINTRO", "", 1, 1, 0, 0, 0, 0, 0},
    {"GOOD01", "GOOD01V", "SGOOD01", "SGOOD01V", 1, 1, 1, 0, 1, 348, 420},
    {"GOOD02", "GOOD02W", "SGOOD02", "GOOD02W", 1, 1, 1, 0, 0, 0, 0},
    {"GOOD03", "GOOD03QW", "SGOOD03", "GOOD03QW", 1, 1, 1, 0, 0, 0, 0},
    {"GOOD03", "GOOD03QW", "SGOOD03", "GOOD03QW", 1, 1, 1, 0, 0, 0, 0},
    {"GOOD04", "GOOD04W", "SGOOD04", "GOOD04W", 1, 1, 1, 0, 0, 0, 0},
    {"GOOD05", "GOOD05V", "SGOOD05", "SGOOD05V", 1, 1, 1, 0, 1, 87, 345},
    {"GOOD06", "GOOD06AV", "SGOOD06", "SGOOD6AV", 1, 1, 1, 0, 1, 280, 186},
    {"GOOD06", "GOOD06BV", "SGOOD06", "SGOOD6BV", 1, 1, 1, 0, 1, 280, 186},
    {"GOOD07", "GOOD07QW", "SGOOD07", "GOOD07QW", 1, 1, 1, 0, 0, 0, 0},
    {"GOOD07", "GOOD07QW", "SGOOD07", "GOOD07QW", 1, 1, 1, 0, 0, 0, 0},
    {"GOOD09", "GOOD09W", "SGOOD09", "GOOD09W", 1, 1, 1, 0, 0, 0, 0},
    {"GOOD10", "GOOD10W", "SGOOD10", "GOOD10W", 1, 1, 1, 0, 0, 0, 0},
    {"GOOD10", "GOOD10W", "SGOOD10", "GOOD10W", 1, 1, 1, 0, 0, 0, 0},
    {"LIBRARY", "LIBRARYW", "SLIBRARY", "LIBRARYW", 1, 1, 1, 0, 0, 0, 0},
    {"EVIL01", "EVIL01V", "SEVIL01", "SEVIL01V", 1, 1, 1, 0, 1, 364, 404},
    {"EVIL02", "EVIL02W", "SEVIL02", "EVIL02W", 1, 1, 1, 0, 0, 0, 0},
    {"EVIL03", "EVIL03QW", "SEVIL03", "EVIL03QW", 1, 1, 1, 0, 0, 0, 0},
    {"EVIL03", "EVIL03QW", "SEVIL03", "EVIL03QW", 1, 1, 1, 0, 0, 0, 0},
    {"EVIL05", "EVIL05AV", "SEVIL05", "SEVIL5AV", 1, 1, 1, 0, 1, 87, 345},
    {"EVIL05", "EVIL05BV", "SEVIL05", "SEVIL5BV", 1, 1, 1, 0, 1, 87, 345},
    {"EVIL05", "RBETRAYV", "SEVIL05", "SBETRAYV", 1, 1, 1, 0, 1, 87, 345},
    {"EVIL06", "EVIL06AW", "SEVIL06", "EVIL06AW", 1, 1, 1, 0, 0, 0, 0},
    {"EVIL06", "EVIL06BW", "SEVIL06", "EVIL06BW", 1, 1, 1, 0, 0, 0, 0},
    {"EVIL07", "EVIL07W", "SEVIL07", "EVIL07W", 1, 1, 1, 0, 0, 0, 0},
    {"EVIL07", "EVIL07W", "SEVIL07", "EVIL07W", 1, 1, 1, 0, 0, 0, 0},
    {"EVIL08", "", "SEVIL08", "", 1, 1, 1, 0, 0, 0, 0},
    {"EVIL09", "EVIL09W", "SEVIL09", "EVIL09W", 1, 1, 1, 0, 0, 0, 0},
    {"EVIL09", "EVIL09W", "SEVIL09", "EVIL09W", 1, 1, 1, 0, 0, 0, 0},
    {"EVIL10", "EVIL11W", "SEVIL10", "EVIL11W", 1, 1, 1, 0, 0, 0, 0},
    {"PRISON", "", "SPRISON", "", 1, 1, 0, 0, 0, 0, 0},
    {"CHOOSE", "CHOOSEW", "SCHOOSE", "CHOOSEW", 1, 1, 1, 1, 0, 0, 0},
    {"CREDITS", "", "CREDITS", "", 0, 0, 1, 1, 0, 0, 0},
    {"EARTH", "", "EARTH", "", 1, 1, 0, 0, 0, 0, 0},
    {"MM6", "", "SMM6", "", 1, 1, 0, 0, 0, 0, 0},
    {"POL1", "MIXPOL1", "SPOL1", "MIXPOL1", 1, 1, 1, 0, 0, 0, 0},
    {"POL2", "MIXPOL2", "SPOL2", "MIXPOL2", 1, 1, 1, 0, 0, 0, 0},
    {"POL3", "MIXPOL3", "SPOL3", "MIXPOL3", 1, 1, 1, 0, 0, 0, 0},
    {"POL4", "MIXPOL4", "SPOL4", "MIXPOL4", 1, 1, 1, 0, 0, 0, 0},
    {"POL5", "MIXPOL5", "SPOL5", "MIXPOL5", 1, 1, 1, 0, 0, 0, 0},
    {"POL6", "MIXPOL6", "SPOL6", "MIXPOL6", 1, 1, 1, 0, 0, 0, 0},
    {"POL7", "MIXPOL7", "SPOL7", "MIXPOL7", 1, 1, 1, 0, 0, 0, 0},
    {"POL8", "MIXPOL8", "SPOL8", "MIXPOL8", 1, 1, 1, 0, 0, 0, 0},
    {"DES9", "MIXDES9", "SDES9", "MIXDES9", 1, 1, 1, 0, 0, 0, 0},
    {"DES10", "MIXDES10", "SDES10", "MIXDES10", 1, 1, 1, 0, 0, 0, 0},
    {"DES11", "MIXDES11", "SDES11", "MIXDES11", 1, 1, 1, 0, 0, 0, 0},
    {"DES12", "MIXDES12", "SDES12", "MIXDES12", 1, 1, 1, 0, 0, 0, 0},
    {"DES13", "MIXDES13", "SDES13", "MIXDES13", 1, 1, 1, 0, 0, 0, 0},
    {"DES14", "MIXDES14", "SDES14", "MIXDES14", 1, 1, 1, 0, 0, 0, 0},
    {"DES15", "MIXDES15", "SDES15", "MIXDES15", 1, 1, 1, 0, 0, 0, 0},
    {"WIZ16", "MIXWIZ16", "SWIZ16", "MIXWIZ16", 1, 1, 1, 0, 0, 0, 0},
    {"WIZ17", "MIXWIZ17", "SWIZ17", "MIXWIZ17", 1, 1, 1, 0, 0, 0, 0},
    {"WIZ18", "MIXWIZ18", "SWIZ18", "MIXWIZ18", 1, 1, 1, 0, 0, 0, 0},
    {"WIZ19", "MIXWIZ19", "SWIZ19", "MIXWIZ19", 1, 1, 1, 0, 0, 0, 0},
    {"WIZ20", "MIXWIZ20", "SWIZ20", "MIXWIZ20", 1, 1, 1, 0, 0, 0, 0},
    {"VOY21", "MIXVOY21", "SVOY21", "MIXVOY21", 1, 1, 1, 0, 0, 0, 0},
    {"VOY22", "MIXVOY22", "SVOY22", "MIXVOY22", 1, 1, 1, 0, 0, 0, 0},
    {"VOY23", "MIXVOY23", "SVOY23", "MIXVOY23", 1, 1, 1, 0, 0, 0, 0},
    {"VOY24", "MIXVOY24", "SVOY24", "MIXVOY24", 1, 1, 1, 0, 0, 0, 0},
    {"VOY25", "MIXVOY25", "SVOY25", "MIXVOY25", 1, 1, 1, 0, 0, 0, 0},
    {"DEFEAT", "", "SDEFEAT", "", 1, 1, 1, 1, 0, 0, 0},
    {"H2XINTRO", "", "SH2XINTR", "", 1, 1, 0, 0, 0, 0, 0},
    {"CYLOGO", "", "SCYLOGO", "", 1, 1, 1, 0, 0, 0, 0},
    {"", "", "", "", 1, 1, 1, 1, 0, 0, 0},
    {"IVYPOL", "", "IVYPOL", "", 0, 0, 1, 1, 1, 214, 47},
    {"IVYDES", "", "IVYDES", "", 0, 0, 1, 1, 1, 214, 273},
    {"IVYWIZ", "", "IVYWIZ", "", 0, 0, 1, 1, 1, 472, 132},
    {"IVYVOY", "", "IVYVOY", "", 0, 0, 1, 1, 1, 41, 132},
    {"CYBCREDS", "", "SCYBCRED", "", 1, 0, 1, 1, 0, 0, 0}
};
DATA(0x004ecd48) int bTesting = 0;
DATA(0x004ecd4c) Smack *smk1 = 0;
DATA(0x004ecd50) Smack *smk2 = 0;
DATA(0x00522f20) signed char bSmackNum = 0;
DATA(0x00522f24) int gbLastFramePlayed = 0;
DATA(0x00522f28) SmackSum smksum = {0};
DATA(0x00522f80) int gbPlayedThrough = 0;
DATA(0x00522f84) signed char bMainDone = 0;
