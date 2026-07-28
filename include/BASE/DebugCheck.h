#ifndef HOMM2_DEBUGCHECK_H
#define HOMM2_DEBUGCHECK_H

// Retail scatters calls to a debug hook whose shipping body is empty: each call
// site survives only as the /Ob1 inline-continuation `jmp $+0`. Header inlines
// leave no CodeView record, so the retail name is unrecoverable; every such
// site shares this one reconstructed no-op. Include this header only from TUs
// with proven sites - the declaration itself perturbs TU state.
inline void DebugCheck(void) {
}

#endif
