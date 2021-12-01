// MouseDrawTool.cpp : アプリケーションのエントリ ポイントを定義します。
//

#include "framework.h"
#include "MouseDrawTool.h"

#define MAX_LOADSTRING 100

// グローバル変数:
HINSTANCE hInst;                                // 現在のインターフェイス
WCHAR szTitle[MAX_LOADSTRING];                  // タイトル バーのテキスト
WCHAR szWindowClass[MAX_LOADSTRING];            // メイン ウィンドウ クラス名
POINTS start, end, old_end;


// このコード モジュールに含まれる関数の宣言を転送します:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
void DrawRect(HWND, POINTS, POINTS);


int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: ここにコードを挿入してください。

    // グローバル文字列を初期化する
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_MOUSEDRAWTOOL, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // アプリケーション初期化の実行:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_MOUSEDRAWTOOL));

    MSG msg;

    // メイン メッセージ ループ:
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
//  関数: MyRegisterClass()
//
//  目的: ウィンドウ クラスを登録します。
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MOUSEDRAWTOOL));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_MOUSEDRAWTOOL);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   関数: InitInstance(HINSTANCE, int)
//
//   目的: インスタンス ハンドルを保存して、メイン ウィンドウを作成します
//
//   コメント:
//
//        この関数で、グローバル変数でインスタンス ハンドルを保存し、
//        メイン プログラム ウィンドウを作成および表示します。
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // グローバル変数にインスタンス ハンドルを格納する

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

#include <commdlg.h>

// [ファイルを開く]ダイアログ
static VOID funcFileOpen(HWND hWnd)
{
    static OPENFILENAME     ofn;
    static TCHAR            szPath[MAX_PATH] = { 0 };
    static TCHAR            szFile[MAX_PATH] = { 0 };

    if (szPath[0] == _T('\0')) {
        GetCurrentDirectory(MAX_PATH, szPath);
    }
    if (ofn.lStructSize == 0) {
        ofn.lStructSize = sizeof(OPENFILENAME);
        ofn.hwndOwner = hWnd;
        ofn.lpstrInitialDir = szPath;       // 初期フォルダ位置
        ofn.lpstrFile = szFile;       // 選択ファイル格納
        ofn.nMaxFile = MAX_PATH;
        ofn.lpstrFilter = 
            _T("BMPファイル(*.bmp)\0*.bmp\0")
            _T("JPEGファイル(*.jpg,*.jpeg)\0*.jpg\0")
            _T("PNGファイル(*.png)\0*.png\0")
            _T("すべてのファイル(*.*)\0*.*\0");
        ofn.lpstrTitle = _T("音楽ファイルを選択します。");
        ofn.Flags = OFN_FILEMUSTEXIST;
    }
    if (GetOpenFileName(&ofn)) {
        MessageBox(hWnd, szFile, _T("ファイルを開く"), MB_OK);
        //TODO: ここにファイルを開いた後の処理を行う。大抵は取得ファイルパスの取得か。
    }
}

//
//  関数: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  目的: メイン ウィンドウのメッセージを処理します。
//
//  WM_COMMAND  - アプリケーション メニューの処理
//  WM_PAINT    - メイン ウィンドウを描画する
//  WM_DESTROY  - 中止メッセージを表示して戻る
//  WM_CLOSE    - アプリケーションが終了するシグナル
//  WM_LBUTTONDOWN  - マウス左ボタン押下時の処理
//  WM_MOUSEMOVE    - マウス移動時の処理
//  WM_LBUTTONUP    - マウス左ボタン話した時の処理
//  WM_CLOSE    - アプリケーションが終了するシグナル
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    int id;
    TCHAR szBuf[32];
    HDC hdc;
    HBRUSH hBrush;
    static BOOL bDraw;

    switch (message)
    {
    case WM_LBUTTONDOWN:
        bDraw = TRUE;
        old_end = start = MAKEPOINTS(lParam);
        DrawRect(hWnd, start, old_end);
        break;
    case WM_MOUSEMOVE:
        if (bDraw) {
            end = MAKEPOINTS(lParam);
            DrawRect(hWnd, start, old_end);
            DrawRect(hWnd, start, end);
            old_end = end;
        }
        else {
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
        break;
    case WM_LBUTTONUP:
        if (bDraw) {
            //    SetCursor(LoadCursor(NULL, IDC_ARROW));
            DrawRect(hWnd, start, end);
            bDraw = FALSE;

            _stprintf_s(szBuf, _countof(szBuf)-1, 
                _T("(%d, %d) - (%d, %d)"),
                start.x, start.y, end.x, end.y);
            SetWindowText(hWnd, szBuf);
            hdc = GetDC(hWnd);
            hBrush = (HBRUSH)GetStockObject(NULL_BRUSH);
            SelectObject(hdc, hBrush);
            Ellipse(hdc, start.x, start.y, end.x, end.y);
            ReleaseDC(hWnd, hdc);
        }
        else {
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
        break;
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // 選択されたメニューの解析:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                //DestroyWindow(hWnd);
                PostMessage(hWnd, WM_CLOSE,0,0);  
                break;
            case IDM_FILE_OPEN:
                //ファイルを開く
                funcFileOpen(hWnd);
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: HDC を使用する描画コードをここに追加してください...
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_CLOSE:
        id = MessageBox(hWnd,
            _T("終了してもよろしいですか"),
            _T("確認"),
            MB_YESNO | MB_ICONQUESTION);
        if (id == IDYES)
            DestroyWindow(hWnd);
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// バージョン情報ボックスのメッセージ ハンドラーです。
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

void DrawRect(HWND hWnd, POINTS beg, POINTS end)
{
    HDC hdc;

    hdc = GetDC(hWnd);
    SetROP2(hdc, R2_NOT);
    MoveToEx(hdc, beg.x, beg.y, NULL);
    LineTo(hdc, end.x, beg.y);
    LineTo(hdc, end.x, end.y);
    LineTo(hdc, beg.x, end.y);
    LineTo(hdc, beg.x, beg.y);
    LineTo(hdc, end.x, end.y);
    ReleaseDC(hWnd, hdc);
    return;
}
