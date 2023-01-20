#pragma once

#include "resource.h"
#include <shellapi.h>
#include "Renderer.h"

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

// Initialization functions
void	CreateMainWindowControls();

// Event handlers
void	HandleDroppedFile(HDROP dropInfo);
void	HandleMouseWheel(short wheelDelta);

// Debug functions
void	PrintMessage(const wchar_t* format, ...);


// States
bool _objectLoaded = false;
bool _isDragging = false;
bool _tumbling = true;

// Renderer class
Renderer renderer;

// Control IDs
#define IDC_ROTATE WM_USER + 1