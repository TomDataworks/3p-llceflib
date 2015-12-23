/**
* @brief Windows test app for LLCefLib
* @author Callum Prentice 2015
*
* $LicenseInfo:firstyear=2001&license=viewerlgpl$
* Second Life Viewer Source Code
* Copyright (C) 2015, Linden Research, Inc.
*
* This library is free software; you can redistribute it and/or
* modify it under the terms of the GNU Lesser General Public
* License as published by the Free Software Foundation;
* version 2.1 of the License only.
*
* This library is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
* Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public
* License along with this library; if not, write to the Free Software
* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*
* Linden Research, Inc., 945 Battery Street, San Francisco, CA  94111  USA
* $/LicenseInfo$
*/

#include <windows.h>
#include <string>
#include <iostream>

#include "resource.h"

#include <functional>

#include <gl/gl.h>
#include "../../llcefLib.h"

using namespace std::placeholders;

int mAppWindowWidth = 1024;
int mAppWindowHeight = 1024;
const int gTextureWidth = 1024;
const int gTextureHeight = 1024;
const int gTextureDepth = 4;
WPARAM gWParam = 0;
LPARAM gLParam = 0L;
int gcharcode = 0;
LLCEFLib* mLLCEFLib;
unsigned char pixels[gTextureWidth * gTextureHeight * gTextureDepth];
GLuint texture_handle = 0;
const std::string gHomePage("https://callum-linden.s3.amazonaws.com/ceftests.html");
const std::string gCefAbout("chrome://about");

/////////////////////////////////////////////////////////////////////////////////
//
void resize_gl_screen(int width, int height)
{
    if (height == 0)
    {
        height = 1;
    }

    mAppWindowWidth = width;
    mAppWindowHeight = height;

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    glViewport(0, 0, width, height);
    glOrtho(0.0f, width, height, 0.0f, -1.0f, 1.0f);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void onPageChangedCallback(unsigned char* pixels, int x, int y, int width, int height, bool is_popup)
{
    glTexSubImage2D(GL_TEXTURE_2D, 0,
                    x, gTextureHeight - y - height,
                    width, height,
                    GL_BGRA_EXT,
                    GL_UNSIGNED_BYTE,
                    pixels);
}

void onNavigateURL(std::string url, std::string target)
{
	mLLCEFLib->navigate(url);
}


void onFileDownload(std::string filename)
{
	MessageBoxA(0, filename.c_str(), "File download", 0);
}

void onRequestExitCallback()
{
    PostQuitMessage(0);
}

/////////////////////////////////////////////////////////////////////////////////
//
void init(HWND hWnd)
{
    mLLCEFLib = new LLCEFLib();

    mLLCEFLib->setOnPageChangedCallback(std::bind(onPageChangedCallback, _1, _2, _3, _4, _5, _6));
	mLLCEFLib->setOnNavigateURLCallback(std::bind(onNavigateURL, _1, _2));
    mLLCEFLib->setOnRequestExitCallback(std::bind(onRequestExitCallback));
	mLLCEFLib->setOnFileDownloadCallback(std::bind(onFileDownload, _1));

    LLCEFLib::LLCEFLibSettings settings;
    settings.initial_width = gTextureWidth;
    settings.initial_height = gTextureHeight;
    settings.javascript_enabled = true;
    settings.cookies_enabled = true;
    settings.plugins_enabled = true;
    settings.cookie_store_path = "c:\\win32gl-cef-cookies";
    settings.user_agent_substring = mLLCEFLib->makeCompatibleUserAgentString("Win32GL");
    settings.accept_language_list = "en-us";
    settings.debug_output = false;

    bool result = mLLCEFLib->init(settings);
    if (result)
    {
        mLLCEFLib->navigate(gHomePage);
    }
}

/////////////////////////////////////////////////////////////////////////////////
//
void update()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // TODO - move
    glLoadIdentity();

    glEnable(GL_TEXTURE_2D);
    glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_QUADS);
    glTexCoord2f(1.0f, 1.0f);
    glVertex2d(mAppWindowWidth, 0);

    glTexCoord2f(0.0f, 1.0f);
    glVertex2d(0, 0);

    glTexCoord2f(0.0f, 0.0f);
    glVertex2d(0, mAppWindowHeight);

    glTexCoord2f(1.0f, 0.0f);
    glVertex2d(mAppWindowWidth, mAppWindowHeight);
    glEnd();

    mLLCEFLib->update();
}

/////////////////////////////////////////////////////////////////////////////////
//
void keyboard_event(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    mLLCEFLib->nativeKeyboardEvent(uMsg, wParam, lParam);
}

/////////////////////////////////////////////////////////////////////////////////
//
LRESULT CALLBACK window_proc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    static int last_char_code = 0;
    int wmId;
    int wmEvent;

    switch (uMsg)
    {
        case WM_COMMAND:
            wmId = LOWORD(wParam);
            wmEvent = HIWORD(wParam);
            switch (wmId)
            {
                case ID_TESTS_REQUESTEXIT:
                    mLLCEFLib->requestExit();
                    break;

                case ID_TESTS_NAVIGATEHOME:
                    mLLCEFLib->navigate(gHomePage);
                    break;

                case ID_TESTS_SETACOOKIE:
                    mLLCEFLib->setCookie("http://callum.com", "cookie_name", "cookie_value", ".callum.com", "/", true, true);
                    break;

                case ID_TESTS_OPENDEVELOPERCONSOLE:
                    mLLCEFLib->showDevTools(true);
                    break;

                case ID_TESTS_ABOUTCEF:
                    mLLCEFLib->navigate(gCefAbout);
                    break;

                default:
                    return DefWindowProc(hWnd, uMsg, wParam, lParam);
            }
            break;

		case WM_LBUTTONDBLCLK:
		{
			int x = (LOWORD(lParam) * gTextureWidth) / mAppWindowWidth;
			int y = (HIWORD(lParam) * gTextureHeight) / mAppWindowHeight;
			mLLCEFLib->mouseButton(LLCEFLib::MB_MOUSE_BUTTON_LEFT, LLCEFLib::ME_MOUSE_DOUBLE_CLICK, x, y);
			return 0;
		}

        case WM_LBUTTONDOWN:
        {
            int x = (LOWORD(lParam) * gTextureWidth) / mAppWindowWidth;
            int y = (HIWORD(lParam) * gTextureHeight) / mAppWindowHeight;
            mLLCEFLib->mouseButton(LLCEFLib::MB_MOUSE_BUTTON_LEFT, LLCEFLib::ME_MOUSE_DOWN, x, y);
            mLLCEFLib->setFocus(true);
            return 0;
        };

        case WM_LBUTTONUP:
        {
            int x = (LOWORD(lParam) * gTextureWidth) / mAppWindowWidth;
            int y = (HIWORD(lParam) * gTextureHeight) / mAppWindowHeight;
            mLLCEFLib->mouseButton(LLCEFLib::MB_MOUSE_BUTTON_LEFT, LLCEFLib::ME_MOUSE_UP, x, y);
            return 0;
        };

        case WM_RBUTTONDOWN:
        {
            int x = (LOWORD(lParam) * gTextureWidth) / mAppWindowWidth;
            int y = (HIWORD(lParam) * gTextureHeight) / mAppWindowHeight;
            mLLCEFLib->mouseButton(LLCEFLib::MB_MOUSE_BUTTON_RIGHT, LLCEFLib::ME_MOUSE_DOWN, x, y);
            return 0;
        }

        case WM_RBUTTONUP:
        {
            int x = (LOWORD(lParam) * gTextureWidth) / mAppWindowWidth;
            int y = (HIWORD(lParam) * gTextureHeight) / mAppWindowHeight;
            mLLCEFLib->mouseButton(LLCEFLib::MB_MOUSE_BUTTON_RIGHT, LLCEFLib::ME_MOUSE_UP, x, y);
            return 0;
        };

        case WM_MOUSEMOVE:
        {
            int x = (LOWORD(lParam) * gTextureWidth) / mAppWindowWidth;
            int y = (HIWORD(lParam) * gTextureHeight) / mAppWindowHeight;
            mLLCEFLib->mouseMove(x, y);
            return 0;
        };

        case WM_CHAR:
        {
            keyboard_event(uMsg, wParam, lParam);
            return 0;
        };

        case WM_KEYDOWN:
        {
            keyboard_event(uMsg, wParam, lParam);
            return 0;
        };

        case WM_KEYUP:
        {
            keyboard_event(uMsg, wParam, lParam);
            return 0;
        };

        case WM_SIZE:
        {
            int new_width = LOWORD(lParam);
            int new_height = HIWORD(lParam);
            resize_gl_screen(new_width, new_height);
            return 0;
        };

        case WM_CLOSE:
        {
            mLLCEFLib->requestExit();
            return 0;
        };
    };

    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

/////////////////////////////////////////////////////////////////////////////////
//
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    WNDCLASS wc;
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC | CS_DBLCLKS;
    wc.lpfnWndProc = (WNDPROC)window_proc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = NULL;
    wc.lpszMenuName = MAKEINTRESOURCE(IDR_MENU1);
    wc.lpszClassName = "Win32GL";
    RegisterClass(&wc);

    RECT window_rect;
    SetRect(&window_rect, 0, 0, mAppWindowWidth, mAppWindowHeight);

    DWORD ex_style = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
    DWORD style = WS_OVERLAPPEDWINDOW;
    AdjustWindowRectEx(&window_rect, style, FALSE, ex_style);

    HWND hWnd = CreateWindowEx(ex_style, "Win32GL", "Win32GL LLCEFLib test", style | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
                               80, 0, window_rect.right - window_rect.left, window_rect.bottom - window_rect.top,
                               NULL, NULL, hInstance, NULL);

    static  PIXELFORMATDESCRIPTOR pfd =
    {
        sizeof(PIXELFORMATDESCRIPTOR),
        1,
        PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER, PFD_TYPE_RGBA,
        32, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 16, 0, 0, PFD_MAIN_PLANE, 0, 0, 0, 0
    };

    HDC hDC = GetDC(hWnd);

    GLuint pixel_format = ChoosePixelFormat(hDC, &pfd);
    SetPixelFormat(hDC, pixel_format, &pfd);
    HGLRC hRC = wglCreateContext(hDC);
    wglMakeCurrent(hDC, hRC);

    ShowWindow(hWnd, SW_SHOW);
    SetForegroundWindow(hWnd);
    SetFocus(hWnd);

    resize_gl_screen(mAppWindowWidth, mAppWindowHeight);

    glGenTextures(1, &texture_handle);

    glBindTexture(GL_TEXTURE_2D, texture_handle);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, gTextureWidth, gTextureHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    init(hWnd);

    bool done = false;
    while (!done)
    {
        MSG msg;
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            if (WM_QUIT == msg.message)
            {
                done = true;
            }
            else
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            };
        }
        else
        {
            update();
            SwapBuffers(hDC);
        };
    };

    mLLCEFLib->shutdown();

    wglMakeCurrent(NULL, NULL);
    wglDeleteContext(hRC);
    ReleaseDC(hWnd, hDC);
    DestroyWindow(hWnd);
    UnregisterClass("Win32GL", hInstance);

    return 0;
}
