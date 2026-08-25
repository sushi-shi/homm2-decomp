#ifndef HOMM2_IRONFIST_DIALOG_H
#define HOMM2_IRONFIST_DIALOG_H

#include <string>

#include <Ints.h>

// Ironfist scripting speaks these names; the values are the retail
// NormalDialog argument and resource-image codes.
enum IronfistDialogType {
    DIALOG_OR    = 1,
    DIALOG_EMPTY = 0
};

enum IronfistNormalDialogType {
    DIALOG_OKAY         = 1,
    DIALOG_YES_NO       = 2,
    DIALOG_CANCEL       = 3,
    DIALOG_RIGHT_CLICK  = 4,
    DIALOG_OKAY_ALT     = 5,
    DIALOG_CANCEL_ALT   = 6,
    DIALOG_LEARN_CHOICE = 7
};

enum IronfistMessageBoxImage {
    IMAGE_EMPTY                  = -1,
    IMAGE_WOOD                   = 0,
    IMAGE_MERCURY                = 1,
    IMAGE_ORE                    = 2,
    IMAGE_SULFUR                 = 3,
    IMAGE_CRYSTALS               = 4,
    IMAGE_GEMS                   = 5,
    IMAGE_GOLD                   = 6,
    IMAGE_GROUP_ARTIFACTS        = 7,
    IMAGE_GROUP_SPELLS           = 8,
    IMAGE_GROUP_PLAYERS          = 9,
    IMAGE_LUCK                   = 10,
    IMAGE_BADLUCK                = 11,
    IMAGE_GOOD_MORALE            = 12,
    IMAGE_BAD_MORALE             = 13,
    IMAGE_EXP                    = 14,
    IMAGE_GROUP_HERO             = 15,
    IMAGE_GROUP_SECONDARY_SKILLS = 17,
    IMAGE_GROUP_UNIT             = 18,
    IMAGE_GROUP_PRIMARY_SKILLS   = 25
};

void H2MessageBox(char* msg);
void H2MessageBox(std::string& msg);
bool H2QuestionBox(char* msg);
char* H2InputBox(char* msg, i32 len);

void DisplayError(const char* msg, const char* title);
void DisplayError(std::string msg, std::string title);

#endif
