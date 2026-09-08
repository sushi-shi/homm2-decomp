// Diagnostic for the 2026-09-08 audit, not a passing regression test.
// Calls the production GenerateStandardFileName from GAME.cpp.
#include <cstring>
#include <cstdio>

void GenerateStandardFileName(char*, char*);

// Equivalent for the '.' search used here; avoids linking game startup.
char* FindLastToken(char* text, char token) { return std::strrchr(text, token); }

int main(int argc, char**) {
    char name[202];
    std::memset(name, 'A', sizeof(name));
    name[argc > 1 ? 8 : 101] = '\0';
    char scratch[100]{};
    GenerateStandardFileName(name, scratch);
    std::puts(scratch);
}
