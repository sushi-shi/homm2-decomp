// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_Re\PHILAI.OBJ   from: (directly linked into exe)
// functions: 82   data: 51
// RVA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

RVA(0x0043781b, 0x1b5)
void ResetHeroRVs(int, int, int);

RVA(0x004379d0, 0x180)
void CheckDoMain(int, int);

RVA(0x00437b50, 0x10)
void ShowStatus(void);

RVA(0x00437b60, 0x55)
void philAI::constructor(void);

RVA(0x00437bb5, 0xac)
void philAI::DoAllHeroInteractions(void);

RVA(0x00437c61, 0x37e)
void philAI::CheckForCreatureUpgrades(void);

RVA(0x00437fdf, 0x4be)
void philAI::CheckBuyStuff(void);

RVA(0x0043849d, 0x2e8)
int philAI::GoodAdjacent(int *);

RVA(0x00438785, 0x4b8)
void philAI::CheckReload(void);

RVA(0x00438c3d, 0x302)
void philAI::CheckBerserk(void);

RVA(0x00438f3f, 0x71)
void philAI::DimensionDoorTo(int, int);

RVA(0x00438fb0, 0x3f9)
int philAI::DoAnywhereDDoorTownGate(int);

RVA(0x004393a9, 0x158)
int philAI::DoDimensionDoor(class hero *);

RVA(0x00439501, 0xb7)
void philAI::SetupRelativeHeroStrengths(void);

RVA(0x004395b8, 0x79)
void ValidateHero(class hero *);

RVA(0x00439631, 0xcad)
void philAI::DoAI(int);

RVA(0x0043a2de, 0x4b)
void philAI::GetGameAIVars(void);

RVA(0x0043a329, 0xe2b)
void philAI::GetTurnAIVars(int);

RVA(0x0043b154, 0x5f4)
void philAI::GetBestBHC(int, struct BHC &);

RVA(0x0043b748, 0x11d)
class hero * philAI::DetermineHeroToMove(int);

RVA(0x0043b865, 0xe7d)
int philAI::DetermineTargetPosition(int &, int &, int, int &);

RVA(0x0043c6e2, 0x791)
void philAI::ProbableOutcomeOfBattle(class armyGroup *, class hero *, class armyGroup *, class hero *, class armyGroup *, int, int, int, float &, int &, int &, int &, int &, int &);

RVA(0x0043ce73, 0x1e)
float philAI::GetOddsOfWinning(int);

RVA(0x0043ce91, 0x826)
void philAI::ValueOfBuyingBuilding(class town *, int, int &, float &);

RVA(0x0043d6b7, 0x19b)
void philAI::GetBestBuilding(class town *, struct BHC &, float &);

RVA(0x0043d852, 0x306)
void philAI::ValueOfBuyingCreature(class town *, int, int &, int, float &);

RVA(0x0043db58, 0x35b)
void philAI::GetBestCreature(class town *, struct BHC &, float &);

RVA(0x0043deb3, 0x48)
int philAI::CreaturesToBuy(class town *, int);

RVA(0x0043defb, 0x5f)
int philAI::CreaturesToBuy(int, int);

RVA(0x0043df5a, 0x9c)
int philAI::MaxBuyableCreatures(int);

RVA(0x0043dff6, 0x2b2)
void philAI::ValueOfBuyingHero(class town *, class hero *, int &, float &);

RVA(0x0043e2a8, 0x1b1)
void philAI::GetBestHero(class town *, struct BHC &, float &);

RVA(0x0043e459, 0x65)
void philAI::LikelihoodOfEnemyAttacking(class town *, class hero *, float &, float &, int &, int &, int &, float &);

RVA(0x0043e4be, 0x1a)
int philAI::MeanRVOfUnexploredTerritory(int);

RVA(0x0043e4d8, 0x1d8)
void philAI::GetGameAttentionValue(int);

RVA(0x0043e6b0, 0xf2)
void philAI::GetTurnAttentionValue(int);

RVA(0x0043e7a2, 0xa6)
int philAI::RVConversion(int * const);

RVA(0x0043e848, 0xd0)
float philAI::TurnsToBuy(int * const);

RVA(0x0043e918, 0x62d)
int philAI::RVOfPosition(int, int, int, int, int, int, int, int, int, int);

RVA(0x0043ef45, 0xaf9)
int philAI::StrategicValueOfPosition(int, int, int, int, int *, int);

RVA(0x0043fa3e, 0x14e)
int philAI::ValueOfTown(class town *);

RVA(0x0043fb8c, 0x180)
void philAI::TurnCostResource(int);

RVA(0x0043fd0c, 0x175)
float philAI::TurnValueOfObelisk(int);

RVA(0x0043fe81, 0x51)
float philAI::FutureDeflator(int * const);

RVA(0x0043fed2, 0xbf8)
int philAI::FightValueOfStack(class armyGroup *, class hero *, int, int, int, int);

RVA(0x00440aca, 0x1e7)
void philAI::EvaluateOneTimeCreaturePurchase(int, int, int, int &, int &, int &);

RVA(0x00440cb1, 0x768)
int philAI::QuickCombat(class armyGroup *, class hero *, class armyGroup *, class hero *, int, int, float &, float &);

RVA(0x00441419, 0x422)
void philAI::HeroInteractionAtHero(class hero *, class hero *, int, int *);

RVA(0x0044183b, 0x882)
void philAI::HeroInteractionAtTown(class hero *, class town *, int, int *);

RVA(0x004420bd, 0x4ba)
void philAI::RedistributeTroops(class armyGroup *, class armyGroup *, int, int, int, int, int);

RVA(0x00442577, 0x39)
int philAI::ChooseGoldOrExperience(int, int);

RVA(0x004425b0, 0xc7)
void philAI::ChooseEvaluateBattle(class armyGroup *, class hero *, class armyGroup *, class hero *, int, int, int, int &, int &);

RVA(0x00442677, 0xfa)
int philAI::ChooseToFightForArtifact(int, int, int);

RVA(0x00442771, 0x53)
int philAI::NetValueOfArtifact(int, int, int, int);

RVA(0x004427c4, 0x1d)
int philAI::ChooseToPayRansomOnHero(int);

RVA(0x004427e1, 0xd7)
void philAI::BuildBuilding(class town *, int);

RVA(0x004428b8, 0x328)
void philAI::BuildHero(class town *, int);

RVA(0x00442be0, 0x2cd)
void philAI::BuildCreature(class town *, int, int);

RVA(0x00442ead, 0x15a)
int philAI::CanBuyBHC(struct BHC &);

RVA(0x00443007, 0x164)
int philAI::CombatMonsterEvent(class hero *, int, int *, class mapCell *);

RVA(0x0044316b, 0x6d7)
int philAI::FightEvent(class hero *, class mapCell *, int);

RVA(0x00443842, 0x73)
int philAI::DamageGroup(class armyGroup *, class hero *, class hero *, float);

RVA(0x004438b5, 0xcb)
void philAI::IncrementHourGlass(void);

RVA(0x00443980, 0x227)
void philAI::TownEvent(class mapCell *, class hero *, int, int);

RVA(0x00443ba7, 0xad)
int philAI::ComputeUpgradeValue(int, int);

RVA(0x00443c54, 0x271)
int philAI::ComputeValueOfSS(class hero *, int, int);

RVA(0x00443ec5, 0x59)
int philAI::ComputeValueOfFreeSS(class hero *, int);

RVA(0x00443f1e, 0xa6)
int philAI::ManaRefreshValue(class hero *, int);

RVA(0x00443fc4, 0x1ac5)
int philAI::ValueOfEventAtPosition(int, int, int, int *);

RVA(0x00445a89, 0x299)
int philAI::EvaluateGenericSite(class mapCell *);

RVA(0x00445d22, 0x5e)
int philAI::EvaluateBarrier(class mapCell *);

RVA(0x00445d80, 0x5e)
int philAI::EvaluatePassword(class mapCell *);

RVA(0x00445dde, 0xf5)
int philAI::EvaluateRecruitSite(class mapCell *);

RVA(0x00445ed3, 0x1d)
int philAI::EvaluateJail(class mapCell *);

RVA(0x00445ef0, 0xf6)
void InitAIMapVars(void);

RVA(0x00445fe6, 0x112)
void CloseAIMapVars(void);

RVA(0x004460f8, 0x26a)
int OnMySide(int);

RVA(0x00446362, 0x2bc)
int philAI::EvaluateArtifactEvent(int, int);

RVA(0x0044661e, 0x30a)
int philAI::EvaluateMineEvent(int, int, int, int *);

RVA(0x00446928, 0x33e)
int philAI::EvaluateMonsterEvent(int, int, int *);

RVA(0x00446c66, 0x5ab)
int philAI::EvaluateHeroEvent(int, int, int, int, int *);

RVA(0x00447211, 0x4fa)
int philAI::EvaluateTownEvent(int, int, int, int, int *);

// ---- data / globals / vtables ----
DATA(0x004f20e0)  // float fFirstWeekTownFV
DATA(0x004f20e4)  // int iVepCacheHits
DATA(0x004f20e8)  // int iTotalVepHits
DATA(0x004f20ec)  // int giShowComputerRoute
DATA(0x004f20f0)  // short int * gaiLiveChanceOfPos
DATA(0x004f20f4)  // short int * gaiHeroStrategicRVOfPos
DATA(0x004f20f8)  // short int * gaiHeroEventStratRVOfPos
DATA(0x004f20fc)  // signed char * gaiTurnValueOfMine
DATA(0x004f2100)  // signed char * gaiEnemyHeroReachable
DATA(0x004f2104)  // long int glLastStartTick
DATA(0x004f2108)  // long int glCurTicks
DATA(0x004f210c)  // long int glTotalTicks
DATA(0x004f2110)  // class hero * gpCurAIHero
DATA(0x004f2114)  // float gfAttackHumanBonus
DATA(0x004f2118)  // float gfAttackComputerBonus
DATA(0x004f211c)  // int iLastFrameRateTimer
DATA(0x004f22bc)  // int bSVSearchArrayInUse
DATA(0x004f2340)  // int bEvaluatingTravelGates
DATA(0x00525620)  // int gbReduceByBerserk
DATA(0x00525624)  // float fBerserkFactor
DATA(0x00525628)  // int giCurPlayer
DATA(0x0052562c)  // signed char * giBuildShipyard
DATA(0x00525634)  // int giMaxHeroesForThisPlayer
DATA(0x00525638)  // signed char * giBuildBoat
DATA(0x00525640)  // float fReduceFactor
DATA(0x00525644)  // unsigned char giCurPlayerBit
DATA(0x00525648)  // int giBestShipyardDist
DATA(0x0052564c)  // int bHeroBuiltThisTurn
DATA(0x00525650)  // short int * gaiHeroLiveChance
DATA(0x005256bc)  // int giHumanTownConquered
DATA(0x005256c0)  // int giCurTurn
DATA(0x005256c8)  // int * costTemp
DATA(0x005256e4)  // int iAlphaMale
DATA(0x005256e8)  // int iDummy
DATA(0x005256ec)  // int gbPossibleShipyardFound
DATA(0x005256f0)  // class searchArray SVSearchArray
DATA(0x00527c08)  // float * gafAITurnCostResource
DATA(0x00527c24)  // int iCurPlaceToVisit
DATA(0x00527c28)  // int giBestShipyardId
DATA(0x00527c2c)  // int gbActualBoatFound
DATA(0x00527c30)  // unsigned char giCurWatchPlayerBit
DATA(0x00527c34)  // class playerData * gpCurPlayer
DATA(0x00527c38)  // float * gfHeroInteractionBonus
DATA(0x00527d10)  // int gbBerserk
DATA(0x00527d14)  // int giCurAIHeroMorale
DATA(0x00527d18)  // signed char * giBuildBoatStuffTurn
DATA(0x00527d20)  // int (*)[2] iPlacesVisited
DATA(0x00527e10)  // int gbReduceByReload
DATA(0x00527e14)  // int gbTroopReload
DATA(0x00527e18)  // int giCurAIHeroLuck
DATA(0x00527e1c)  // int gbActualShipyardFound
