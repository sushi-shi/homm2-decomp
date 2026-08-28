#ifndef HOMM2_PLATFORM_INPUT_REPLAY_H
#define HOMM2_PLATFORM_INPUT_REPLAY_H

#include "Input.h"

#include <cstddef>
#include <cstdint>
#include <iosfwd>
#include <string>
#include <string_view>
#include <vector>

namespace platform {

using ReplayKeyResolver = bool (*)(
    std::string_view name,
    Key& key,
    unsigned& scanCode,
    unsigned& physicalCode,
    unsigned& character
);

struct ReplayParseError {
    std::size_t line = 0;
    std::string message;
};

class InputReplay {
public:
    bool Load(std::istream& stream, ReplayKeyResolver resolveKey, ReplayParseError& error);
    void Start(std::uint32_t now);
    bool NextDue(std::uint32_t now, Event& event);

    bool Empty() const { return m_events.empty(); }
    std::size_t Size() const { return m_events.size(); }
    std::size_t Remaining() const { return m_events.size() - m_index; }

private:
    struct TimedEvent {
        std::uint32_t milliseconds = 0;
        Event event;
    };

    std::vector<TimedEvent> m_events;
    std::size_t m_index = 0;
    std::uint32_t m_start = 0;
    bool m_started = false;
};

}

#endif
