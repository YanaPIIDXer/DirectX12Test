#include <Windows.h>

LRESULT WindowProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_  HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd)
{
	const LPCWSTR className = L"DirectX12Test";

	WNDCLASSEX w = {};
	w.cbSize = sizeof(WNDCLASSEX);
	w.lpfnWndProc = (WNDPROC)WindowProc;
	w.lpszClassName = className;
	w.hInstance = GetModuleHandle(nullptr);
	RegisterClassEx(&w);

	RECT wrc = { 0, 0, 640, 480 };
	AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false);

	HWND hWnd = CreateWindow(className, className, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, wrc.right - wrc.left, wrc.bottom - wrc.top, nullptr, nullptr, w.hInstance, nullptr);
	ShowWindow(hWnd, SW_SHOW);

	MSG msg = {};
	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	UnregisterClass(className, w.hInstance);
	return 0;
}

LRESULT WindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
		case WM_DESTROY:

			PostQuitMessage(0);
			break;
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}
