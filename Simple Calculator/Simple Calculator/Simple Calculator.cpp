#include <windows.h>
#include <string>
#include <dwmapi.h>
#pragma comment(lib, "dwmapi.lib")

#define BG_COLOR RGB(25,25,28)
#define BTN_COLOR RGB(40,40,44)
#define TXT_COLOR RGB(230,230,230)
#define ACCENT RGB(0,120,215)

HWND hEdit;
HFONT hFont;
std::wstring current;
double first = 0;
wchar_t op = 0;

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {

    switch (msg) {

    case WM_CREATE: {
        BOOL dark = TRUE;
        DwmSetWindowAttribute(hwnd, 20, &dark, sizeof(dark)); // Dark title bar

        hFont = CreateFontW(
            20, 0, 0, 0, FW_MEDIUM,
            FALSE, FALSE, FALSE,
            DEFAULT_CHARSET,
            OUT_OUTLINE_PRECIS,
            CLIP_DEFAULT_PRECIS,
            CLEARTYPE_QUALITY,
            VARIABLE_PITCH,
            L"Segoe UI"
        );

        hEdit = CreateWindowW(
            L"STATIC", L"",
            WS_CHILD | WS_VISIBLE | SS_RIGHT,
            15, 15, 260, 35,
            hwnd, NULL, NULL, NULL
        );

        SendMessageW(hEdit, WM_SETFONT, (WPARAM)hFont, TRUE);

        const wchar_t* btn[] = {
            L"7",L"8",L"9",L"/",
            L"4",L"5",L"6",L"*",
            L"1",L"2",L"3",L"-",
            L"0",L"C",L"=",L"+"
        };

        for (int i = 0; i < 16; i++) {
            HWND b = CreateWindowW(
                L"BUTTON", btn[i],
                WS_CHILD | WS_VISIBLE | BS_FLAT,
                15 + (i % 4) * 65,
                65 + (i / 4) * 55,
                60, 50,
                hwnd,
                (HMENU)(INT_PTR)(100 + i),
                NULL, NULL
            );
            SendMessageW(b, WM_SETFONT, (WPARAM)hFont, TRUE);
        }
        break;
    }

    case WM_CTLCOLORSTATIC:
    case WM_CTLCOLORBTN: {
        HDC hdc = (HDC)wParam;
        SetTextColor(hdc, TXT_COLOR);
        SetBkColor(hdc, BTN_COLOR);
        static HBRUSH brush = CreateSolidBrush(BTN_COLOR);
        return (LRESULT)brush;
    }

    case WM_ERASEBKGND:
        return 1;

    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        HBRUSH bg = CreateSolidBrush(BG_COLOR);
        FillRect(hdc, &ps.rcPaint, bg);
        DeleteObject(bg);
        EndPaint(hwnd, &ps);
        break;
    }

    case WM_COMMAND: {
        int id = LOWORD(wParam);
        if (id < 100 || id > 115) break;

        wchar_t t[2];
        GetWindowTextW((HWND)lParam, t, 2);

        if (t[0] >= L'0' && t[0] <= L'9') current += t[0];
        else if (t[0] == L'C') { current.clear(); first = 0; op = 0; }
        else if (t[0] == L'=') {
            double s = _wtof(current.c_str());
            if (op == L'+') first += s;
            if (op == L'-') first -= s;
            if (op == L'*') first *= s;
            if (op == L'/') first = (s != 0) ? first / s : 0;
            current = std::to_wstring(first);
            op = 0;
        }
        else { first = _wtof(current.c_str()); current.clear(); op = t[0]; }

        SetWindowTextW(hEdit, current.c_str());
        break;
    }

    case WM_DESTROY:
        DeleteObject(hFont);
        PostQuitMessage(0);
        break;
    }

    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE h, HINSTANCE, LPSTR, int n) {
    WNDCLASSW wc = {};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = h;
    wc.lpszClassName = L"Calc2026";

    RegisterClassW(&wc);

    HWND hwnd = CreateWindowW(
        L"Calc2026", L"Calculator 2026",
        WS_OVERLAPPED | WS_MINIMIZEBOX | WS_SYSMENU,
        600, 300, 310, 380,
        NULL, NULL, h, NULL
    );

    ShowWindow(hwnd, n);

    MSG msg;
    while (GetMessageW(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
    return 0;
}
