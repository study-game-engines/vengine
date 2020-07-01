#pragma once

#include <Windows.h>

extern WPARAM currentUpKey;
extern WPARAM currentDownKey;
extern bool mouseUp;

void StoreKeyDownInput(WPARAM key);
void StoreKeyUpInput(WPARAM key);
void StoreMouseDownInput(WPARAM key);
void StoreMouseUpInput(WPARAM key);
bool GetKeyUpState(int key);
bool GetKeyDownState(int key);
bool GetMouseDownState();
bool GetMouseUpState();

