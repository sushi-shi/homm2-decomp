#include <IRONFIST/gui.h>

#include <BASE/heroWindow.h>
#include <BASE/message.h>

static tag_message MakeWidgetMessage(i32 id, i32 command) {
    tag_message message;
    message.type = MESSAGE_WIDGET;
    message.payload.widget.command = static_cast<BaseWidgetCommand>(command);
    message.payload.widget.id = id;
    return message;
}

void GUIBroadcastMessage(heroWindow* hwnd, i32 id, i32 command, void* payload) {
    tag_message message = MakeWidgetMessage(id, command);
    message.payload.widget.data.text = static_cast<char*>(payload);
    hwnd->BroadcastMessage(message);
}

void GUIAddFlag(heroWindow* hwnd, i32 id, i32 flags) {
    tag_message message = MakeWidgetMessage(id, H2EnumIndex(WIDGET_COMMAND_SET_FLAGS));
    message.payload.widget.data.value = flags;
    hwnd->BroadcastMessage(message);
}

void GUIRemoveFlag(heroWindow* hwnd, i32 id, i32 flags) {
    tag_message message = MakeWidgetMessage(id, H2EnumIndex(WIDGET_COMMAND_CLEAR_FLAGS));
    message.payload.widget.data.value = flags;
    hwnd->BroadcastMessage(message);
}

void GUISetImgIdx(heroWindow* hwnd, i32 id, i32 frame) {
    tag_message message = MakeWidgetMessage(id, H2EnumIndex(WIDGET_COMMAND_SET_FRAME));
    message.payload.widget.data.value = frame;
    hwnd->BroadcastMessage(message);
}

void GUISetIcon(heroWindow* hwnd, i32 id, char* iconName) {
    tag_message message = MakeWidgetMessage(id, H2EnumIndex(WIDGET_COMMAND_SET_ICON));
    message.payload.widget.data.text = iconName;
    hwnd->BroadcastMessage(message);
}

void GUISetIcon(heroWindow* hwnd, i32 id, std::string& iconName) {
    GUISetIcon(hwnd, id, &iconName[0]);
}

void GUISetText(heroWindow* hwnd, i32 id, char* text) {
    tag_message message = MakeWidgetMessage(id, H2EnumIndex(WIDGET_COMMAND_SET_TEXT));
    message.payload.widget.data.text = text;
    hwnd->BroadcastMessage(message);
}

void GUISetText(heroWindow* hwnd, i32 id, std::string& text) {
    GUISetText(hwnd, id, &text[0]);
}

void GUIDroplistAdd(heroWindow* hwnd, i32 id, char* text) {
    tag_message message = MakeWidgetMessage(id, H2EnumIndex(WIDGET_COMMAND_APPEND_ITEM));
    message.payload.widget.data.text = text;
    hwnd->BroadcastMessage(message);
}

void GUIDroplistAdd(heroWindow* hwnd, i32 id, std::string& text) {
    GUIDroplistAdd(hwnd, id, &text[0]);
}

void GUIDroplistClear(heroWindow* hwnd, i32 id) {
    tag_message message = MakeWidgetMessage(id, H2EnumIndex(WIDGET_COMMAND_CLEAR_ITEMS));
    message.payload.widget.data.value = 0;
    hwnd->BroadcastMessage(message);
}

i32 GUIGetDropdownSelection(heroWindow* hwnd, i32 id) {
    tag_message message = MakeWidgetMessage(id, H2EnumIndex(WIDGET_COMMAND_GET_SELECTION));
    message.payload.widget.data.value = 0;
    hwnd->BroadcastMessage(message);
    return message.payload.widget.data.value;
}

void GUISetDropdownSelection(heroWindow* hwnd, i32 id, i32 index) {
    tag_message message = MakeWidgetMessage(id, H2EnumIndex(WIDGET_COMMAND_SET_SELECTION));
    message.payload.widget.data.value = index;
    hwnd->BroadcastMessage(message);
}
