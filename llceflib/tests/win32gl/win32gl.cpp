/**
* @brief Windows test app for LLCefLib
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

#include <gl/gl.h>
#include "../../llceflib.h"

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

/////////////////////////////////////////////////////////////////////////////////
//
void resize_gl_screen( int width, int height )
{
    if ( height == 0 )
        height = 1;

    mAppWindowWidth = width;
    mAppWindowHeight = height;

    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();

    glViewport( 0, 0, width, height );
    glOrtho( 0.0f, width, height, 0.0f, -1.0f, 1.0f );

    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();
}

void pageChangedCallback(unsigned char* pixels, int width, int height)
{
	glTexSubImage2D(GL_TEXTURE_2D, 0,
		0, 0,
		width, height,
		GL_BGRA_EXT,
		GL_UNSIGNED_BYTE,
		pixels);
}

/////////////////////////////////////////////////////////////////////////////////
//
void init( HWND hWnd )
{
	mLLCEFLib = new LLCEFLib();

	mLLCEFLib->setPageChangedCallback(std::bind(pageChangedCallback, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));

	LLCEFLibSettings settings;
	settings.inital_width = gTextureWidth;
	settings.inital_height = gTextureHeight;
	settings.javascript_enabled = true;
	settings.cookies_enabled = true;
	settings.user_agent_substring = "Win32GL Test";
	settings.accept_language_list = "en-us";

	bool result = mLLCEFLib->init(settings);
	if(result)
	{
		//mLLCEFLib->navigate("https://callum-linden.s3.amazonaws.com/cookie_test.html");
		//mLLCEFLib->navigate("https://secondlife.com");
		mLLCEFLib->navigate("google.com");
	}
}

/////////////////////////////////////////////////////////////////////////////////
//
void reset()
{
}

/////////////////////////////////////////////////////////////////////////////////
//
void update()
{
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	// TODO - move
    glLoadIdentity();

	// todo - move
    glBindTexture( GL_TEXTURE_2D, texture_handle );

    glEnable( GL_TEXTURE_2D );
    glColor3f( 1.0f, 1.0f, 1.0f );
    glBegin( GL_QUADS );
        glTexCoord2f( 1.0f, 0.0f );
        glVertex2d( mAppWindowWidth, 0 );

        glTexCoord2f( 0.0f, 0.0f );
        glVertex2d( 0, 0 );

        glTexCoord2f( 0.0f, 1.0f );
        glVertex2d( 0, mAppWindowHeight );

        glTexCoord2f( 1.0f, 1.0f );
        glVertex2d( mAppWindowWidth, mAppWindowHeight );
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
LRESULT CALLBACK window_proc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	static int last_char_code = 0;

    switch ( uMsg )
    {
        case WM_LBUTTONDOWN:
        {
			int x = (LOWORD(lParam) * gTextureWidth) / mAppWindowWidth;
			int y = (HIWORD(lParam) * gTextureHeight) / mAppWindowHeight;
			mLLCEFLib->mouseButton(MB_MOUSE_BUTTON_LEFT, ME_MOUSE_DOWN, x, y);
			mLLCEFLib->setFocus(true);
			return 0;
        };

        case WM_LBUTTONUP:
        {
			int x = (LOWORD(lParam) * gTextureWidth) / mAppWindowWidth;
			int y = (HIWORD(lParam) * gTextureHeight) / mAppWindowHeight;
			mLLCEFLib->mouseButton(MB_MOUSE_BUTTON_LEFT, ME_MOUSE_UP, x, y);
            return 0;
        };

		case WM_RBUTTONUP:
		{
			mLLCEFLib->reset();
			PostMessage(hWnd, WM_CLOSE, 0, 0L);
			return 0;
		};

		case WM_MOUSEMOVE:
        {
			int x = (LOWORD(lParam) * gTextureWidth) / mAppWindowWidth;
			int y = (HIWORD(lParam) * gTextureHeight) / mAppWindowHeight;
			mLLCEFLib->mouseMove(x, y);
			return 0;
        };

        case WM_CLOSE:
        {
            PostQuitMessage( 0 );
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
            int new_width = LOWORD( lParam );
            int new_height = HIWORD( lParam );
            resize_gl_screen( new_width, new_height );
            return 0;
        };
    };

    return DefWindowProc( hWnd, uMsg, wParam, lParam );
}

/////////////////////////////////////////////////////////////////////////////////
//
int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow )
{
    WNDCLASS wc;
    wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    wc.lpfnWndProc = (WNDPROC)window_proc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon( NULL, IDI_WINLOGO );
    wc.hCursor = LoadCursor( NULL, IDC_ARROW );
    wc.hbrBackground = NULL;
    wc.lpszMenuName = NULL;
    wc.lpszClassName = "Win32GL";
    RegisterClass( &wc );

    RECT window_rect;
    SetRect( &window_rect, 0, 0, mAppWindowWidth, mAppWindowHeight );

    DWORD ex_style = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
    DWORD style = WS_OVERLAPPEDWINDOW;
    AdjustWindowRectEx( &window_rect, style, FALSE, ex_style );

    HWND hWnd = CreateWindowEx( ex_style, "Win32GL", "Win32GL LLCEFLib test", style | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
                            80, 0, window_rect.right - window_rect.left, window_rect.bottom - window_rect.top,
                                NULL, NULL, hInstance, NULL );

    static  PIXELFORMATDESCRIPTOR pfd =
    {
        sizeof( PIXELFORMATDESCRIPTOR ),
        1,
        PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER, PFD_TYPE_RGBA,
        32, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 16, 0, 0, PFD_MAIN_PLANE, 0, 0, 0, 0
    };

    HDC hDC = GetDC( hWnd );

    GLuint pixel_format = ChoosePixelFormat( hDC, &pfd );
    SetPixelFormat( hDC, pixel_format, &pfd );
    HGLRC hRC = wglCreateContext( hDC );
    wglMakeCurrent( hDC, hRC );

    ShowWindow( hWnd,SW_SHOW );
    SetForegroundWindow( hWnd );
    SetFocus( hWnd );

    resize_gl_screen( mAppWindowWidth, mAppWindowHeight );

    glGenTextures( 1, &texture_handle );

    glBindTexture( GL_TEXTURE_2D, texture_handle );
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, gTextureWidth, gTextureHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
    glTexParameteri( GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR );
    init( hWnd );

    bool done = false;
    while( ! done )
    {
        MSG msg;
        if ( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) )
        {
            if ( WM_QUIT == msg.message )
            {
                done = true;
            }
            else
            {
                TranslateMessage( &msg );
                DispatchMessage( &msg );
            };
        }
        else
        {
            update();
            SwapBuffers( hDC );
        };
    };

    wglMakeCurrent( NULL,NULL );
    wglDeleteContext( hRC );
    ReleaseDC( hWnd,hDC );
    DestroyWindow( hWnd );
    UnregisterClass( "Win32GL", hInstance );

    reset();

    return 0;
}
