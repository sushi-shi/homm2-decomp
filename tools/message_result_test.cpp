#include <BASE/message.h>

#include <cstdio>
#include <initializer_list>

int main() {
    for (const i32 result : {0x7801, 0x7802, -1, 0}) {
        tag_message message{};
        message.type = MESSAGE_WIDGET;
        message.payload.widget.screenY = -123;
        message.payload.widget.data.value = result;
        SetExecutiveResult(message, message.payload.widget.data.value);
        if (message.type != MESSAGE_EXECUTIVE
            || message.payload.executive.command != EXECUTIVE_COMMAND_RETURN_RESULT
            || message.payload.executive.result != result) {
            std::fprintf(stderr, "dialog result lost during message variant transition\n");
            return 1;
        }
    }
}
