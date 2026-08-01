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
    MOVIE_PATH_SIZE                = 352,
    MILES_SOUND_SYSTEM_PREFERENCE  = 15,
    CAMPAIGN_BLIT_X                = 49,
    CAMPAIGN_BLIT_Y                = 78,
    CAMPAIGN_BLIT_WIDTH            = 538,
    CAMPAIGN_BLIT_HEIGHT           = 258,
    VIDEO_SPEED_TEST               = 3,
    VIDEO_OPEN_READ_SLOW_THRESHOLD = 2000,
    VIDEO_DECOMP_SLOW_THRESHOLD    = 1300,
    SMACK_OPTION_COUNT             = 73
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
    i32 playing16;
    i32 unusedPlaybackState4;
    i32 musicStarted36;
    i32 companionStarted1;
    i32 primaryStarted9;
    i32 unusedOne1 = 1;
    char path7[MOVIE_PATH_SIZE];
    i8 savedPalette9[PALETTE_DATA_SIZE];
    i32 unusedFrameHead36;

    gbLastFramePlayed = false;
    musicStarted36 = 0;
    if (bSmackNum == CHOOSE_CAMPAIGN) {
        i32 initialMouseY29;
        i32 initialMouseX3;
        i32 unusedInitialMouseState11;

        brotherIcon = gpResourceManager->GetIcon("brothers.icn");
        gpMouseManager->MouseCoords(initialMouseX3, initialMouseY29);
        if (initialMouseX3 < CAMPAIGN_DIVIDER_X)
            gbCampaignSideChoice = CAMPAIGN_ARCHIBALD;
        else
            gbCampaignSideChoice = CAMPAIGN_ROLAND;
    }

    platform::ChangeMenu(hmnuDflt);
    gpMouseManager->HideColorPointer();
    bMainDone = 1;
    memcpy(savedPalette9, gPalette->m_data, PALETTE_DATA_SIZE);

    // Retail picked a sound driver for the movie player here. There is one.
    bSmackSound = !(gbNoSound || !gpSoundManager->m_digitalReady
                    || gConfig.soundVolume == CONFIG_VOLUME_MUTED
                    || bSmackNum == SMACK_CREDITS);

    if (xSmackFromNetwork && bSmackNum > FIRST_NETWORK - 1)
        strcpy(path7, "i:\\projects\\heroes\\art\\fin3d\\");
    else if (bSmackNum == EXPANSION_CAMPAIGN)
        strcpy(path7, ".\\DATA\\");
    else
        strcpy(path7, gcAnimPath);

    if (gConfig.slowVideo)
        sprintf(gText, "%s%s.SMK", path7, SmackOptions[bSmackNum].slowFileName);
    else
        sprintf(gText, "%s%s.SMK", path7, SmackOptions[bSmackNum].fileName);

    smk1 = platform::kInvalidMovie;
    if (bSmackNum != EXPANSION_CAMPAIGN) {
        smk1 = platform::MovieOpen(gText, bSmackSound);
        if (smk1 == platform::kInvalidMovie)
            ShutDown("Unable to open animation file.");
        platform::MovieTarget(
            smk1,
            gpWindowManager->m_screen->m_pixels,
            GRAPHICS_WIDTH,
            GRAPHICS_HEIGHT,
            0,
            0
        );
    }

    if (strlen(SmackOptions[bSmackNum].companionFileName) > 1) {
        if (gConfig.slowVideo)
            sprintf(gText, "%s%s.SMK", path7, SmackOptions[bSmackNum].slowCompanionFileName);
        else
            sprintf(gText, "%s%s.SMK", path7, SmackOptions[bSmackNum].companionFileName);
        smk2 = platform::MovieOpen(gText, bSmackSound);
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

    playing16 = 1;
    primaryStarted9 = 0;
    companionStarted1 = 0;

    if (bSmackNum == CHOOSE_CAMPAIGN) {
        i32 eventSink;

        platform::PumpEvents();
        while (gpInputManager->GetEvent().type != MESSAGE_NONE) {
            eventSink = 0;
        }
    }

    while (playing16) {
        if (bSmackNum == EXPANSION_CAMPAIGN) {
            if (!primaryStarted9) {
                gpMouseManager->SetPointer("advmice.mse", POINTER_ID, MOUSE_AUTO_CURSOR_TYPE);
                gpMouseManager->ReallyShowPointer();
                gpSoundManager->PlayAmbientMusic(MAIN_MUSIC, 0, -1);
                backImage = gpResourceManager->GetIcon("x_ivy.icn");
                if (!backImage)
                    MemError();
                backImage->DrawToBuffer(0, 0, 0, ICON_DRAW_NORMAL);
                backImage->DrawToBuffer(0, 0, 1, ICON_DRAW_NORMAL);
                sprintf(gText, "%s%s.SMK", path7, "IVYPOL");
                smk2 = platform::MovieOpen(gText, false);
                if (const u8* moviePalette = platform::MoviePalette(smk2))
                    memcpy(gPalette->m_data, moviePalette, PALETTE_DATA_SIZE);
                platform::MovieClose(smk2);
                smk2 = platform::kInvalidMovie;
                ConvertSmackerPalette(reinterpret_cast<u8*>(gPalette->m_data));
                UpdatePalette(gPalette->m_data);
                memcpy(gpBufferPalette->m_data, gPalette->m_data, PALETTE_DATA_SIZE);
                gpWindowManager->FadeScreen(FADE_IN, FAST_FADE, NULL);
                primaryStarted9 = 1;
            }
        } else if (!platform::MovieWaiting(smk1)) {
            if (bSmackNum == INTRO_MUSIC && !musicStarted36) {
                musicStarted36 = 1;
                gpSoundManager->PlayAmbientMusic(INTRO_SECOND_MUSIC, 0, -1);
            }
            if ((!primaryStarted9 || platform::MovieFrameCount(smk1) > 1)
                && (bSmackNum != CONGRATS || platform::MovieFrameCount(smk1) - 1 != platform::MovieFrameIndex(smk1))) {
                DoAdvance(smk1, 1, 1, primaryStarted9 || !SmackOptions[bSmackNum].fadeIn, 0);
            }
            if (platform::MovieFrameIndex(smk1) > 0 || platform::MovieFrameCount(smk1) <= 1) {
                if (!primaryStarted9) {
                    if (bSmackNum == CHOOSE_CAMPAIGN) {
                        gpMouseManager
                            ->SetPointer("advmice.mse", POINTER_ID, MOUSE_AUTO_CURSOR_TYPE);
                        gpMouseManager->ReallyShowPointer();
                    }
                    if (SmackOptions[bSmackNum].fadeIn) {
                        memcpy(gpBufferPalette->m_data, gPalette->m_data, PALETTE_DATA_SIZE);
                        gpWindowManager->FadeScreen(FADE_IN, FAST_FADE, NULL);
                    }
                    if (bSmackNum == SMACK_CREDITS)
                        gpSoundManager->PlayAmbientMusic(MAIN_MUSIC, 0, -1);
                    if (bSmackNum == SPECIAL_MUSIC)
                        gpSoundManager->PlayAmbientMusic(MAIN_MUSIC, 0, -1);
                }
                primaryStarted9 = 1;
            }
        }

        if (smk2 != platform::kInvalidMovie && primaryStarted9 && !platform::MovieWaiting(smk2)) {
            if (companionStarted1 && platform::MovieFrameCount(smk2) - 1 == platform::MovieFrameIndex(smk2)) {
                i32 drawLastFrame;
                i32 advanceLastFrame;

                advanceLastFrame = 0;
                if (SmackOptions[bSmackNum].drawCompanion && !gConfig.slowVideo) {
                    drawLastFrame = 1;
                } else if (bSmackNum == EXPANSION_CAMPAIGN) {
                    drawLastFrame = 1;
                    advanceLastFrame = 1;
                } else {
                    drawLastFrame = 0;
                }
                DoAdvance(smk2, drawLastFrame, advanceLastFrame, 0, 1);
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
                companionStarted1 = 1;
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
                    ExpansionCampaignId expansionChoice1;

                    gpMouseManager->MouseCoords(expansionMouseX6, expansionMouseY4);
                    expansionChoice1 = ExpansionCampaignRect(expansionMouseX6, expansionMouseY4);
                    if (expansionChoice1 != xLastChoice) {
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
                        xLastChoice = expansionChoice1;
                        if (smk2 != platform::kInvalidMovie) {
                            platform::MovieClose(smk2);
                            smk2 = platform::kInvalidMovie;
                        }
                        if (expansionChoice1 != EXPANSION_CAMPAIGN_NONE) {
                            bExpansionSmackNum =
                                static_cast<i8>(expansionChoice1 + EXPANSION_FIRST_MOVIE);
                            sprintf(
                                gText,
                                "%s%s.SMK",
                                path7,
                                SmackOptions[bExpansionSmackNum].fileName
                            );
                            smk2 = platform::MovieOpen(gText, bSmackSound);
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

        if (bSmackNum == CONGRATS && platform::MovieFrameIndex(smk1) + 1 == platform::MovieFrameCount(smk1) && !musicStarted36) {
            musicStarted36 = 1;
            gpSoundManager->PlayAmbientMusic(LOSE_MUSIC, 0, -1);
        }

        if (!SmackOptions[bSmackNum].waitForInput
            && (gbLastFramePlayed
                || (smk2
                    && (platform::MovieFrameCount(smk2) - 1 <= platform::MovieFrameIndex(smk2)
                        || (platform::MovieFrameIndex(smk2) <= 0 && companionStarted1)))
                || (!smk2
                    && (platform::MovieFrameCount(smk1) <= platform::MovieFrameIndex(smk1)
                        || (platform::MovieFrameIndex(smk1) <= 0 && primaryStarted9))))) {
            playing16 = 0;
            gbPlayedThrough = true;
        }
    }

playbackDone:
    if (bSmackNum == CHOOSE_CAMPAIGN) {
        gpMouseManager->HideColorPointer();
        gpMouseManager->SetPointer("advmice.mse", POINTER_DEFAULT, MOUSE_AUTO_CURSOR_TYPE);
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
        memcpy(gPalette->m_data, savedPalette9, PALETTE_DATA_SIZE);
        UpdatePalette(gPalette->m_data);
    }
    gpMouseManager->ShowColorPointer();
    if (brotherIcon)
        gpResourceManager->Dispose(static_cast<resource*>(brotherIcon));
    brotherIcon = NULL;
    if (backImage)
        gpResourceManager->Dispose(static_cast<resource*>(backImage));
    backImage = NULL;
}

void ShutDownSmacker(void) {
    if (smk1 != platform::kInvalidMovie)
        platform::MovieClose(smk1);
    smk1 = NULL;
    if (smk2 != platform::kInvalidMovie)
        platform::MovieClose(smk2);
    smk2 = platform::kInvalidMovie;
}

i32 PlaySmacker(i32 smackNumber) {
    i8 savedPalette[PALETTE_DATA_SIZE];
    i32 oldUpdateFlags;

    xLastChoice = EXPANSION_CAMPAIGN_NONE;
    gbInSmackMgr = true;
    gbPlayedThrough = false;
    memcpy(savedPalette, gpBufferPalette->m_data, PALETTE_DATA_SIZE);
    oldUpdateFlags = gpWindowManager->m_updateFlags;
    gpWindowManager->m_updateFlags = 0;
    if (smackNumber != EXPANSION_CAMPAIGN) {
        gpSoundManager->m_samplesReady = 1;
        gpSoundManager->PlayAmbientMusic(-1, 0, -1);
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

i32 bSmackSound = 0;
icon* brotherIcon = NULL;
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
platform::MovieId smk1 = platform::kInvalidMovie;
platform::MovieId smk2 = platform::kInvalidMovie;
i8 bSmackNum;
b32 gbLastFramePlayed;
b32 gbPlayedThrough;
i8 bMainDone;
