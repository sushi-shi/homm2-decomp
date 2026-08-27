#include <PLATFORM/InputReplay.h>

#include <cstdio>
#include <sstream>
#include <string_view>

namespace {

bool Expect(bool condition, const char* description) {
    if (condition) {
        return true;
    }
    std::fprintf(stderr, "input replay mismatch: %s\n", description);
    return false;
}

bool ResolveKey(
    std::string_view name,
    platform::Key& key,
    unsigned& scanCode,
    unsigned& character
) {
    if (name != "A") {
        return false;
    }
    key = platform::Key::A;
    scanCode = 0x1e;
    character = 'A';
    return true;
}

bool Rejects(const char* input, std::size_t line, const char* description) {
    platform::InputReplay replay;
    platform::ReplayParseError error;
    std::istringstream stream(input);
    return Expect(!replay.Load(stream, ResolveKey, error) && error.line == line, description);
}

}

int main() {
    using platform::Event;
    using platform::InputReplay;
    using platform::MouseButton;
    using platform::ReplayParseError;

    bool valid = true;
    std::istringstream stream(
        "# timestamped input\n"
        "0 move 10 20\n"
        "5 left-down 10 20\n"
        "5 key-down A\n"
        "8 text Привет мир\n"
        "9 left-up 11 21\n"
    );
    InputReplay replay;
    ReplayParseError error;
    valid &= Expect(replay.Load(stream, ResolveKey, error), "valid replay parses");
    valid &= Expect(replay.Size() == 5 && replay.Remaining() == 5, "event count");

    Event event;
    replay.Start(0xfffffffcu);
    valid &= Expect(
        replay.NextDue(0xfffffffcu, event)
            && event.type == Event::Type::MouseMove
            && event.position.x == 10 && event.position.y == 20,
        "zero-time event"
    );
    valid &= Expect(!replay.NextDue(0xffffffffu, event), "future event held");
    valid &= Expect(
        replay.NextDue(1u, event)
            && event.type == Event::Type::MouseDown
            && event.button == MouseButton::Left,
        "deadline survives tick wrap"
    );
    valid &= Expect(
        replay.NextDue(1u, event)
            && event.type == Event::Type::KeyDown
            && event.key == platform::Key::A
            && event.scanCode == 0x1e
            && event.character == 'A',
        "equal timestamps retain source order"
    );
    valid &= Expect(
        replay.NextDue(4u, event)
            && event.type == Event::Type::TextInput
            && event.text == "Привет мир",
        "UTF-8 text payload"
    );
    valid &= Expect(
        replay.NextDue(5u, event)
            && event.type == Event::Type::MouseUp
            && event.position.x == 11 && event.position.y == 21,
        "last event"
    );
    valid &= Expect(!replay.NextDue(100u, event) && replay.Remaining() == 0, "replay drains");

    valid &= Rejects("move 1 2\n", 1, "missing timestamp");
    valid &= Rejects("0 wheel 1 2\n", 1, "unknown action");
    valid &= Rejects("0 move 1\n", 1, "missing coordinate");
    valid &= Rejects("0 key-down Missing\n", 1, "unknown key");
    valid &= Rejects("2 move 1 2\n1 move 1 2\n", 2, "decreasing timestamp");
    valid &= Rejects("0 move 1 2 trailing\n", 1, "trailing mouse field");
    valid &= Rejects("0 key-down A trailing\n", 1, "trailing key field");

    InputReplay unchanged;
    std::istringstream first("0 move 3 4\n");
    valid &= Expect(unchanged.Load(first, ResolveKey, error), "transaction seed");
    std::istringstream invalid("0 unknown\n");
    valid &= Expect(!unchanged.Load(invalid, ResolveKey, error), "transaction failure");
    valid &= Expect(unchanged.Size() == 1, "failed load preserves prior replay");

    return valid ? 0 : 1;
}
