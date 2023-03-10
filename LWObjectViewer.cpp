// LWObjectViewer.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "LWObjectViewer.h"

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                        // current instance
WCHAR szTitle[MAX_LOADSTRING];          // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];    // the main window class name
HWND _mainWindow;						// Main window handle
HWND _renderWindow;						// Render output window handle
RECT _renderWindowRect;					// Render output window client rect

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
	if (!renderer.Initialize(_renderWindow, RENDER_WINDOW_WIDTH, RENDER_WINDOW_HEIGHT)) return 0;

	// Load object if specified on command line
	if (lstrcmpi(lpCmdLine, L"") != 0) {
		if (!LoadObject(lpCmdLine)) {

			// If not loaded correctly from the command line 
			// then quit immediately
			PostQuitMessage(0);
		}
	}

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
	wcex.hbrBackground = (HBRUSH)NULL;
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

	_mainWindow = CreateWindowW(
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

	if (!_mainWindow) {
		return FALSE;
	}

	ShowWindow(_mainWindow, nCmdShow);
	UpdateWindow(_mainWindow);

	// Register window as drop target
	DragAcceptFiles(_mainWindow, TRUE);

	// Populate main window controls
	CreateMainWindowControls();

	// Get render window rect
	GetClientRect(_renderWindow, &_renderWindowRect);

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
HWND CreateButton(int x, int y, int width, int height, std::string text, UINT id) {

	// Convert button text
	std::wstring buttonText = std::wstring(text.begin(), text.end());

	// Create button as child of main window
	return CreateWindowW(
		L"BUTTON",
		buttonText.c_str(),
		WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
		x, y, 
		width, height,
		_mainWindow,
		(HMENU)id,
		(HINSTANCE)GetWindowLongPtr(_mainWindow, GWLP_HINSTANCE),
		NULL
	);
}

/// <summary>
/// Create static field
/// </summary>
/// <param name="labelText">Label text</param>
HWND CreateField(HWND parent, int x, int y, LPCWSTR labelText) {

	SIZE textDimensions;
	GetTextExtentPoint32(GetDC(parent), labelText, lstrlen(labelText), &textDimensions);

	return CreateWindow(L"STATIC", labelText, WS_VISIBLE | WS_CHILD, x, y, textDimensions.cx, textDimensions.cy, parent, NULL, (HINSTANCE)GetWindowLongPtr(_mainWindow, GWLP_HINSTANCE), NULL);
}

/// <summary>
/// Create main window child controls
/// </summary>
void CreateMainWindowControls() {

	// Create child render frame
	_renderWindow = CreateWindowW(
		L"Static",
		NULL,
		WS_VISIBLE | WS_CHILD,
		10, 10, 
		RENDER_WINDOW_WIDTH, RENDER_WINDOW_HEIGHT,
		_mainWindow,
		NULL,
		(HINSTANCE)GetWindowLongPtr(_mainWindow, GWLP_HINSTANCE),
		NULL
	);

	// Define margins
	int leftMargin = 1050;
	int contentWidth = 220;
	int topMargin = 10;

	int boxLeftMargin = 15;
	int boxTopMargin = 25;

	// Create Object Information Box
	_infoBox = CreateWindow(L"BUTTON", L"", WS_VISIBLE | WS_CHILD | BS_GROUPBOX, leftMargin, topMargin, contentWidth, 200, _mainWindow, NULL, (HINSTANCE)GetWindowLongPtr(_mainWindow, GWLP_HINSTANCE), NULL);

	// Vertices
	int topOffset = 0;
	int fieldOffset = 110;
	CreateField(_infoBox, boxLeftMargin, boxTopMargin + topOffset, L"Vertices:");
	_infoVertices = CreateField(_infoBox, boxLeftMargin + fieldOffset, boxTopMargin + topOffset, L"0");

	// Triangles
	topOffset += 25;
	CreateField(_infoBox, boxLeftMargin, boxTopMargin + topOffset, L"Triangles:");
	_infoTriangles = CreateField(_infoBox, boxLeftMargin + fieldOffset, boxTopMargin + topOffset, L"0");

	// Non-Triangles
	topOffset += 25;
	CreateField(_infoBox, boxLeftMargin, boxTopMargin + topOffset, L"Non-Triangles:");
	_infoNonTriangles = CreateField(_infoBox, boxLeftMargin + fieldOffset, boxTopMargin + topOffset, L"0");

	// Layers
	topOffset += 25;
	CreateField(_infoBox, boxLeftMargin, boxTopMargin + topOffset, L"Layers:");
	_infoLayers = CreateField(_infoBox, boxLeftMargin + fieldOffset, boxTopMargin + topOffset, L"0");

	// Create reset button
	CreateButton(leftMargin, topMargin + 220, contentWidth, 40, "Reset Object", IDC_RESET_OBJECT);
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
			LoadObject((LPWSTR)draggedFilename);
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
	if (!PtInRect(&_renderWindowRect, dragPoint)) return;

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
/// Load object using filename
/// </summary>
/// <param name="pathname">Path and filename of object</param>
bool LoadObject(LPWSTR pathname) {

	// Convert pathname
	int numChars = lstrlen(pathname);
	char mbFilename[MAX_PATH];
	UINT bytesWritten = WideCharToMultiByte(CP_ACP, WC_COMPOSITECHECK | WC_DEFAULTCHAR, (LPWSTR)pathname, -1, mbFilename, MAX_PATH, NULL, NULL);
	string objectPathname = string((char*)mbFilename, numChars);

	// Strip leading/trailing quotes
	if (objectPathname.front() == '"' && objectPathname.back() == '"') {
		objectPathname = objectPathname.substr(1, numChars - 2);
	}

	// Load the object file
	wstring errorReason;
	if (renderer.LoadObject(objectPathname, errorReason)) {

		// Object loaded
		_objectLoaded = true;

		// Set object info
		_objectInfo = renderer.GetObjectInfo();
		SetFieldValue(_infoVertices, _objectInfo.numVertices);
		SetFieldValue(_infoTriangles, _objectInfo.numTriangles);
		SetFieldValue(_infoNonTriangles, _objectInfo.numNonTriangles);
		SetFieldValue(_infoLayers, _objectInfo.numLayers);

		return true;
	}
	else {

		// Error loeading the object
		MessageBox(_mainWindow, errorReason.c_str(), L"Couldn't Load the Object", MB_ICONERROR | MB_OK);
		return false;
	}
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

/// <summary>
/// Set value in field
/// </summary>
/// <param name="field">Window hanlde of field</param>
/// <param name="value">Numeric value</param>
void SetFieldValue(HWND field, int numericValue) {

	// Convert value to string
	std::wstring valueText = std::to_wstring(numericValue);

	// Set the field value
	SetWindowText(field, valueText.c_str());

	// Resize field
	SIZE textDimensions;
	GetTextExtentPoint32(GetDC(_infoBox), valueText.c_str(), valueText.size(), &textDimensions);
	SetWindowPos(field, NULL, 0, 0, textDimensions.cx, textDimensions.cy, SWP_NOMOVE);
}
