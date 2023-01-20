// LWObjectViewer.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "LWObjectViewer.h"

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                        // current instance
WCHAR szTitle[MAX_LOADSTRING];          // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];    // the main window class name
HWND mainWindow;						// Main window handle
HWND renderWindow;						// Render output window handle
RECT renderWindowRect;					// Render output window client rect

// Definitions
UINT WINDOW_WIDTH = 1300;
UINT WINDOW_HEIGHT = 900;
UINT RENDER_WINDOW_WIDTH = 1024;
UINT RENDER_WINDOW_HEIGHT = 768;

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow) {
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// Initialize global strings
	LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_LWOBJECTVIEWER, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance(hInstance, nCmdShow)) {
		return FALSE;
	}

	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_LWOBJECTVIEWER));

	// Initialize renderer
	if (!renderer.Initialize(renderWindow, RENDER_WINDOW_WIDTH, RENDER_WINDOW_HEIGHT)) return 0;

	// Peek at initial message in queue
	MSG msg;
	msg.message = WM_NULL;
	PeekMessage(&msg, NULL, 0U, 0U, PM_NOREMOVE);

	// Message and rendering loop
	bool hasMessage;
	while (msg.message != WM_QUIT) {

		// Process window messages
		hasMessage = PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE) == 0 ? false : true;
		if (hasMessage) {

			// Translate and dispatch window message, same as original loop
			if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) {
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else {

			// No message, so process the scene if an object is loaded
			if (_objectLoaded) {

				// Update scene
				renderer.Update();

				// Render frame
				renderer.Render();

				// Present frame
				renderer.Present();
			}
		}
	}

	return (int)msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance) {
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_DBLCLKS;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_LWOBJECTVIEWER));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(LTGRAY_BRUSH);
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_LWOBJECTVIEWER);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow) {
	hInst = hInstance; // Store instance handle in our global variable

	mainWindow = CreateWindowW(
		szWindowClass,
		szTitle,
		WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		WINDOW_WIDTH,
		WINDOW_HEIGHT,
		nullptr,
		nullptr,
		hInstance,
		nullptr);

	if (!mainWindow) {
		return FALSE;
	}

	ShowWindow(mainWindow, nCmdShow);
	UpdateWindow(mainWindow);

	// Register window as drop target
	DragAcceptFiles(mainWindow, TRUE);

	// Populate main window controls
	CreateMainWindowControls();

	// Get render window rect
	GetClientRect(renderWindow, &renderWindowRect);

	return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {

	switch (message) {
		case WM_COMMAND:
			{
				int wmId = LOWORD(wParam);
				// Parse the menu selections:
				switch (wmId) {
					case IDM_ABOUT:
						DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
						break;
					case IDC_RESET_OBJECT:
						renderer.ResetTransformations();
						break;
					case IDM_EXIT:
						DestroyWindow(hWnd);
						break;
					default:
						return DefWindowProc(hWnd, message, wParam, lParam);
				}
			}
			break;
		case WM_DROPFILES:
			HandleDroppedFile((HDROP)wParam);
			break;
		case WM_LBUTTONDOWN:
			HandleMouseDragging(hWnd, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
			break;
		case WM_LBUTTONUP:
			if (_isDragging) {
				_isDragging = false;
			}
			break;
		case WM_MOUSEMOVE:
			if (_isDragging) {
				HandleMouseDragging(hWnd, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
			}
			break;
		case WM_MOUSEWHEEL:
			HandleMouseWheel(GET_WHEEL_DELTA_WPARAM(wParam));
			break;
		case WM_PAINT:
			{
				PAINTSTRUCT ps;
				HDC hdc = BeginPaint(hWnd, &ps);
				// Probably best not to do anything here since 
				// D3D will basically overwrite this area anyway
				EndPaint(hWnd, &ps);
			}
			break;
		case WM_DESTROY:

			// Shut down renderer
			renderer.Shutdown();

			// Post quit message
			PostQuitMessage(0);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
	UNREFERENCED_PARAMETER(lParam);
	switch (message) {
		case WM_INITDIALOG:
			return (INT_PTR)TRUE;

		case WM_COMMAND:
			if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) {
				EndDialog(hDlg, LOWORD(wParam));
				return (INT_PTR)TRUE;
			}
			break;
	}
	return (INT_PTR)FALSE;
}

/// <summary>
/// Create main window button
/// </summary>
/// <param name="x">X coordinate</param>
/// <param name="y">Y coordinate</param>
/// <param name="text">Button text</param>
/// <param name="id">Button ID</param>
HWND CreateButton(int x, int y, std::string text, UINT id) {

	// Convert button text
	std::wstring buttonText = std::wstring(text.begin(), text.end());

	// Create button as child of main window
	return CreateWindowW(
		L"BUTTON",
		buttonText.c_str(),
		WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
		x, y, 
		150, 30,
		mainWindow,
		(HMENU)id,
		(HINSTANCE)GetWindowLongPtr(mainWindow, GWLP_HINSTANCE),
		NULL
	);
}

/// <summary>
/// Create main window child controls
/// </summary>
void CreateMainWindowControls() {

	// Create child render frame
	renderWindow = CreateWindowW(
		L"Static",
		NULL,
		WS_VISIBLE | WS_CHILD,
		10, 10, 
		RENDER_WINDOW_WIDTH, RENDER_WINDOW_HEIGHT,
		mainWindow,
		NULL,
		(HINSTANCE)GetWindowLongPtr(mainWindow, GWLP_HINSTANCE),
		NULL
	);

	// Define margins
	int leftMargin = 1050;
	int topMargin = 10;

	// Create rotate button
	CreateButton(leftMargin, topMargin, "Reset Object", IDC_RESET_OBJECT);
}

/// <summary>
/// Handle dropped file
/// </summary>
/// <param name="dropInfo"></param>
void HandleDroppedFile(HDROP dropInfo) {

	// Get number of dropped files
	int numFiles = DragQueryFile(dropInfo, 0xFFFFFFFF, nullptr, 0);

	// Get file details if there's at least one, but we're 
	// really only interested in the first file
	if (numFiles > 0) {

		// Get filename
		LPWSTR draggedFilename[MAX_PATH];
		UINT numChars = DragQueryFile(dropInfo, 0, (LPWSTR)draggedFilename, MAX_PATH);
		if (numChars != 0) {

			// Convert pathname
			char mbFilename[MAX_PATH];
			UINT bytesWritten = WideCharToMultiByte(CP_ACP, WC_COMPOSITECHECK | WC_DEFAULTCHAR, (LPWSTR)draggedFilename, -1, mbFilename, MAX_PATH, NULL, NULL);
		
			// Load the object file
			string objectPathname = string((char*)mbFilename, numChars);
			if (renderer.LoadObject(objectPathname)) {

				// Object loaded
				_objectLoaded = true;
			}
		}
	}
}

/// <summary>
/// Handle mouse dragging on render window
/// </summary>
/// <param name="x">Mouse x coordinate</param>
/// <param name="y">Mouse y coordinate</param>
void HandleMouseDragging(HWND hwnd, long x, long y) {

	float yaw = 0;
	float pitch = 0;

	// Assemble point
	POINT dragPoint = POINT({ x, y });

	// Only accept dragging within render window
	if (!PtInRect(&renderWindowRect, dragPoint)) return;

	// Save drag origin point if dragging has just started
	if (!_isDragging) {
		_dragOrigin = dragPoint;
		_isDragging = true;
	}

	// Apply rotation while dragging
	if (_isDragging) {

		// Compute drag vector
		yaw = (float)(_dragOrigin.x - dragPoint.x);
		pitch = -(float)(_dragOrigin.y - dragPoint.y);

		// Scale rotation
		yaw *= 0.01f;
		pitch *= 0.01f;

		// Apply rotation
		renderer.Rotate(yaw, pitch);

		// Reset origin to apply incremental rotation
		_dragOrigin = dragPoint;
	}
}

/// <summary>
/// Handle mouse wheel scrolling
/// </summary>
/// <param name="wheelDelta">Scroll direction</param>
void HandleMouseWheel(short wheelDelta) {
	renderer.AdjustViewDistance(wheelDelta);
}

/// <summary>
/// Print debug message
/// </summary>
/// <param name="format">Format string</param>
/// <param name="">parameters</param>
void PrintMessage(const wchar_t* format, ...) {

	wchar_t message[4096];
	va_list args;
	int numCharsWritten;
	int maxMessage = sizeof(message) - 3;

	// Write formatted output to message string for all arguments
	va_start(args, format);
	numCharsWritten = _vsnwprintf_s(message, maxMessage, format, args);
	va_end(args);

	// Output the message to console
	OutputDebugString(message);
}
