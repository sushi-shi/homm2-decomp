#ifndef HOMM2_SOURCE_ARMYGROUP_H
#define HOMM2_SOURCE_ARMYGROUP_H
// Reconstructed class (SOURCE) from CodeView NB09 of HEROES2W.EXE — NOT original source.
// 13 methods, 0 own-virtual, 0 static data.
#include <va.h>
// forward declarations:
class hero;
class town;

class armyGroup {
public:
    // --- constructors ---
    armyGroup(void);
    // --- methods ---
    void View(int);
    int HasAllUndead(void);
    int HasSomeUndead(void);
    int GetMorale(class hero *, class town *, class armyGroup *);
    void Dismiss(int);
    int IsMember(int);
    int IsHomogeneous(int);
    int CanJoin(int);
    int GetNumArmies(void);
    int Add(int, int, int);
    void Swap(int, class armyGroup *, int);
    void DamageGroup(float);
};
#endif // HOMM2_SOURCE_ARMYGROUP_H
