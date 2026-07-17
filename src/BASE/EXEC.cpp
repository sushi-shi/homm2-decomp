// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_RE\EXEC.OBJ   from: .\basewin.lib
// functions: 10   data: 0
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include <BASE/executive.h>
#include <BASE/EXEC_TYPES.h>
#include <BASE/baseManager.h>
#include <BASE/Misc.h>
#include <stdio.h>
#include <BASE/resourceManager.h>
#include <BASE/inputManager.h>
#include <BASE/soundManager.h>
#include <BASE/heroWindowManager.h>
#include <BASE/mouseManager.h>
#include <SOURCE/KB.h>
#include <SOURCE/kbwin.h>
#include <SOURCE/X_GLOBAL.h>

// @data-layout-note Retail's EXEC contribution is one initialized-data interval,
// RVA 0x11fb20..0x11fd34 (0x214), with no rdata or loader-zero contribution.
// The former source produced 12 pooled literal COMDATs: raw size 0x156 and a
// replay end at offset 0x163. It collapsed six "Can't add manager!" uses and two
// "-----" uses, and five initialization/list payloads differed from the PE.
// Retail HIGHLOW sites prove 18 distinct addresses in call order at owner addends
// 0x000, 0x038, 0x088, 0x0a4, 0x0c0, 0x100, 0x114, 0x128, 0x13c, 0x150,
// 0x170, 0x178, 0x18c, 0x194, 0x1c0, 0x1e0, 0x1f4, and 0x208. Separate static
// arrays were tested and rejected because VC4.2 aligned each owner to eight bytes,
// growing candidate data to 0x23b. This structured owner compiles to one 0x214
// section whose bytes equal the retail interval and whose 18 relocation addends
// equal those PE addresses. Do not reintroduce pooled literals, independent arrays,
// padding symbols, aliases, supplemental identities, or section pragmas.
DATA(0x0051fb20) static SExecutiveText gExecutiveText = {
    "Unable to initialize resources - possible disk problem.",
    "Unable to initialize input devices - possible problem with mouse or keyboard.",
    "Unable to initialize sound.",
    "Unable to initialize mouse.",
    "Unable to initialize windows - possible memory or disk error.",
    "Can't add manager!",
    "Can't add manager!",
    "Can't add manager!",
    "Can't add manager!",
    "-----Manager List Start-----",
    "-----",
    "Head %d   Tail %d",
    "-----",
    "Manager %20s  this %d   prev %d  next %d",
    "--*--Manager List Stop --*--\n\n",
    "Can't add manager!",
    "Can't add manager!",
    "Terminated"
};

VA(0x004d1610, 0x10)
executive::executive(void) {
    m_managerListHead = 0;
    m_managerListTail = 0;
    m_activeManager = 0;
    m_result = 0;
}

VA(0x004d1620, 0x9e)
i32 executive::InitSystem(void) {
    if (gpResourceManager->Open(EXECUTIVE_MANAGER_DEFAULT_PRIORITY) != 0)
        ShutDown(gExecutiveText.resourceInitError);
    if (gpInputManager->Open(EXECUTIVE_MANAGER_DEFAULT_PRIORITY) != 0)
        ShutDown(gExecutiveText.inputInitError);
    if (giCurExe == 1) {
        if (gpSoundManager->Open(EXECUTIVE_MANAGER_DEFAULT_PRIORITY) != 0)
            ShutDown(gExecutiveText.soundInitError);
    }
    if (AddManager(gpMouseManager, EXECUTIVE_MANAGER_DEFAULT_PRIORITY) != 0)
        ShutDown(gExecutiveText.mouseInitError);
    if (AddManager(gpWindowManager, EXECUTIVE_MANAGER_DEFAULT_PRIORITY) != 0)
        ShutDown(gExecutiveText.windowInitError);
    return 0;
}

// Complete /O2 structure with an exact frame/CFG and 11/11 ordered external
// relocations. The only canonical byte residual is the commutative comparison at
// +0x2d (ours cmp ECX,EAX; retail cmp EAX,ECX). Reordering the condition, naming
// the window-manager operand, and 20 AST permutations did not steer it. A bounded
// tu_state_noise seed-45 enum probe produced an audited exact closure without
// changing exact siblings; revisit only if earlier EXEC TU state changes.
VA(0x004d16c0, 0x86)
void executive::ShutDownSystem(void) {
    EarlyShutDownSystem();
    gpSoundManager->Close();
    baseManager* next = m_managerListHead;
    baseManager* cur;
    while ((cur = next) != 0) {
        next = cur->m_next;
        if (cur != gpWindowManager && cur != gpMouseManager)
            RemoveManager(cur);
    }
    if (gpWindowManager->m_active == 1)
        RemoveManager(gpWindowManager);
    if (gpMouseManager->m_active == 1)
        RemoveManager(gpMouseManager);
    gpInputManager->Close();
    gpResourceManager->Close();
}

// @semantic
// Complete /O2 structure with the retail frame/CFG and all 16 ordered external
// relocations. The snapshot loop at +0x19..+0x40 is byte-exact. The only structural
// residual is +0xce..+0xe9 in the manager restoration loop: ours assigns EDX=manager
// and ECX=index while retail assigns ECX=manager and EDX=index. Declaration/init
// reordering and 25 AST permutations did not steer it; two bounded TU-state probes
// (78 trials total) reached only a disposable 99.4375%. Revisit after earlier EXEC
// TU-state changes.
VA(0x004d1750, 0xfb)
i32 executive::DoDialog(class baseManager* manager) {
    baseManager* managerList[EXECUTIVE_DIALOG_MANAGER_CAPACITY];
    baseManager* previousList[EXECUTIVE_DIALOG_MANAGER_CAPACITY];
    baseManager* nextList[EXECUTIVE_DIALOG_MANAGER_CAPACITY];
    i32 dialogStorage[4];
    executive* dialog = reinterpret_cast<executive*>(dialogStorage);
    baseManager* listManager;
    i32 managerIndex;
    i32 managerCount;
    managerCount = 0;
    dialog->m_managerListHead = 0;
    dialog->m_managerListTail = 0;
    dialog->m_activeManager = 0;
    listManager = m_managerListHead;
    dialog->m_result = 0;
    managerIndex = 0;
    if (listManager != 0) {
        do {
            managerList[managerIndex] = listManager;
            previousList[managerIndex] = listManager->m_prev;
            listManager = listManager->m_next;
            nextList[managerIndex] = listManager;
            managerIndex++;
            managerCount++;
        } while (listManager != 0);
    }
    if (AddManager(manager, EXECUTIVE_MANAGER_DEFAULT_PRIORITY) != 0)
        ShutDown(gExecutiveText.dialogManagerError1);
    if (dialog->AddManager(gpMouseManager, EXECUTIVE_MANAGER_DEFAULT_PRIORITY) != 0)
        ShutDown(gExecutiveText.dialogManagerError2);
    if (dialog->AddManager(gpWindowManager, EXECUTIVE_MANAGER_DEFAULT_PRIORITY) != 0)
        ShutDown(gExecutiveText.dialogManagerError3);
    if (dialog->AddManager(manager, EXECUTIVE_MANAGER_DEFAULT_PRIORITY) != 0)
        ShutDown(gExecutiveText.dialogManagerError4);
    dialog->MainLoop();
    RemoveManager(manager);
    if (managerCount > 0) {
        managerIndex = 0;
        do {
            listManager = managerList[managerIndex];
            listManager->m_prev = previousList[managerIndex];
            listManager->m_next = nextList[managerIndex];
            managerIndex++;
            managerCount--;
        } while (managerCount != 0);
    }
    return dialog->m_result;
}

VA(0x004d1850, 0x86)
void executive::PrintManagerList(void) {
    LogStr(gExecutiveText.managerListStart);
    LogStr(gExecutiveText.managerListDivider1);
    sprintf(gText, gExecutiveText.managerListHeaderFormat, m_managerListHead, m_managerListTail);
    LogStr(gText);
    LogStr(gExecutiveText.managerListDivider2);
    for (baseManager* m = m_managerListHead; m != 0; m = m->m_next) {
        sprintf(gText, gExecutiveText.managerListEntryFormat, m->m_name, m, m->m_prev, m->m_next);
        LogStr(gText);
    }
    LogStr(gExecutiveText.managerListStop);
}

VA(0x004d18e0, 0xce)
i32 executive::AddManager(class baseManager* mgr, i32 priority) {
    if (mgr == 0)
        return EXECUTIVE_MANAGER_ERROR;
    if (priority == EXECUTIVE_MANAGER_DEFAULT_PRIORITY) {
        priority = m_managerListTail == 0 ? 0 : m_managerListTail->m_priority + 1;
    }
    if (mgr->m_active == 0 && mgr->Open(priority) != 0)
        return EXECUTIVE_MANAGER_ERROR;
    baseManager* tail = m_managerListTail;
    baseManager* cur = m_managerListTail;
    if (cur != 0) {
        do {
            if (cur->m_priority <= priority)
                break;
            cur = cur->m_prev;
        } while (cur != 0);
    }
    if (cur == 0) {
        mgr->m_next = m_managerListHead;
        mgr->m_prev = 0;
        if (m_managerListHead != 0)
            m_managerListHead->m_prev = mgr;
        m_managerListHead = mgr;
        if (m_managerListTail == 0)
            m_managerListTail = mgr;
    } else if (cur->m_next == 0) {
        mgr->m_next = 0;
        mgr->m_prev = tail;
        m_managerListTail->m_next = mgr;
        m_managerListTail = mgr;
    } else {
        mgr->m_prev = cur;
        mgr->m_next = cur->m_next;
        cur->m_next->m_prev = mgr;
        cur->m_next = mgr;
    }
    return EXECUTIVE_MANAGER_SUCCESS;
}

// Complete /O2 structure and CFG with 0/0 relocations. The only canonical byte
// residual is +0x15..+0x1b: ours loads head then tail, while retail loads tail then
// head before the same equality test. A shared next lifetime, an explicit tail
// local, swapped condition spelling, and 25 AST permutations were tried. A bounded
// tu_state_noise seed-45 enum probe produced an audited exact closure without
// changing exact siblings; revisit only if earlier EXEC TU state changes.
VA(0x004d19b0, 0x76)
void executive::RemoveManager(class baseManager* mgr) {
    if (mgr != 0) {
        mgr->Close();
        baseManager* next;
        baseManager* prev = mgr->m_prev;
        if (prev == 0) {
            baseManager* tail = m_managerListTail;
            if (m_managerListHead == tail) {
                m_managerListTail = 0;
                m_managerListHead = 0;
            } else {
                next = mgr->m_next;
                m_managerListHead = next;
                next->m_prev = 0;
            }
            mgr->m_prev = 0;
            mgr->m_next = 0;
            return;
        }
        next = mgr->m_next;
        prev->m_next = next;
        if (next == 0)
            m_managerListTail = prev;
        else
            next->m_prev = prev;
        mgr->m_prev = 0;
        mgr->m_next = 0;
    }
}

VA(0x004d1a30, 0x5a)
void executive::CallManager(class baseManager* mgr) {
    baseManager* saved = m_activeManager;
    RemoveManager(saved);
    if (AddManager(mgr, EXECUTIVE_MANAGER_DEFAULT_PRIORITY) != 0)
        ShutDown(gExecutiveText.callManagerError1);
    MainLoop();
    RemoveManager(mgr);
    if (AddManager(saved, EXECUTIVE_MANAGER_DEFAULT_PRIORITY) != 0)
        ShutDown(gExecutiveText.callManagerError2);
    m_activeManager = saved;
}

VA(0x004d1a90, 0xfa)
void executive::MainLoop(void) {
    tag_message message;
    i32 keepDispatching;
    i32 done = 0;
    i32 result;
    baseManager* manager;
    if (m_managerListHead != 0) {
        gpInputManager->Flush();
        do {
            Process1WindowsMessage();
            message = gpInputManager->GetEvent();
            keepDispatching = 1;
            m_activeManager = m_managerListHead;
            if (m_managerListHead == 0)
                return;
            do {
                if (!keepDispatching)
                    break;
                if (done)
                    return;
                manager = m_activeManager;
                if (manager->m_active == 1
                    && (message.type != MESSAGE_MOUSE_MOVE || gpWindowManager != manager)) {
                    result = manager->Main(message);
                    switch (result) {
                        case EXECUTIVE_MANAGER_STOP_DISPATCH:
                            keepDispatching = 0;
                            break;
                        case EXECUTIVE_MANAGER_HANDLE_EXECUTIVE_MESSAGE:
                            if ((message.type & MESSAGE_EXECUTIVE) == 0)
                                break;
                            switch (message.payload.executive.command) {
                                case EXECUTIVE_COMMAND_TERMINATE_LOOP:
                                    done++;
                                    break;
                                case EXECUTIVE_COMMAND_REMOVE_MANAGER:
                                    RemoveManager(m_activeManager);
                                    m_activeManager = 0;
                                    break;
                                case EXECUTIVE_COMMAND_RETURN_RESULT:
                                    m_result = message.payload.executive.result;
                                    done++;
                                    break;
                            }
                            break;
                    }
                }
                if (m_activeManager != 0)
                    m_activeManager = m_activeManager->m_next;
            } while (m_activeManager != 0);
        } while (!done);
    }
}

VA(0x004d1b90, 0xa)
void executive::Terminate(void) {
    ShutDown(gExecutiveText.terminationMessage);
}
