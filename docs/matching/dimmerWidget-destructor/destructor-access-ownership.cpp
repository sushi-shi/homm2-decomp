// Target: retail scalar deleting wrapper VA 0x004d3440.
// Measured 2026-09-26, matcher-3 / matcher/bss-dimmer, pinned VC6 SP5.
//
// New ownership hypothesis: derived destruction may be protected/private while
// construction is public. Retail calls the ordinary dimmerWidget destructor
// only from its scalar deleting wrapper (fresh homm2 sema xref 0xd3470).
// The real WINDOW client constructs dimmerWidget objects, while widget ownership
// supports polymorphic destruction through the base. Access control therefore
// remained a plausible owner-header claim, not a request for fake code/storage.
// Previous declaration-order/body/split matrices did not vary this access bound.
//
// Matrix: public / protected / private access for only the virtual derived
// destructor declaration. Constructor and other method declarations remain
// public; all method definitions, base layout, include set, and production
// unit_flags(BASE/DIMMER) remain unchanged. The control has the original header.
//
// Artifacts:
//   build/link/dimmer-access-ownership/run.py
//   build/link/dimmer-access-ownership/{run.log,results.json,audit.json}
//   build/link/dimmer-access-ownership/{public,protected,private}/
// Each arm retains its disposable complete header, source, and raw object.
// All three requested arms compiled successfully; no timeout or truncation.
//
// All three emit:
//   section 3 ctor(), 4 vtable, 5 ??_G, 6 ctor(args), 7 Read, 8 Main,
//   9 Draw, 10 ordinary destructor.
// Access affects only the destructor/wrapper decorated name:
//   public:    ??1dimmerWidget@@UAE@XZ / ??_GdimmerWidget@@UAEPAXI@Z
//   protected: ??1dimmerWidget@@MAE@XZ / ??_GdimmerWidget@@MAEPAXI@Z
//   private:   ??1dimmerWidget@@EAE@XZ / ??_GdimmerWidget@@EAEPAXI@Z
// The vtable's weak vector-wrapper reference carries the corresponding access.
//
// Audit: all seven method bodies and the vtable retain exact raw section bytes,
// sizes, and offsets. Complete ordered relocation sites/types/addends agree;
// identities agree after explicitly mapping the three access decorations of
// the same destructor and deleting wrappers. No relocation field is masked to
// establish this result. The public control matches the prior audited baseline.
//
// Disposition: rejected. None changes the first-constructor-adjacent wrapper
// order. No source/header/build change retained, and no claim is made about
// untested compiler states or other ownership models. A final native link is
// unnecessary for rejection because the desired contribution-order change did
// not occur in any arm. This is a complete three-arm structural comparison,
// not an unchanged-source TU-state census.

// Baseline owner header fragment:
// public:
//     virtual ~dimmerWidget(void) OVERRIDE;
//     virtual void Draw(void) OVERRIDE;

// Protected owner header fragment:
// protected:
//     virtual ~dimmerWidget(void) OVERRIDE;
// public:
//     virtual void Draw(void) OVERRIDE;

// Private owner header fragment:
// private:
//     virtual ~dimmerWidget(void) OVERRIDE;
// public:
//     virtual void Draw(void) OVERRIDE;
