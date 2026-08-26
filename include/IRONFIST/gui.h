#ifndef HOMM2_IRONFIST_GUI_H
#define HOMM2_IRONFIST_GUI_H

#include <string>

#include <Ints.h>

class heroWindow;

namespace ironfist {

/*
 * Ironfist's widget-message convenience wrappers (gui/msg.cpp), expressed
 * over the recovered message types.
 */
void GUIBroadcastMessage(heroWindow* hwnd, i32 id, i32 command, void* payload);
void GUIAddFlag(heroWindow* hwnd, i32 id, i32 flags);
void GUIRemoveFlag(heroWindow* hwnd, i32 id, i32 flags);
void GUISetImgIdx(heroWindow* hwnd, i32 id, i32 frame);
void GUISetIcon(heroWindow* hwnd, i32 id, char* iconName);
void GUISetIcon(heroWindow* hwnd, i32 id, std::string& iconName);
void GUISetText(heroWindow* hwnd, i32 id, char* text);
void GUISetText(heroWindow* hwnd, i32 id, std::string& text);
void GUIDroplistAdd(heroWindow* hwnd, i32 id, char* text);
void GUIDroplistAdd(heroWindow* hwnd, i32 id, std::string& text);
void GUIDroplistClear(heroWindow* hwnd, i32 id);
i32 GUIGetDropdownSelection(heroWindow* hwnd, i32 id);
void GUISetDropdownSelection(heroWindow* hwnd, i32 id, i32 index);

} // namespace ironfist

#endif
