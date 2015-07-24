/**
 * @brief LLCEFLib - Wrapper for CEF SDK for use in LL Web Media Plugin
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

#ifndef _LLCEFLIB
#define _LLCEFLIB

#include <string>
#include <memory>
#include <functional>

class LLCEFLibImpl;

// version information
const std::string LLCEFLIB_BASE_VERSION = "1.0.1";
const std::string CEF_VERSION_WIN = "(CEF-WIN-3.2357.1281.gd660177-32)";
const std::string CEF_VERSION_OSX = "(CEF-OSX-3.2171.2069-32)";
#ifdef _MSC_VER
const std::string LLCEFLIB_VERSION = LLCEFLIB_BASE_VERSION + "-" + CEF_VERSION_WIN;
#else
const std::string LLCEFLIB_VERSION = LLCEFLIB_BASE_VERSION + "-" + CEF_VERSION_OSX;
#endif

// used to pass in various settings during initialization
struct LLCEFLibSettings
{
    // initial dimensions of the browser window
    unsigned int inital_width = 512;
    unsigned int inital_height = 512;

    // substring inserted into existing user agent string
    std::string user_agent_substring = "";

    // enable/disable features
    bool javascript_enabled = true;
    bool plugins_enabled = true;
    bool cookies_enabled = true;

	// path to cookie store
	std::string cookie_store_path;

    // list of language locale codes used to configure the Accept-Language HTTP header value
	// and change the default language of the browser
	std::string accept_language_list = "en-us";
};

typedef enum e_key_event
{
	KE_KEY_DOWN,
	KE_KEY_REPEAT,
	KE_KEY_UP
}EKeyEvent;

typedef enum e_mouse_event
{
	ME_MOUSE_MOVE,
	ME_MOUSE_DOWN,
	ME_MOUSE_UP,
	ME_MOUSE_DOUBLE_CLICK
}EMouseEvent;

typedef enum e_mouse_button
{
	MB_MOUSE_BUTTON_LEFT,
	MB_MOUSE_BUTTON_RIGHT,
	MB_MOUSE_BUTTON_MIDDLE
}EMouseButton;

typedef enum e_keyboard_modifier
{
	KM_MODIFIER_NONE =    0x00,
	KM_MODIFIER_SHIFT =   0x01,
	KM_MODIFIER_CONTROL = 0x02,
	KM_MODIFIER_ALT =     0x04,
	KM_MODIFIER_META =    0x08
}EKeyboardModifier;

class LLCEFLib
{
    public:
        LLCEFLib();
        ~LLCEFLib();

        bool init(LLCEFLibSettings& user_settings);

        void update();
        void setSize(int width, int height);
        void getSize(int& width, int& height);
        void navigate(std::string url);
		void setPageZoom(double zoom_val)	;
		void mouseButton(EMouseButton mouse_button, EMouseEvent mouse_event, int x, int y);
        void mouseMove(int x, int y);

		void keyboardEvent(
			EKeyEvent key_event,
			uint32_t key_code,
			const char *utf8_text,
			EKeyboardModifier modifiers,
			uint32_t native_scan_code,
			uint32_t native_virtual_key,
			uint32_t native_modifiers);
		void nativeKeyboardEvent(uint32_t msg, uint32_t wparam, uint64_t lparam);
        void keyPress(int code, bool is_down);
        void setFocus(bool focus);
        void mouseWheel(int deltaY);

        void reset();

        void stop();
        void reload(bool ignore_cache);
        bool canGoBack();
        void goBack();
        bool canGoForward();
        void goForward();
        bool isLoading();

        void setPageChangedCallback(std::function<void(unsigned char*, int, int)> callback);
        void setOnCustomSchemeURLCallback(std::function<void(std::string)> callback);
        void setOnConsoleMessageCallback(std::function<void(std::string, std::string, int)> callback);
        void setOnStatusMessageCallback(std::function<void(std::string)> callback);
		void setOnTitleChangeCallback(std::function<void(std::string)> callback);
		void setOnLoadStartCallback(std::function<void()> callback);
		void setOnLoadEndCallback(std::function<void(int)> callback);
		void setOnNavigateURLCallback(std::function<void(std::string)> callback);
		void setOnHTTPAuthCallback(std::function<bool(const std::string host, const std::string realm, std::string&, std::string&)> callback);

    private:
        std::auto_ptr <LLCEFLibImpl> mImpl;
};

#endif // _LLCEFLIB
