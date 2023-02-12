#pragma once

#include "resource.h"
#include <shellapi.h>
#include <windowsx.h>

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
void	HandleMouseDragging(HWND hwnd, long x, long y);
void	HandleMouseWheel(short wheelDelta);

// Field functions
HWND	CreateField(HWND parent, int x, int y, LPCWSTR labelText);
void	SetFieldValue(HWND field, int value);

// Debug functions
void	PrintMessage(const wchar_t* format, ...);

// Object Information
HWND _infoBox;
HWND _infoVertices;
HWND _infoTriangles;
HWND _infoLayers;
Renderer::ObjectInfo _objectInfo;

// States
bool _objectLoaded = false;
bool _isDragging = false;
bool _tumbling = true;
POINT _dragOrigin;

// Renderer class
Renderer renderer;

// Control IDs
#define IDC_RESET_OBJECT WM_USER + 1