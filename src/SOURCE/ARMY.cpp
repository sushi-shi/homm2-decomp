// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_Re\ARMY.OBJ   from: (directly linked into exe)
// functions: 45   data: 2
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
VA(0x0044a8c0, 0xcf)
// void army::constructor(void);

VA(0x0044a98f, 0x18)
// void army::WaitSample(int);

VA(0x0044a9a7, 0xde)
// void army::InitClean(void);

VA(0x0044aa85, 0x443)
// void army::Init(int, int, int, int, int, int);

VA(0x0044aec8, 0x4a6)
// void army::LoadResources(void);

VA(0x0044b36e, 0xe8)
// void army::FreeResources(void);

VA(0x0044b456, 0x7d2)
// void army::DrawToBuffer(int, int, int);

VA(0x0044bc28, 0x2d)
// void army::Wince(void);

VA(0x0044bc55, 0xb90)
// void army::Walk(int, int, int);

VA(0x0044c7e5, 0x14aa)
// void army::SpecialAttack(void);

VA(0x0044dc8f, 0x2b)
// void army::DirDoAttack(int);

VA(0x0044dcba, 0x4e7)
// void army::DoHydraAttack(int);

VA(0x0044e1a1, 0x1267)
// void army::DoAttack(int);

VA(0x0044f408, 0x16)
// void army::ResetPath(void);

VA(0x0044f41e, 0x25)
// int army::WalkTo(void);

VA(0x0044f443, 0x2ee)
// int army::WalkTo(int);

VA(0x0044f731, 0x25)
// int army::AttackTo(void);

VA(0x0044f756, 0x1e8)
// int army::AttackTo(int);

VA(0x0044f93e, 0x282)
// void army::CheckLuck(void);

VA(0x0044fbc0, 0x56e)
// void army::DamageEnemy(class army *, int *, int *, int, int);

VA(0x0045012e, 0x23c)
// int army::Damage(long int, int);

VA(0x0045036a, 0x1361)
// void army::PowEffect(int, int, int, int);

VA(0x004516cb, 0x35)
// unsigned long int army::Strength(void);

VA(0x00451700, 0x66)
// int army::LeaveNoBody(void);

VA(0x00451766, 0x3f5)
// void army::ProcessDeath(int);

VA(0x00451b5b, 0x39d)
// void army::SpellEffect(int, int, int);

VA(0x00451ef8, 0x10f)
// void army::CancelSpellType(int);

VA(0x00452007, 0x178)
// void army::CancelIndividualSpell(int);

VA(0x0045217f, 0x282)
// int army::SetSpellInfluence(int, int);

VA(0x00452401, 0x94)
// void army::DecrementSpellRounds(void);

VA(0x00452495, 0x644)
// void army::GoBerserk(void);

VA(0x00452ad9, 0x3f1)
// void army::MoveAttack(int, int);

VA(0x00452eca, 0x931)
// float army::SpellCastWorkChance(int);

VA(0x004537fb, 0x56)
// int army::SpellCastWorks(int);

VA(0x00453851, 0x39e)
// void BuildTempWalkSeq(struct SMonFrameInfo *, int, int);

VA(0x00453bef, 0x66)
// void army::DispelGood(void);

VA(0x00453c55, 0x90)
// void army::Cure(int);

VA(0x00453ce5, 0x79)
// int army::MidX(void);

VA(0x00453d5e, 0x59)
// int army::MidY(void);

VA(0x00453db7, 0x57)
// int army::TopY(void);

VA(0x00453e0e, 0xcc)
// int army::RightX(void);

VA(0x00453eda, 0xcc)
// int army::LeftX(void);

VA(0x00453fa6, 0x171)
// int army::OtherArmyAdjacent(int, int);

VA(0x00454117, 0x1e1)
// void ModifyFrameInfo(struct SMonFrameInfo *, int);

VA(0x004542f8, 0xbe)
// int army::GetPowBaseY(void);

// ---- data / globals / vtables ----
DATA(0x004f54a8)  // int bSecondAttack
DATA(0x00527eb4)  // int gbGenieHalf
