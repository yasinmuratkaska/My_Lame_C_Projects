//Compile: gcc CatchTheShit.c -o CatchTheShit -mwindows  -lgdi32 -luser32
//SPOILER: To exit -> Tab and enter

#define UNICODE
#define _UNICODE
#include <windows.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#ifndef GET_X_LPARAM
#define GET_X_LPARAM(lp) ((int)(short)LOWORD(lp))
#define GET_Y_LPARAM(lp) ((int)(short)HIWORD(lp))
#endif

//GLOBAL VARS
HWND g_mainWindow; 
HWND g_congratsWindow;
int g_emojiX, g_emojiY;
int g_emojiWidth=100, g_emojiHeight=100;
BOOL g_showHaha=FALSE; 
DWORD g_HahaStartTime; 
BOOL g_emojiFocus=FALSE;
int g_screenWidth, g_screenHeight;

//FUNC PROTOTYPES
LRESULT CALLBACK MainWindowProcedure(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam); 
LRESULT CALLBACK CongratsWindowProcedure(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam); 
void MoveEmojiRandomPlace();
BOOL IsMouseOnEmoji(int mouseX, int mouseY);
void DrawEmoji(HDC hdc, int x, int y);
void DrawHahaString(HDC hdc);
void CreateCongratsWindow();

//STARTING POINT
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow){
//Telling to windows our window properties
    WNDCLASSEX wc={0};
    wc.cbSize=sizeof(WNDCLASSEX);
    wc.style=CS_HREDRAW|CS_VREDRAW;
    wc.lpfnWndProc=MainWindowProcedure;
    wc.hInstance=hInstance;
    wc.hCursor=LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground=(HBRUSH)GetStockObject(BLACK_BRUSH);
    wc.lpszClassName=L"ShitEmojiClass";

    //SAVE WINDOW CLASS TO OS
    if(!RegisterClassEx(&wc)){
        MessageBox(NULL, L"Window Class Can't Be Saved!", L"ERROR", MB_ICONERROR);
        return 1;
    }

    //TAKE SCREEN MEASURES
    g_screenWidth=GetSystemMetrics(SM_CXSCREEN);
    g_screenHeight=GetSystemMetrics(SM_CYSCREEN);

    //CREATE MAIN WINDOW - BORDERLESS FULL
    g_mainWindow=CreateWindowEx(
        WS_EX_TOPMOST,
        L"ShitEmojiClass",
        L"Shit Escape Game",
        WS_POPUP|WS_VISIBLE,
        0,0,
        g_screenWidth, g_screenHeight,
        NULL,NULL,hInstance,NULL
    ) ;
    if (g_mainWindow==NULL){
        MessageBox(NULL, L"The Windows Can't Be Created!",L"ERROR",MB_ICONERROR);
        return 1;
    }

    //RANDOM NUMBER GENERATOR - FOR EMOJI LOCATIONS
    srand((unsigned int)time(NULL));

    //PLACE EMOJI TO STARTINT POINT
    g_emojiX=(g_screenWidth-g_screenWidth)/2;
    g_emojiY=(g_screenHeight-g_screenHeight)/2;

    //SHOW WINDOW
    ShowWindow(g_mainWindow, SW_SHOWMAXIMIZED);
    UpdateWindow(g_mainWindow);

    //MSG LOOP
    MSG msg;
    while (GetMessage(&msg,NULL,0,0)){
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return (int)msg.wParam;
}

//MAIN WINDOW MSG PROCESS FUNC
LRESULT CALLBACK MainWindowProcedure(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam){
    switch (msg){
        // ITS CALLED WHEN FIRST WINDOW CREATED
        case WM_CREATE:{
            //ADD TAB BUTTON FOCUS
            SetWindowLong(hwnd, GWL_STYLE,GetWindowLong(hwnd, GWL_STYLE)|WS_TABSTOP);
            return 0;
        }
        case WM_PAINT:{
            PAINTSTRUCT ps;
            HDC hdc=BeginPaint(hwnd,&ps);
            //CHECK HAHAHA THEN DRAW
            if (g_showHaha){
                //500 MILISEC CHECK
                if(GetTickCount() - g_HahaStartTime>500){
                    g_showHaha=FALSE;
                }
                else{
                    DrawHahaString(hdc);
                }
            }
            //DRAW EMOJI
            DrawEmoji(hdc,g_emojiX,g_emojiY);

            //DRAW ORDER OF CLICK ON THE SHIT TO QUIT 
            SetTextColor(hdc, RGB(255,255,255));
            SetBkColor(hdc, RGB(0,0,0));
            
            RECT wordRect;
            wordRect.left=g_emojiX - 80;
            wordRect.top=g_emojiY - 40;
            wordRect.right=g_emojiX +180;
            wordRect.bottom=g_emojiY - 10;

            DrawText(hdc, L"Click On Shit to Quit", -1, &wordRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

            //IF EMOJI IS FOCUSED, DRAW FOCUS FRAME
            if (g_emojiFocus){
                HPEN redPencil=CreatePen(PS_SOLID, 3, RGB(255,0,0));
                HPEN oldPEncil=(HPEN)SelectObject(hdc, redPencil);
            
                //DRAW RECT AROUND EMOJI
                Rectangle(hdc, g_emojiX -5,g_emojiY -5, g_emojiX + g_emojiWidth +5, g_emojiY + g_emojiHeight+5);
                SelectObject(hdc, oldPEncil);
                DeleteObject(redPencil);

            }

            EndPaint(hwnd, &ps);
            return 0;



        }
        //WHEN MOUSE MOVE
        case WM_MOUSEMOVE:{
            //GET MOUSE LOCATION
            int mouseX=GET_X_LPARAM(lParam);
            int mouseY=GET_Y_LPARAM(lParam);

            //IF MOUSE ON EMOJI
            if (IsMouseOnEmoji(mouseX, mouseY)){
                //REPLACE EMOJI
                MoveEmojiRandomPlace();

                //SHOW HAHAHA TEXT AND SAVE TIME
                g_showHaha=TRUE;
                g_HahaStartTime=GetTickCount();

                //RE-DRAW THE WINDOWS
                InvalidateRect(hwnd, NULL, TRUE);
                UpdateWindow(hwnd);
            }
            return 0;
            
        }

        //WM_KEYDOWN: WHEN PRESSED A KEY
        case WM_KEYDOWN:{
            switch(wParam){
                case VK_TAB:
                    g_emojiFocus=TRUE;
                    InvalidateRect(hwnd, NULL, TRUE);
                    return 0;
                case VK_RETURN:
                    if (g_emojiFocus){
                        CreateCongratsWindow();
                        DestroyWindow(hwnd);
                    }
                    return 0;

                case VK_ESCAPE:
                    return 0;
                case VK_F4:
                    if(GetAsyncKeyState(VK_MENU)&0x8000){
                        return 0;
                    }
                    break;
            }
            break;
        }

        //WM_SYSCOMMAND - SYSCOMMANDS
        case WM_SYSCOMMAND:{
            if(wParam == SC_CLOSE || wParam == SC_MINIMIZE || wParam == SC_MAXIMIZE){
                return 0;
            }
            break;
        }

        //WM_DESTORY: WHEN WINDOW IS CLOSED
        case WM_DESTROY:{
            PostQuitMessage(0);
            return 0;
        }


    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

LRESULT CALLBACK CongratsWindowProcedure(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg){
        case WM_CREATE:{
            HWND hButton=CreateWindow(
            L"BUTTON",
            L"CONGRATS!",
            WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
            (g_screenWidth-200)/2,
            (g_screenHeight-80)/2,
            200,80,
            hwnd,
            (HMENU)1,
            ((LPCREATESTRUCT)lParam)->hInstance,
            NULL
        );
    
        //MAKE BUTTON FONT BIGGEr AND MAKE IT BOLD
        HFONT hFont = CreateFont(28,0,0,0, FW_BOLD, FALSE, FALSE, FALSE,
                            DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
                            CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
                            DEFAULT_PITCH | FF_DONTCARE, L"Arial");
        SendMessage(hButton, WM_SETFONT, (WPARAM)hFont, TRUE);

        return 0;
    }

    case WM_COMMAND:{
        //WHEN THE BUTTON IS CLICKED
        if (LOWORD(wParam)==1){
            DestroyWindow(hwnd);
            PostQuitMessage(0);
        }
        return 0;
    }

    case WM_DESTROY:{
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}
}
//MOVE EMOJI RANDOM PLACE
void MoveEmojiRandomPlace(){
    int tryingCount=0;
    int newX, newY;

    do{
        newX = rand()%(g_screenWidth - g_emojiWidth);
        newY=rand()%(g_screenHeight - g_emojiWidth);
        tryingCount++;
        
    }while((abs(newX-g_emojiX)<200 && abs(newY-g_emojiY)<200)&&tryingCount>10);

    g_emojiX=newX;
    g_emojiY=newY;
}
//CHECK IF MOUSE ON EMOJI
BOOL IsMouseOnEmoji(int mouseX, int mouseY){
    return(mouseX>=g_emojiX && mouseX<=g_emojiX+g_emojiWidth && mouseY>=g_emojiY && mouseY<= g_emojiY+g_emojiHeight);
}
//DRAW EMOJI OF SHIT
void DrawEmoji(HDC hdc, int x, int y){
    //BODY OF SHIT
    HBRUSH brownBrush=CreateSolidBrush(RGB(139,69,19));
    HBRUSH oldBrush=(HBRUSH)SelectObject(hdc, brownBrush);
    Ellipse(hdc,x,y,x+g_emojiWidth, y+g_emojiHeight);
    //3D EFFECT
    HBRUSH lightbrownBrush=CreateSolidBrush(RGB(160,82,45));
    SelectObject(hdc, lightbrownBrush);
    Ellipse(hdc,x+10,y+10,x+g_emojiWidth-10, y+30);
    //EYES
    HBRUSH whiteBrush = CreateSolidBrush(RGB(255,255,255));
    SelectObject(hdc, whiteBrush);
    Ellipse(hdc, x+25, y+35,x+40,y+50);//LEFT EYE
    Ellipse(hdc, x+60, y+35,x+75,y+50);//RIGHT EYE
    //MOUTH
    HPEN redPencil=CreatePen(PS_SOLID,2,RGB(255,0,0));
    HPEN oldPencil=(HPEN)SelectObject(hdc,redPencil);
    Arc(hdc,x+30,y+50,x+70,y+80,x+30,y+65,x+70,y+65);
    //LOAD OLD OBJECTS AND CLEAN THE CREATED ONES
    SelectObject(hdc,oldBrush);
    SelectObject(hdc,oldPencil);
    DeleteObject(brownBrush);
    DeleteObject(lightbrownBrush);
    DeleteObject(whiteBrush);
    DeleteObject(redPencil);
}

//DRAW HAHAHA AT RANDOM LOCATIONS
void DrawHahaString(HDC hdc){
    for(int i=0;i<5;i++){
        int randomX=rand()%(g_screenWidth-100);
        int randomY=rand()%(g_screenHeight-30);

        SetTextColor(hdc,RGB(rand()%256,rand()%256,rand()%256));
        SetBkColor(hdc,RGB(0,0,0));

        TextOut(hdc,randomX,randomY,L"HAHAHA!!!",9);

    }

}

//CREATE CONGRATS WINDOWS
void CreateCongratsWindow(){
    WNDCLASSEX wc={0};
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = CongratsWindowProcedure;
    wc.hInstance = GetModuleHandle(NULL);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)GetStockObject(LTGRAY_BRUSH);
    wc.lpszClassName = L"CongratsWindowClass";
    RegisterClassEx(&wc);

    g_congratsWindow=CreateWindowEx(
        WS_EX_TOPMOST,
        L"CongratsWindowClass",
        L"Congrats!",
        WS_POPUP | WS_VISIBLE,
        (g_screenWidth - 400) / 2,  
        (g_screenHeight- 200) / 2, 
        400, 200,
        NULL, NULL, GetModuleHandle(NULL), NULL
    );

    //SHOW WINDOW
    ShowWindow(g_congratsWindow,SW_SHOW);
    UpdateWindow(g_congratsWindow);

}
