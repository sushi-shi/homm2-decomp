// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_Re\ARMYGRP.OBJ   from: (directly linked into exe)
// functions: 13   data: 0
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include <SOURCE/armyGroup.h>
VA(0x0048c040, 0x3c)
armyGroup::armyGroup(void) {}

VA(0x0048c07c, 0x18)
void armyGroup::View(int) {}

VA(0x0048c094, 0x73)
int armyGroup::HasAllUndead(void) { return 0; }

VA(0x0048c107, 0x73)
int armyGroup::HasSomeUndead(void) { return 0; }

VA(0x0048c17a, 0x24d)
int armyGroup::GetMorale(class hero *, class town *, class armyGroup *) { return 0; }

VA(0x0048c3c7, 0x2f)
void armyGroup::Dismiss(int) {}

VA(0x0048c3f6, 0x55)
int armyGroup::IsMember(int) { return 0; }

VA(0x0048c44b, 0x14e)
int armyGroup::IsHomogeneous(int) { return 0; }

VA(0x0048c599, 0x54)
int armyGroup::CanJoin(int) { return 0; }

VA(0x0048c5ed, 0x54)
int armyGroup::GetNumArmies(void) { return 0; }

VA(0x0048c641, 0x11c)
int armyGroup::Add(int, int, int) { return 0; }

VA(0x0048c75d, 0x75)
void armyGroup::Swap(int, class armyGroup *, int) {}

VA(0x0048c7d2, 0x14d)
void armyGroup::DamageGroup(float) {}

