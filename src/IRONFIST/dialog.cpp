#include <IRONFIST/dialog.h>

#include <BASE/heroWindowManager.h>
#include <BASE/Misc.h>
#include <SOURCE/KB.h>

#include <PLATFORM/Platform.h>

void H2MessageBox(char* msg) {
    if (msg) {
        NormalDialog(msg, NORMAL_DIALOG_INFO, -1, -1, -1, 0, -1, 0, -1, 0);
    }
}

void H2MessageBox(std::string& msg) {
    if (!msg.empty()) {
        H2MessageBox(&msg[0]);
    }
}

bool H2QuestionBox(char* msg) {
    NormalDialog(msg, NORMAL_DIALOG_CONFIRM, -1, -1, -1, 0, -1, 0, -1, 0);
    return gpWindowManager->m_dialogResult != NORMAL_DIALOG_BUTTON_SIX;
}

char* H2InputBox(char* msg, i32 len) {
    char* res = static_cast<char*>(H2_ALLOC(len + 1));
    GetDataEntry(msg, res, len + 1, 0, 0, 1);
    return res;
}

void DisplayError(const char* msg, const char* title) {
    platform::Host().ShowMessage(title, msg);
}

void DisplayError(std::string msg, std::string title) {
    DisplayError(msg.c_str(), title.c_str());
}
