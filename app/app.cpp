//app.cpp: 定义应用程序的入口点。
//

#include "stdafx.h"
#include "app.h"


#define MAX_LOADSTRING 100

// 全局变量: 
HINSTANCE hInst;                                // 当前实例
WCHAR szTitle[MAX_LOADSTRING];                  // 标题栏文本
WCHAR szWindowClass[MAX_LOADSTRING];            // 主窗口类名

// 此代码模块中包含的函数的前向声明: 
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

HINSTANCE ghInstance = 0;

#define WINDOW_CLASS_NAME L"sciter-plain-window"			// the main window class name

//
//  FUNCTION: window::init()
//
//  PURPOSE: Registers the window class.
//
bool window::init_class()
{
	static ATOM cls = 0;
	if (cls)
		return true;

	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = wnd_proc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = ghInstance;
	wcex.hIcon = LoadIcon(ghInstance, MAKEINTRESOURCE(IDI_PLAINWIN));
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = 0;//MAKEINTRESOURCE(IDC_PLAINWIN);
	wcex.lpszClassName = WINDOW_CLASS_NAME;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	cls = RegisterClassEx(&wcex);
	return cls != 0;
}

LRESULT CALLBACK window::wnd_proc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (message == WM_KEYDOWN && wParam == VK_F5)
	{
		window* self = ptr(hWnd);
		self->load_file(L"res:default.htm");
		return 0;
	}
	else if (message == WM_KEYDOWN && wParam == VK_F2)
	{
		// testing sciter::host<window>::call_function() and so SciterCall ()
		window* self = ptr(hWnd);
		sciter::dom::element root = self->get_root();

		sciter::value r;
		try {
			r = root.call_function("Test.add", sciter::value(2), sciter::value(2));
		}
		catch (sciter::script_error& err) {
			std::cerr << err.what() << std::endl;
		}
		//or sciter::value r = self->call_function("Test.add",sciter::value(2),sciter::value(2));
		assert(r.is_int() && r.get(0) == 4);
		return 0;
	}
	else if (message == WM_KEYDOWN && wParam == VK_F9)
	{
		window* self = ptr(hWnd);
		//self->load_file(L"http://terrainformatica.com/tests/test.htm");
		self->load_file(L"file://C:/sciter/sciter/sdk/demos/sciter/res/default.htm");
		return 0;
	}
	else if (message == WM_KEYDOWN && wParam == VK_F8)
	{
		window* self = ptr(hWnd);
		sciter::dom::element root = self->get_root();
		sciter::dom::element el_time = root.find_first("input#time");
		ULARGE_INTEGER ft; GetSystemTimeAsFileTime((FILETIME*)&ft);
		el_time.set_value(sciter::value::date(ft.QuadPart));

		return 0;
	}
	if (message == WM_KEYDOWN && wParam == VK_F1)
	{
		window* self = ptr(hWnd);
		sciter::dom::element root = self->get_root();
		sciter::dom::element first = root.find_first("#first");
		sciter::dom::element second = root.find_first("#second");

		auto fw = first.get_style_attribute("width");
		auto sw = second.get_style_attribute("width");

		return 0;
	}



	//date_time::DT_UTC | date_time::DT_HAS_DATE | date_time::DT_HAS_TIME | date_time::DT_HAS_SECONDS


	//SCITER integration starts
	BOOL handled = FALSE;
	LRESULT lr = SciterProcND(hWnd, message, wParam, lParam, &handled);
	if (handled)
		return lr;
	//SCITER integration ends

	window* self = ptr(hWnd);

	switch (message)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	}
	return DefWindowProc(hWnd, message, wParam, lParam);
}


window::window()
{
	init_class();
	_hwnd = CreateWindow(WINDOW_CLASS_NAME, L"demo", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, ghInstance, this);
	if (!_hwnd)
		return;

	init();

	ShowWindow(_hwnd, SW_SHOW);
	UpdateWindow(_hwnd);
}

window* window::ptr(HWND hwnd)
{
	return reinterpret_cast<window*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
}

bool window::init()
{
	SetWindowLongPtr(_hwnd, GWLP_USERDATA, LONG_PTR(this));
	setup_callback();
	load_file(L"res:default.htm");
	return true;
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: 在此放置代码。
	ghInstance = hInstance;
    // 初始化全局字符串
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_APP, szWindowClass, MAX_LOADSTRING);

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_APP));

    MSG msg;

	window wnd;

	if (!wnd.is_valid())
		return FALSE;

    // 主消息循环: 
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}

//
//  函数: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  目的:    处理主窗口的消息。
//
//  WM_COMMAND  - 处理应用程序菜单
//  WM_PAINT    - 绘制主窗口
//  WM_DESTROY  - 发送退出消息并返回
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // 分析菜单选择: 
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: 在此处添加使用 hdc 的任何绘图代码...
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// “关于”框的消息处理程序。
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
