#include <SOURCE/EVENTS.h>

#include <cstdio>

int main() {
    struct Case { const char* answer; const char* expected; bool equal; };
    const Case cases[] = {
        {"Gold", "gold", true}, {"golden", "gold", true},
        {"golf", "gold", false}, {"gol", "gold", false},
        {"cats", "cat ", true}, {"catapult", "cat", true},
        {"a", "a ", false}, {"a ", "a ", true},
        {"anything", "", true}, {"", "    ", false}, {"    ", "    ", true},
        {"мост", "море", false}, {"МОРЕ", "море", true},
        {"мореплаватель", "море", true}, {"мор", "море", false},
        {"ЁЖИК", "ёжик", true}, {"ÉTÉ", "été", true},
        {"猫犬鳥魚", "猫犬鳥虫", false}, {"猫犬鳥魚山", "猫犬鳥魚", true},
        {"猫犬鳥", "猫犬鳥 ", true}, {"\xff", "cat", false},
        {"cat", "\xff", false},
    };
    for (const Case& test : cases) {
        if ((RiddleStringsEqual(test.answer, test.expected) != 0) != test.equal) {
            std::fprintf(stderr, "riddle mismatch: '%s' / '%s'\n", test.answer, test.expected);
            return 1;
        }
    }
    return 0;
}
