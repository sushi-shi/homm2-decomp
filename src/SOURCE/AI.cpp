// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_Re\AI.OBJ   from: (directly linked into exe)
// functions: 19   data: 0
// RVA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

RVA(0x004c0790, 0x8d7)
int combatManager::AICheckRetreat(void);

RVA(0x004c1067, 0x129c)
void combatManager::DoCompAI(int);

RVA(0x004c2303, 0xc9)
float combatManager::GetModLichDamage(class army *, float);

RVA(0x004c23cc, 0x32e)
void combatManager::DoLichShot(class army *);

RVA(0x004c26fa, 0x131)
int combatManager::GetShooterMask(int);

RVA(0x004c282b, 0xc0)
int combatManager::GetMirrorImageMask(int);

RVA(0x004c28eb, 0x11f)
int combatManager::GetFlyerMask(int);

RVA(0x004c2a0a, 0xc0)
int combatManager::GetAllMask(int);

RVA(0x004c2aca, 0x141)
int combatManager::GetWalkerMask(int);

RVA(0x004c2c0b, 0xe9)
int combatManager::GetOutOfItMask(int);

RVA(0x004c2cf4, 0xd6)
int combatManager::GetTraitorMask(int);

RVA(0x004c2dca, 0x1f1)
int combatManager::GetBestArmy(int, int);

RVA(0x004c2fbb, 0xc2)
int combatManager::GetWorstArmy(int, int);

RVA(0x004c307d, 0x16f)
int combatManager::GetClosestArmy(class army *, int, int);

RVA(0x004c31ec, 0xc1)
unsigned long int combatManager::GetStrength(int, int);

RVA(0x004c32ad, 0x1bb)
int combatManager::AttemptAttack(class army *, int, int);

RVA(0x004c3468, 0x182)
int combatManager::AttemptAdjacentAttack(class army *);

RVA(0x004c35ea, 0x240)
int combatManager::WalkTowardArmyFront(class army *, int, int);

RVA(0x004c382a, 0x244)
int combatManager::WalkTowardArmy(class army *, int, int);

