// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_RE\DIMMER.OBJ   from: .\basewin.lib
// functions: 7   data: 1
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include <BASE/dimmerWidget.h>
#include <BASE/resourceManager.h>
#include <SOURCE/KB.h>
VA(0x004dd330, 0x1e)
dimmerWidget::dimmerWidget(void) : widget(0, 0, 0, 0, 0, 0) {}

VA(0x004dd350, 0x34)
dimmerWidget::dimmerWidget(i16 a, i16 b, i16 c, i16 d, i16 e, i16 f) : widget(a, b, c, d, e, f) {}

VA(0x004dd390, 0x5f)
void dimmerWidget::Read(void) {
    m_x = gpResourceManager->ReadWord();
    m_y = gpResourceManager->ReadWord();
    m_width = gpResourceManager->ReadWord();
    m_height = gpResourceManager->ReadWord();
    m_id = gpResourceManager->ReadWord();
    m_kind = gpResourceManager->ReadWord();
}

VA(0x004dd3f0, 0xd)
i32 dimmerWidget::Main(struct tag_message& msg) {
    return widget::Main(msg);
}

VA(0x004dd400, 0x5)
void dimmerWidget::Draw(void) {
    Dim();
}

// ===== vtable dimmerWidget : public widget  (3 slots) =====
//  [ 0] VA(0x004dd400, 0x5)  void dimmerWidget::Draw(void)   <- override (implements widget pure virtual)
//  [ 1] VA(0x004dd410, 0x25)  void * dimmerWidget::scalar_dtor(unsigned int)   <- override (implements widget pure virtual)
//  [ 2] VA(0x004dd3f0, 0xd)  int dimmerWidget::Main(struct tag_message &)   <- override (implements widget pure virtual)

// ---- vtables (compiler-emitted; census) ----
VTBL(dimmerWidget, 0x004ebae0);
