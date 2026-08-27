#include <Ints.h>
#include <BASE/message.h>
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
#include <SOURCE/ExpCampaign.h>
#include <SOURCE/SMACKMGR.h>
#include <SOURCE/X_GLOBAL.h>
#include <PLATFORM/Runtime.h>
#include <SOURCE/wingraph.h>
#include <PLATFORM/Movie.h>
#include <stdio.h>
#include <string.h>

#include <string>

typedef enum SmackManagerConstant {
    PALETTE_VALUE_SHIFT            = 2,
    AUDIO_OPEN_FLAGS               = 0xfe000,
    NORMAL_FADE                    = 0x80,
    SHORT_FADE                     = 8,
    FAST_FADE                      = 4,
    CHOOSE_CAMPAIGN                = 35,
    SMACK_CREDITS                  = 36,
    SMACK_EARTH                    = 37,
    FIRST_NETWORK                  = 39,
    EXPANSION_CAMPAIGN             = 67,
    EXPANSION_FIRST_MOVIE          = 68,
    SPECIAL_MUSIC                  = 72,
    BUKA_LOGO                      = 73,
    BUKA_CREDITS                   = 74,
    CONGRATS                       = 2,
    INTRO_MUSIC                    = 3,
    LOW_MEMORY_MOVIE               = 30,
    BACKGROUND_COLOR               = 36,
    MAIN_MUSIC                     = 42,
    LOSE_MUSIC                     = 43,
    INTRO_SECOND_MUSIC             = 19,
    POINTER_ID                     = 40,
    POINTER_DEFAULT                = 0,
    EXPANSION_RECT_COUNT           = 4,
    CAMPAIGN_DIVIDER_X             = GRAPHICS_WIDTH / 2,
    CAMPAIGN_LEFT_FRAME            = 0,
    CAMPAIGN_RIGHT_FRAME           = 1,
    CAMPAIGN_RIGHT_SELECTED_FRAME  = 2,
    CAMPAIGN_LEFT_SELECTED_FRAME   = 3,
    CONGRATS_FIRST_FRAME           = 36,
    CONGRATS_TEXT_X                = 409,
    CONGRATS_TEXT_Y                = 98,
    CONGRATS_TEXT_WIDTH            = 134,
    CONGRATS_TEXT_HEIGHT           = 217,
    CONGRATS_BLIT_WIDTH            = GRAPHICS_WIDTH - 1,
    CONGRATS_BLIT_HEIGHT           = GRAPHICS_HEIGHT - 1,
    MILES_SOUND_SYSTEM_PREFERENCE  = 15,
    CAMPAIGN_BLIT_X                = 49,
    CAMPAIGN_BLIT_Y                = 78,
    CAMPAIGN_BLIT_WIDTH            = 538,
    CAMPAIGN_BLIT_HEIGHT           = 258,
    VIDEO_SPEED_TEST               = 3,
    VIDEO_OPEN_READ_SLOW_THRESHOLD = 2000,
    VIDEO_DECOMP_SLOW_THRESHOLD    = 1300,
    SMACK_OPTION_COUNT             = 75
} SmackManagerConstant;

static i8 bExpansionSmackNum;

void ConvertSmackerPalette(u8* paletteData) {
    i32 i;

    for (i = 0; i < PALETTE_DATA_SIZE; ++i)
        paletteData[i] =
            static_cast<u8>(static_cast<i32>(paletteData[i]) >> PALETTE_VALUE_SHIFT);
}

void DoAdvance(
    platform::MovieId movie,
    i32 drawFrame,
    i32 advanceFrame,
    i32 updatePalette,
    i32 skipPalette
) {
    platform::MovieFrame frame;
    if (drawFrame && platform::MovieDraw(movie, frame)) {
        if (frame.palette != NULL && !skipPalette) {
            memcpy(gPalette->m_data, frame.palette, PALETTE_DATA_SIZE);
            ConvertSmackerPalette(reinterpret_cast<u8*>(gPalette->m_data));
            if (updatePalette)
                UpdatePalette(gPalette->m_data);
        }

        if (bSmackNum == CHOOSE_CAMPAIGN) {
            if (gbCampaignSideChoice == CAMPAIGN_ARCHIBALD) {
                brotherIcon->DrawToBuffer(0, 0, CAMPAIGN_LEFT_FRAME, ICON_DRAW_NORMAL);
                brotherIcon->DrawToBuffer(0, 0, CAMPAIGN_LEFT_SELECTED_FRAME, ICON_DRAW_NORMAL);
            } else {
                brotherIcon->DrawToBuffer(0, 0, CAMPAIGN_RIGHT_FRAME, ICON_DRAW_NORMAL);
                brotherIcon->DrawToBuffer(0, 0, CAMPAIGN_RIGHT_SELECTED_FRAME, ICON_DRAW_NORMAL);
            }
        }
        if (bSmackNum == EXPANSION_CAMPAIGN && xLastChoice != EXPANSION_CAMPAIGN_NONE)
            backImage->DrawToBuffer(0, 0, 1, ICON_DRAW_NORMAL);

        const bool congrats = bSmackNum == CONGRATS && frame.index >= CONGRATS_FIRST_FRAME;
        if (congrats) {
            smallFont->DrawBoundedString(
                congratsText,
                CONGRATS_TEXT_X,
                CONGRATS_TEXT_Y,
                CONGRATS_TEXT_WIDTH,
                CONGRATS_TEXT_HEIGHT,
                FONT_DRAW_SCENARIO_WIN,
                FONT_ALIGN_CENTER_BOTH
            );
        }

        // The frame the text first lands on has to repaint the whole banner.
        if (congrats && frame.index == CONGRATS_FIRST_FRAME) {
            BlitBitmapToScreen(
                gpWindowManager->m_screen,
                0,
                0,
                CONGRATS_BLIT_WIDTH,
                CONGRATS_BLIT_HEIGHT,
                0,
                0
            );
        } else {
            BlitBitmapToScreen(
                gpWindowManager->m_screen,
                frame.dirty.x,
                frame.dirty.y,
                frame.dirty.width,
                frame.dirty.height,
                frame.dirty.x,
                frame.dirty.y
            );
        }
    }
    if (advanceFrame)
        platform::MovieAdvance(movie);
}

void SmackManagerMain(void) {
    b32 playing17;
    b32 musicStarted0;
    b32 companionStarted26;
    b32 primaryStarted7;
    i8 savedPalette4[PALETTE_DATA_SIZE];

    gpSoundManager->SaveBackend();
    gbLastFramePlayed = false;
    musicStarted0 = false;
    if (bSmackNum == CHOOSE_CAMPAIGN) {
        i32 initialMouseY29;
        i32 initialMouseX3;

        brotherIcon = gpResourceManager->GetIcon(
            "brothers.icn"
        );
        gpMouseManager->MouseCoords(initialMouseX3, initialMouseY29);
        if (initialMouseX3 < CAMPAIGN_DIVIDER_X)
            gbCampaignSideChoice = CAMPAIGN_ARCHIBALD;
        else
            gbCampaignSideChoice = CAMPAIGN_ROLAND;
    }

    platform::ChangeMenu(hmnuDflt);
    gpMouseManager->HideColorPointer();
    bMainDone = true;
    memcpy(savedPalette4, gPalette->m_data, PALETTE_DATA_SIZE);

    bSmackSound = IsSoundBackendActive(gpSoundManager)
               && gConfig.soundVolume != CONFIG_VOLUME_MUTED
               && bSmackNum != SMACK_CREDITS
               && bSmackNum != BUKA_CREDITS;

    const std::string movieDirectory =
        bSmackNum == EXPANSION_CAMPAIGN ? ".\\DATA\\"
                                        : std::string(gcRegCDRomPath) + gcAnimPath;
    const auto moviePath = [&](const char* name) {
        return movieDirectory + name + ".SMK";
    };
    const std::string primaryMoviePath = moviePath(
        gConfig.slowVideo ? SmackOptions[bSmackNum].slowFileName
                          : SmackOptions[bSmackNum].fileName
    );

    smk1 = platform::kInvalidMovie;
    if (bSmackNum != EXPANSION_CAMPAIGN) {
        smk1 = platform::MovieOpen(primaryMoviePath.c_str(), bSmackSound);
        if (smk1 == platform::kInvalidMovie) {
            // Ironfist plays on without the CD: missing movies are skipped
            // and the campaign chooser gets a still backdrop instead.
            cmpnNoCD = gpResourceManager->GetIcon("CMPNNOCD.icn");
        } else {
            platform::MovieTarget(
                smk1,
                gpWindowManager->m_screen->m_pixels,
                GRAPHICS_WIDTH,
                GRAPHICS_HEIGHT,
                0,
                0
            );
        }
    }

    if (smk1 != platform::kInvalidMovie
        && strlen(SmackOptions[bSmackNum].companionFileName) > 1) {
        const std::string companionMoviePath = moviePath(
            gConfig.slowVideo ? SmackOptions[bSmackNum].slowCompanionFileName
                              : SmackOptions[bSmackNum].companionFileName
        );
        smk2 = platform::MovieOpen(companionMoviePath.c_str(), bSmackSound);
        if (SmackOptions[bSmackNum].drawCompanion && !gConfig.slowVideo
            && bSmackNum != EXPANSION_CAMPAIGN) {
            platform::MovieTarget(
                smk2,
                gpWindowManager->m_screen->m_pixels,
                GRAPHICS_WIDTH,
                GRAPHICS_HEIGHT,
                SmackOptions[bSmackNum].companionX,
                SmackOptions[bSmackNum].companionY
            );
        }
    }

    FillBitmapArea(gpWindowManager->m_screen, 0, 0, GRAPHICS_WIDTH, GRAPHICS_HEIGHT, 0);
    BlitBitmapToScreen(
        gpWindowManager->m_screen,
        0,
        0,
        GRAPHICS_WIDTH,
        GRAPHICS_HEIGHT,
        0,
        0
    );
    if (SmackOptions[bSmackNum].fadeIn)
        gpWindowManager->FadeScreen(FADE_OUT, NORMAL_FADE, NULL);

    playing17 = true;
    primaryStarted7 = false;
    companionStarted26 = false;

    if (bSmackNum == CHOOSE_CAMPAIGN) {
        if (smk1 == platform::kInvalidMovie) {
            gpWindowManager->FadeScreen(FADE_IN, FAST_FADE, NULL);
            cmpnNoCD->DrawToBuffer(0, 0, 0, ICON_DRAW_NORMAL);
            BlitBitmapToScreen(
                gpWindowManager->m_screen,
                0,
                0,
                GRAPHICS_WIDTH,
                GRAPHICS_HEIGHT,
                0,
                0
            );
            gpMouseManager->SetPointer("advmice.mse", POINTER_ID, MOUSE_AUTO_CURSOR_TYPE);
            gpMouseManager->ReallyShowPointer();
        }
        platform::PumpEvents();
        while (gpInputManager->GetEvent().type != MESSAGE_NONE)
            ;
    }

    while (playing17) {
        if (smk1 == platform::kInvalidMovie && bSmackNum != CHOOSE_CAMPAIGN
            && bSmackNum != EXPANSION_CAMPAIGN)
            break;
        if (bSmackNum == EXPANSION_CAMPAIGN) {
            if (!primaryStarted7) {
                gpMouseManager->SetPointer(
                    "advmice.mse",
                    POINTER_ID,
                    MOUSE_AUTO_CURSOR_TYPE
                );
                gpMouseManager->ReallyShowPointer();
                gpSoundManager->PlayAmbientMusic(MAIN_MUSIC);
                backImage = gpResourceManager->GetIcon(
                    "x_ivy.icn"
                );
                if (!backImage)
                    MemError();
                backImage->DrawToBuffer(0, 0, 0, ICON_DRAW_NORMAL);
                backImage->DrawToBuffer(0, 0, 1, ICON_DRAW_NORMAL);
                const std::string paletteMoviePath = moviePath("IVYPOL");
                smk2 = platform::MovieOpen(paletteMoviePath.c_str(), false);
                if (const u8* moviePalette = platform::MoviePalette(smk2))
                    memcpy(gPalette->m_data, moviePalette, PALETTE_DATA_SIZE);
                platform::MovieClose(smk2);
                smk2 = platform::kInvalidMovie;
                ConvertSmackerPalette(reinterpret_cast<u8*>(gPalette->m_data));
                UpdatePalette(gPalette->m_data);
                memcpy(gpBufferPalette->m_data, gPalette->m_data, PALETTE_DATA_SIZE);
                gpWindowManager->FadeScreen(FADE_IN, FAST_FADE, NULL);
                primaryStarted7 = true;
            }
        } else if (smk1 != platform::kInvalidMovie && !platform::MovieWaiting(smk1)) {
            if (bSmackNum == INTRO_MUSIC && !musicStarted0) {
                musicStarted0 = true;
                gpSoundManager->PlayAmbientMusic(INTRO_SECOND_MUSIC);
            }
            if ((!primaryStarted7 || platform::MovieFrameCount(smk1) > 1)
                && (bSmackNum != CONGRATS
                    || platform::MovieFrameIndex(smk1)
                        != platform::MovieFrameCount(smk1) - 1)) {
                DoAdvance(smk1, 1, 1, primaryStarted7 || !SmackOptions[bSmackNum].fadeIn, 0);
            }
            if (platform::MovieFrameIndex(smk1) > 0 || platform::MovieFrameCount(smk1) <= 1) {
                if (!primaryStarted7) {
                    if (bSmackNum == CHOOSE_CAMPAIGN) {
                        gpMouseManager->SetPointer(
                            "advmice.mse",
                            POINTER_ID,
                            MOUSE_AUTO_CURSOR_TYPE
                        );
                        gpMouseManager->ReallyShowPointer();
                    }
                    if (SmackOptions[bSmackNum].fadeIn) {
                        memcpy(gpBufferPalette->m_data, gPalette->m_data, PALETTE_DATA_SIZE);
                        gpWindowManager->FadeScreen(FADE_IN, FAST_FADE, NULL);
                    }
                    if (bSmackNum == SMACK_CREDITS || bSmackNum == BUKA_CREDITS)
                        gpSoundManager->PlayAmbientMusic(MAIN_MUSIC);
                    if (bSmackNum == SPECIAL_MUSIC)
                        gpSoundManager->PlayAmbientMusic(MAIN_MUSIC);
                }
                primaryStarted7 = true;
            }
        }

        if (smk2 != platform::kInvalidMovie && primaryStarted7
            && !platform::MovieWaiting(smk2)) {
            if (companionStarted26
                && platform::MovieFrameIndex(smk2)
                    == platform::MovieFrameCount(smk2) - 1) {
                b32 drawLastFrame;
                b32 advanceLastFrame2;

                advanceLastFrame2 = false;
                if (SmackOptions[bSmackNum].drawCompanion && !gConfig.slowVideo) {
                    drawLastFrame = true;
                } else if (bSmackNum == EXPANSION_CAMPAIGN) {
                    drawLastFrame = true;
                    advanceLastFrame2 = true;
                } else {
                    drawLastFrame = false;
                }
                DoAdvance(smk2, drawLastFrame, advanceLastFrame2, 0, 1);
                gbLastFramePlayed = true;
                while (platform::MovieWaiting(smk2))
                    platform::PumpEvents();
            } else {
                if (bSmackNum == EXPANSION_CAMPAIGN)
                    DoAdvance(smk2, 1, 1, 0, 1);
                else
                    DoAdvance(smk2, SmackOptions[bSmackNum].drawCompanion, 1, 0, 1);
            }
            if (smk2 != platform::kInvalidMovie && platform::MovieFrameIndex(smk2) > 0)
                companionStarted26 = true;
        }

        platform::PumpEvents();
        tag_message message;
        message = gpInputManager->GetEvent();
        switch (message.type) {
            case MESSAGE_MOUSE_MOVE:
                if (bSmackNum == CHOOSE_CAMPAIGN) {
                    i32 campaignMouseX5;
                    i32 campaignMouseY3;
                    CampaignSide campaignChoice4;

                    gpMouseManager->MouseCoords(campaignMouseX5, campaignMouseY3);
                    if (campaignMouseX5 < CAMPAIGN_DIVIDER_X)
                        campaignChoice4 = CAMPAIGN_ARCHIBALD;
                    else
                        campaignChoice4 = CAMPAIGN_ROLAND;
                    if (campaignChoice4 == gbCampaignSideChoice)
                        break;
                    gbCampaignSideChoice = campaignChoice4;
                    if (!smk1) {
                        cmpnNoCD->DrawToBuffer(0, 0, 0, ICON_DRAW_NORMAL);
                        BlitBitmapToScreen(
                            gpWindowManager->m_screen,
                            0,
                            0,
                            GRAPHICS_WIDTH,
                            GRAPHICS_HEIGHT,
                            0,
                            0
                        );
                    }
                    if (gbCampaignSideChoice == CAMPAIGN_ARCHIBALD) {
                        brotherIcon->DrawToBuffer(0, 0, CAMPAIGN_LEFT_FRAME, ICON_DRAW_NORMAL);
                        brotherIcon->DrawToBuffer(0, 0, CAMPAIGN_LEFT_SELECTED_FRAME, ICON_DRAW_NORMAL);
                    } else {
                        brotherIcon->DrawToBuffer(0, 0, CAMPAIGN_RIGHT_FRAME, ICON_DRAW_NORMAL);
                        brotherIcon->DrawToBuffer(0, 0, CAMPAIGN_RIGHT_SELECTED_FRAME, ICON_DRAW_NORMAL);
                    }
                    BlitBitmapToScreen(
                        gpWindowManager->m_screen,
                        CAMPAIGN_BLIT_X,
                        CAMPAIGN_BLIT_Y,
                        CAMPAIGN_BLIT_WIDTH,
                        CAMPAIGN_BLIT_HEIGHT,
                        CAMPAIGN_BLIT_X,
                        CAMPAIGN_BLIT_Y
                    );
                } else if (bSmackNum == EXPANSION_CAMPAIGN) {
                    i32 expansionMouseX6;
                    i32 expansionMouseY4;
                    ExpansionCampaignId expansionChoice0;

                    gpMouseManager->MouseCoords(expansionMouseX6, expansionMouseY4);
                    expansionChoice0 = ExpansionCampaignRect(expansionMouseX6, expansionMouseY4);
                    if (expansionChoice0 != xLastChoice) {
                        backImage->DrawToBuffer(0, 0, 0, ICON_DRAW_NORMAL);
                        BlitBitmapToScreen(
                            gpWindowManager->m_screen,
                            0,
                            0,
                            GRAPHICS_WIDTH,
                            GRAPHICS_HEIGHT,
                            0,
                            0
                        );
                        xLastChoice = expansionChoice0;
                        if (smk2 != platform::kInvalidMovie) {
                            platform::MovieClose(smk2);
                            smk2 = platform::kInvalidMovie;
                        }
                        if (expansionChoice0 != EXPANSION_CAMPAIGN_NONE) {
                            bExpansionSmackNum =
                                static_cast<i8>(expansionChoice0 + EXPANSION_FIRST_MOVIE);
                            const std::string expansionMoviePath =
                                moviePath(SmackOptions[bExpansionSmackNum].fileName);
                            smk2 = platform::MovieOpen(expansionMoviePath.c_str(), bSmackSound);
                            platform::MovieTarget(
                                smk2,
                                gpWindowManager->m_screen->m_pixels,
                                GRAPHICS_WIDTH,
                                GRAPHICS_HEIGHT,
                                SmackOptions[bExpansionSmackNum].companionX,
                                SmackOptions[bExpansionSmackNum].companionY
                            );
                            backImage->DrawToBuffer(0, 0, 1, ICON_DRAW_NORMAL);
                        }
                    }
                }
                break;
            case MESSAGE_KEY_DOWN:
                if (message.payload.keyboard.keyCode == INPUT_SCAN_F4)
                    break;
            case MESSAGE_RIGHT_BUTTON_DOWN:
                if (bSmackNum == CHOOSE_CAMPAIGN)
                    break;
                if (bSmackNum == EXPANSION_CAMPAIGN)
                    break;
            case MESSAGE_LEFT_BUTTON_DOWN:
                if (bSmackNum == SMACK_EARTH)
                    break;
                if (bSmackNum == EXPANSION_CAMPAIGN && xLastChoice == EXPANSION_CAMPAIGN_NONE)
                    break;
                goto playbackDone;
                break;
            default:
                break;
        }

        if (bSmackNum == CONGRATS && smk1 != platform::kInvalidMovie
            && platform::MovieFrameIndex(smk1) + 1 == platform::MovieFrameCount(smk1)
            && !musicStarted0) {
            musicStarted0 = true;
            gpSoundManager->PlayAmbientMusic(LOSE_MUSIC);
        }

        if (!SmackOptions[bSmackNum].waitForInput
            && (gbLastFramePlayed
                || (smk2 != platform::kInvalidMovie
                    && (platform::MovieFrameIndex(smk2)
                            >= platform::MovieFrameCount(smk2) - 1
                        || (platform::MovieFrameIndex(smk2) <= 0 && companionStarted26)))
                || (smk2 == platform::kInvalidMovie
                    && smk1 != platform::kInvalidMovie
                    && (platform::MovieFrameIndex(smk1) >= platform::MovieFrameCount(smk1)
                        || (platform::MovieFrameIndex(smk1) <= 0 && primaryStarted7))))) {
            playing17 = false;
            gbPlayedThrough = true;
        }
    }

playbackDone:
    if (bSmackNum == CHOOSE_CAMPAIGN) {
        gpMouseManager->HideColorPointer();
        gpMouseManager->SetPointer(
            "advmice.mse",
            POINTER_DEFAULT,
            MOUSE_AUTO_CURSOR_TYPE
        );
    }

    if (SmackOptions[bSmackNum].fadeOut) {
        memcpy(gpBufferPalette->m_data, gPalette->m_data, PALETTE_DATA_SIZE);
        gpWindowManager->FadeScreen(FADE_OUT, SHORT_FADE, NULL);
        FillBitmapArea(
            gpWindowManager->m_screen,
            0,
            0,
            GRAPHICS_WIDTH,
            GRAPHICS_HEIGHT,
            BACKGROUND_COLOR
        );
        BlitBitmapToScreen(
            gpWindowManager->m_screen,
            0,
            0,
            GRAPHICS_WIDTH,
            GRAPHICS_HEIGHT,
            0,
            0
        );
    } else if (!gbPlayedThrough && bSmackNum != CONGRATS) {
        memcpy(gpBufferPalette->m_data, gPalette->m_data, PALETTE_DATA_SIZE);
        gpWindowManager->FadeScreen(FADE_OUT, NORMAL_FADE, NULL);
        FillBitmapArea(
            gpWindowManager->m_screen,
            0,
            0,
            GRAPHICS_WIDTH,
            GRAPHICS_HEIGHT,
            BACKGROUND_COLOR
        );
        BlitBitmapToScreen(
            gpWindowManager->m_screen,
            0,
            0,
            GRAPHICS_WIDTH,
            GRAPHICS_HEIGHT,
            0,
            0
        );
    }

    if (smk1 != platform::kInvalidMovie)
        platform::MovieClose(smk1);
    smk1 = platform::kInvalidMovie;
    if (smk2 != platform::kInvalidMovie)
        platform::MovieClose(smk2);
    smk2 = platform::kInvalidMovie;
    if (bSmackNum != CONGRATS) {
        memcpy(gPalette->m_data, savedPalette4, PALETTE_DATA_SIZE);
        UpdatePalette(gPalette->m_data);
    }
    gpMouseManager->ShowColorPointer();
    if (brotherIcon)
        gpResourceManager->Dispose(static_cast<resource*>(brotherIcon));
    brotherIcon = NULL;
    if (cmpnNoCD)
        gpResourceManager->Dispose(static_cast<resource*>(cmpnNoCD));
    cmpnNoCD = NULL;
    if (backImage)
        gpResourceManager->Dispose(static_cast<resource*>(backImage));
    backImage = NULL;
    gpSoundManager->RestoreBackend();
}

void ShutDownSmacker(void) {
    if (smk1 != platform::kInvalidMovie)
        platform::MovieClose(smk1);
    smk1 = platform::kInvalidMovie;
    if (smk2 != platform::kInvalidMovie)
        platform::MovieClose(smk2);
    smk2 = platform::kInvalidMovie;
}

i32 PlaySmacker(i32 smackNumber) {
    i32 oldUpdateFlags;
    i8 savedPalette[PALETTE_DATA_SIZE];

    xLastChoice = EXPANSION_CAMPAIGN_NONE;
    if (gbNoCDRom)
        return 0;

    gbInSmackMgr = true;
    gbPlayedThrough = false;
    memcpy(savedPalette, gpBufferPalette->m_data, PALETTE_DATA_SIZE);
    oldUpdateFlags = gpWindowManager->m_updateFlags;
    gpWindowManager->m_updateFlags = 0;
    if (smackNumber != EXPANSION_CAMPAIGN) {
        gSoundBackendsReady = 1;
        gpSoundManager->PlayAmbientMusic(-1);
    }
    if (gConfig.slowVideo == VIDEO_SPEED_TEST) {
        gConfig.slowVideo = 0;
        WritePrefs();
        bSmackNum = SMACK_EARTH;
        SmackManagerMain();
        // Retail timed the test movie to decide this. Decoding is no longer
        // the bottleneck it was, so only a memory shortage forces slow video.
        if (gbLowMemory) {
            gConfig.slowVideo = 1;
            WritePrefs();
        }
    }
    bSmackNum = static_cast<i8>(smackNumber);
    SmackManagerMain();
    memcpy(gpBufferPalette->m_data, savedPalette, PALETTE_DATA_SIZE);
    gpWindowManager->m_updateFlags = oldUpdateFlags;
    gbInSmackMgr = false;
    return gbPlayedThrough;
}

b32 bSmackSound = false;
icon* brotherIcon = NULL;
icon* cmpnNoCD = NULL;
static tag_rect expansionCampaignRects[EXPANSION_RECT_COUNT] =
    {{215, 49, 230, 150}, {217, 275, 230, 150}, {475, 132, 120, 180}, {41, 132, 120, 180}};

ExpansionCampaignId ExpansionCampaignRect(i32 x, i32 y) {
    ExpansionCampaignId campaign = EXPANSION_CAMPAIGN_PRICE_OF_LOYALTY;
    for (; H2EnumIndex(campaign) < EXPANSION_RECT_COUNT; ++campaign) {
        if (PointInRect(x, y, &expansionCampaignRects[H2EnumIndex(campaign)]))
            return campaign;
    }
    return EXPANSION_CAMPAIGN_NONE;
}

i8 PointInRect(i32 x, i32 y, tag_rect* rect) {
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

icon* backImage = NULL;
SSmackOptions SmackOptions[SMACK_OPTION_COUNT] = {
    {"H2XINTRO", "", "H2XINTRO", "", true, true, false, false, false, 0, 0},
    {"NWCLOGO", "", "NWCLOGO", "", true, true, false, false, false, 0, 0},
    {"WIN", "", "WIN", "", true, false, false, true, false, 0, 0},
    {"LOSE", "", "LOSE", "", true, true, true, true, false, 0, 0},
    {"INTRO", "", "INTRO", "", true, true, false, false, false, 0, 0},
    {"GOOD01", "GOOD01V", "GOOD01", "GOOD01V", true, true, true, false, true, 348, 420},
    {"GOOD02", "GOOD02W", "GOOD02", "GOOD02W", true, true, true, false, false, 0, 0},
    {"GOOD03", "GOOD03QW", "GOOD03", "GOOD03QW", true, true, true, false, false, 0, 0},
    {"GOOD03", "GOOD03QW", "GOOD03", "GOOD03QW", true, true, true, false, false, 0, 0},
    {"GOOD04", "GOOD04W", "GOOD04", "GOOD04W", true, true, true, false, false, 0, 0},
    {"GOOD05", "GOOD05V", "GOOD05", "GOOD05V", true, true, true, false, true, 87, 345},
    {"GOOD06", "GOOD06AV", "GOOD06", "GOOD6AV", true, true, true, false, true, 280, 186},
    {"GOOD06", "GOOD06BV", "GOOD06", "GOOD6BV", true, true, true, false, true, 280, 186},
    {"GOOD07", "GOOD07QW", "GOOD07", "GOOD07QW", true, true, true, false, false, 0, 0},
    {"GOOD07", "GOOD07QW", "GOOD07", "GOOD07QW", true, true, true, false, false, 0, 0},
    {"GOOD09", "GOOD09W", "GOOD09", "GOOD09W", true, true, true, false, false, 0, 0},
    {"GOOD10", "GOOD10W", "GOOD10", "GOOD10W", true, true, true, false, false, 0, 0},
    {"GOOD10", "GOOD10W", "GOOD10", "GOOD10W", true, true, true, false, false, 0, 0},
    {"LIBRARY", "LIBRARYW", "LIBRARY", "LIBRARYW", true, true, true, false, false, 0, 0},
    {"EVIL01", "EVIL01V", "EVIL01", "EVIL01V", true, true, true, false, true, 364, 404},
    {"EVIL02", "EVIL02W", "EVIL02", "EVIL02W", true, true, true, false, false, 0, 0},
    {"EVIL03", "EVIL03QW", "EVIL03", "EVIL03QW", true, true, true, false, false, 0, 0},
    {"EVIL03", "EVIL03QW", "EVIL03", "EVIL03QW", true, true, true, false, false, 0, 0},
    {"EVIL05", "EVIL05AV", "EVIL05", "EVIL5AV", true, true, true, false, true, 87, 345},
    {"EVIL05", "EVIL05BV", "EVIL05", "EVIL5BV", true, true, true, false, true, 87, 345},
    {"EVIL05", "RBETRAYV", "EVIL05", "RBETRAYV", true, true, true, false, true, 87, 345},
    {"EVIL06", "EVIL06AW", "EVIL06", "EVIL06AW", true, true, true, false, false, 0, 0},
    {"EVIL06", "EVIL06BW", "EVIL06", "EVIL06BW", true, true, true, false, false, 0, 0},
    {"EVIL07", "EVIL07W", "EVIL07", "EVIL07W", true, true, true, false, false, 0, 0},
    {"EVIL07", "EVIL07W", "EVIL07", "EVIL07W", true, true, true, false, false, 0, 0},
    {"EVIL08", "", "EVIL08", "", true, true, true, false, false, 0, 0},
    {"EVIL09", "EVIL09W", "EVIL09", "EVIL09W", true, true, true, false, false, 0, 0},
    {"EVIL09", "EVIL09W", "EVIL09", "EVIL09W", true, true, true, false, false, 0, 0},
    {"EVIL10", "EVIL11W", "EVIL10", "EVIL11W", true, true, true, false, false, 0, 0},
    {"PRISON", "", "PRISON", "", true, true, false, false, false, 0, 0},
    {"CHOOSE", "CHOOSEW", "CHOOSE", "CHOOSEW", true, true, true, true, false, 0, 0},
    {"CREDITS", "", "CREDITS", "", false, false, true, true, false, 0, 0},
    {"EARTH", "", "EARTH", "", true, true, false, false, false, 0, 0},
    {"MM6", "", "SMM6", "", true, true, false, false, false, 0, 0},
    {"POL1", "MIXPOL1", "POL1", "MIXPOL1", true, true, true, false, false, 0, 0},
    {"POL2", "MIXPOL2", "POL2", "MIXPOL2", true, true, true, false, false, 0, 0},
    {"POL3", "MIXPOL3", "POL3", "MIXPOL3", true, true, true, false, false, 0, 0},
    {"POL4", "MIXPOL4", "POL4", "MIXPOL4", true, true, true, false, false, 0, 0},
    {"POL5", "MIXPOL5", "POL5", "MIXPOL5", true, true, true, false, false, 0, 0},
    {"POL6", "MIXPOL6", "POL6", "MIXPOL6", true, true, true, false, false, 0, 0},
    {"POL7", "MIXPOL7", "POL7", "MIXPOL7", true, true, true, false, false, 0, 0},
    {"POL8", "MIXPOL8", "POL8", "MIXPOL8", true, true, true, false, false, 0, 0},
    {"DES9", "MIXDES9", "DES9", "MIXDES9", true, true, true, false, false, 0, 0},
    {"DES10", "MIXDES10", "DES10", "MIXDES10", true, true, true, false, false, 0, 0},
    {"DES11", "MIXDES11", "DES11", "MIXDES11", true, true, true, false, false, 0, 0},
    {"DES12", "MIXDES12", "DES12", "MIXDES12", true, true, true, false, false, 0, 0},
    {"DES13", "MIXDES13", "DES13", "MIXDES13", true, true, true, false, false, 0, 0},
    {"DES14", "MIXDES14", "DES14", "MIXDES14", true, true, true, false, false, 0, 0},
    {"DES15", "MIXDES15", "DES15", "MIXDES15", true, true, true, false, false, 0, 0},
    {"WIZ16", "MIXWIZ16", "WIZ16", "MIXWIZ16", true, true, true, false, false, 0, 0},
    {"WIZ17", "MIXWIZ17", "WIZ17", "MIXWIZ17", true, true, true, false, false, 0, 0},
    {"WIZ18", "MIXWIZ18", "WIZ18", "MIXWIZ18", true, true, true, false, false, 0, 0},
    {"WIZ19", "MIXWIZ19", "WIZ19", "MIXWIZ19", true, true, true, false, false, 0, 0},
    {"WIZ20", "MIXWIZ20", "WIZ20", "MIXWIZ20", true, true, true, false, false, 0, 0},
    {"VOY21", "MIXVOY21", "VOY21", "MIXVOY21", true, true, true, false, false, 0, 0},
    {"VOY22", "MIXVOY22", "VOY22", "MIXVOY22", true, true, true, false, false, 0, 0},
    {"VOY23", "MIXVOY23", "VOY23", "MIXVOY23", true, true, true, false, false, 0, 0},
    {"VOY24", "MIXVOY24", "VOY24", "MIXVOY24", true, true, true, false, false, 0, 0},
    {"VOY25", "MIXVOY25", "VOY25", "MIXVOY25", true, true, true, false, false, 0, 0},
    {"DEFEAT", "", "DEFEAT", "", true, true, true, true, false, 0, 0},
    {"H2XINTRO", "", "H2XINTR", "", true, true, false, false, false, 0, 0},
    {"CYLOGO", "", "CYLOGO", "", true, true, true, false, false, 0, 0},
    {"", "", "", "", true, true, true, true, false, 0, 0},
    {"IVYPOL", "", "IVYPOL", "", false, false, true, true, true, 214, 47},
    {"IVYDES", "", "IVYDES", "", false, false, true, true, true, 214, 273},
    {"IVYWIZ", "", "IVYWIZ", "", false, false, true, true, true, 472, 132},
    {"IVYVOY", "", "IVYVOY", "", false, false, true, true, true, 41, 132},
    {"CYBCREDS", "", "CYBCREDS", "", true, false, true, true, false, 0, 0},
    {"BUKA", "", "BUKA", "", true, true, false, false, false, 0, 0},
    {"BUKACRED", "", "BUKACRED", "", true, false, true, false, false, 0, 0}
};
platform::MovieId smk1 = platform::kInvalidMovie;
platform::MovieId smk2 = platform::kInvalidMovie;
i8 bSmackNum;
b32 gbLastFramePlayed;
b32 gbPlayedThrough;
b8 bMainDone;
