#include <BASE/inputManager.h>
#include <BASE/soundManager.h>
#include <BASE/icon.h>
#include <BASE/sample.h>
#include <SOURCE/combatManager.h>

#include <cstddef>

// These are runtime objects whose members cross ordinary reference/pointer
// interfaces. No byte-packing contract applies to them.
static_assert(alignof(inputManager) >= alignof(tag_message));
static_assert(offsetof(inputManager, m_eventRing) % alignof(tag_message) == 0);
static_assert(alignof(soundManager) >= alignof(i32));
static_assert(offsetof(soundManager, m_musicTrack) % alignof(i32) == 0);
static_assert(offsetof(soundManager, m_musicFadeSteps) % alignof(i32) == 0);
static_assert(alignof(icon) >= alignof(u8*));
static_assert(offsetof(icon, m_data) % alignof(u8*) == 0);
static_assert(alignof(sample) >= alignof(SamplePlaybackData));
static_assert(offsetof(sample, m_playbackData) % alignof(SamplePlaybackData) == 0);
static_assert(alignof(combatManager) >= alignof(hero));
static_assert(offsetof(combatManager, m_captain) % alignof(hero) == 0);

int main() { return 0; }
