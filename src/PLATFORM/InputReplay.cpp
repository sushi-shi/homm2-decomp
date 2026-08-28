#include <PLATFORM/InputReplay.h>

#include <cctype>
#include <limits>
#include <sstream>
#include <utility>

namespace platform {
namespace {

bool HasTrailingField(std::istringstream& fields) {
    std::string trailing;
    return static_cast<bool>(fields >> trailing);
}

bool IsBlankOrComment(const std::string& line) {
    for (const char character : line) {
        if (std::isspace(static_cast<unsigned char>(character)) != 0) {
            continue;
        }
        return character == '#';
    }
    return true;
}

}

bool InputReplay::Load(
    std::istream& stream,
    ReplayKeyResolver resolveKey,
    ReplayParseError& error
) {
    std::vector<TimedEvent> parsed;
    std::string line;
    std::size_t lineNumber = 0;
    std::uint32_t previousMilliseconds = 0;
    bool havePrevious = false;

    error = {};
    while (std::getline(stream, line)) {
        ++lineNumber;
        if (IsBlankOrComment(line)) {
            continue;
        }

        std::istringstream fields(line);
        unsigned long long milliseconds = 0;
        std::string action;
        fields >> milliseconds >> action;
        if (!fields) {
            error = {lineNumber, "expected: <milliseconds> <action> [arguments]"};
            return false;
        }
        if (milliseconds > std::numeric_limits<std::uint32_t>::max()) {
            error = {lineNumber, "timestamp is outside the 32-bit millisecond range"};
            return false;
        }

        TimedEvent replay;
        replay.milliseconds = static_cast<std::uint32_t>(milliseconds);
        if (havePrevious && replay.milliseconds < previousMilliseconds) {
            error = {lineNumber, "timestamps must be nondecreasing"};
            return false;
        }

        if (action == "move") {
            replay.event.type = Event::Type::MouseMove;
        } else if (action == "left-down") {
            replay.event.type = Event::Type::MouseDown;
            replay.event.button = MouseButton::Left;
        } else if (action == "left-up") {
            replay.event.type = Event::Type::MouseUp;
            replay.event.button = MouseButton::Left;
        } else if (action == "right-down") {
            replay.event.type = Event::Type::MouseDown;
            replay.event.button = MouseButton::Right;
        } else if (action == "right-up") {
            replay.event.type = Event::Type::MouseUp;
            replay.event.button = MouseButton::Right;
        } else if (action == "key-down" || action == "key-up") {
            std::string name;
            fields >> name;
            if (!fields || resolveKey == nullptr
                || !resolveKey(
                    name,
                    replay.event.key,
                    replay.event.scanCode,
                    replay.event.physicalCode,
                    replay.event.character
                )) {
                error = {lineNumber, "unknown or missing key name"};
                return false;
            }
            replay.event.type =
                action == "key-down" ? Event::Type::KeyDown : Event::Type::KeyUp;
            if (HasTrailingField(fields)) {
                error = {lineNumber, "unexpected field after key name"};
                return false;
            }
        } else if (action == "text") {
            replay.event.type = Event::Type::TextInput;
            std::getline(fields, replay.event.text);
            if (!replay.event.text.empty() && replay.event.text.front() == ' ') {
                replay.event.text.erase(0, 1);
            }
        } else {
            error = {lineNumber, "unknown action: " + action};
            return false;
        }

        if (replay.event.type == Event::Type::MouseMove
            || replay.event.type == Event::Type::MouseDown
            || replay.event.type == Event::Type::MouseUp) {
            fields >> replay.event.position.x >> replay.event.position.y;
            if (!fields) {
                error = {lineNumber, "mouse action requires x and y coordinates"};
                return false;
            }
            if (HasTrailingField(fields)) {
                error = {lineNumber, "unexpected field after mouse coordinates"};
                return false;
            }
        }

        previousMilliseconds = replay.milliseconds;
        havePrevious = true;
        parsed.push_back(std::move(replay));
    }
    if (stream.bad()) {
        error = {lineNumber, "failed while reading replay"};
        return false;
    }

    m_events = std::move(parsed);
    m_index = 0;
    m_start = 0;
    m_started = false;
    return true;
}

void InputReplay::Start(std::uint32_t now) {
    m_start = now;
    m_index = 0;
    m_started = true;
}

bool InputReplay::NextDue(std::uint32_t now, Event& event) {
    if (!m_started || m_index >= m_events.size()) {
        return false;
    }
    const std::uint32_t elapsed = now - m_start;
    if (m_events[m_index].milliseconds > elapsed) {
        return false;
    }
    event = m_events[m_index].event;
    ++m_index;
    return true;
}

}
