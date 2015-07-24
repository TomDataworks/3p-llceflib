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

#ifndef _LLCEFLIBIMPL
#define _LLCEFLIBIMPL

#include "include/cef_app.h"
#include "llbrowserclient.h"
#include <functional>

#include "llceflib.h"
#include "llceflibplatform.h"

class CefSchemeRegistrar;
struct LLCEFLibSettings;
class LLBrowserClient;
class LLContextHandler;

class LLCEFLibImpl :
    public CefApp
{
    public:         // TODO: divide up methods into those uin pImpl interface and not
        LLCEFLibImpl();
        virtual ~LLCEFLibImpl();

        bool init(LLCEFLibSettings& user_settings);
        void update();
        void setSize(int width, int height);
        void getSize(int& width, int& height);
        int getDepth();
        void navigate(std::string url);
		void setPageZoom(double zoom_val);
        void reset();

        void setPageChangedCallback(std::function<void(unsigned char*, int, int)> callback);

        void pageChanged(unsigned char*, int, int);

        void setOnCustomSchemeURLCallback(std::function<void(std::string)> callback);
        void onCustomSchemeURL(std::string url);

        void setOnConsoleMessageCallback(std::function<void(std::string, std::string, int)> callback);
        void onConsoleMessage(std::string message, std::string source, int line);

        void setOnStatusMessageCallback(std::function<void(std::string)> callback);
        void onStatusMessage(std::string value);

        void setOnTitleChangeCallback(std::function<void(std::string)> callback);
        void onTitleChange(std::string title);

		void OnBeforeClose(CefRefPtr<CefBrowser> browser);

		void setOnLoadStartCallback(std::function<void()> callback);
		void onLoadStart();

		void setOnLoadEndCallback(std::function<void(int)> callback);
		void onLoadEnd(int httpStatusCode);

		void setOnNavigateURLCallback(std::function<void(std::string)> callback);
		void onNavigateURL(std::string url);

		void setOnHTTPAuthCallback(std::function<bool(const std::string host, const std::string realm, std::string&, std::string&)> callback);
		bool onHTTPAuth(const std::string host, const std::string realm, std::string& username, std::string& password);

		void mouseButton(EMouseButton mouse_button, EMouseEvent mouse_event, int x, int y);
		void mouseMove(int x, int y);
		void nativeMouseEvent(uint32_t msg, uint32_t wparam, uint64_t lparam);

		void nativeKeyboardEvent(uint32_t msg, uint32_t wparam, uint64_t lparam);
        void keyPress(int code, bool is_down);
		void keyboardEvent(
			EKeyEvent key_event,
			uint32_t key_code,
			const char *utf8_text,
			EKeyboardModifier modifiers,
			uint32_t native_scan_code,
			uint32_t native_virtual_key,
			uint32_t native_modifiers);

        void mouseWheel(int deltaY);
        void setFocus(bool focus);

        void stop();
        void reload(bool ignore_cache);
        bool canGoBack();
        void goBack();
        bool canGoForward();
        void goForward();
        bool isLoading();

        /* virtual */
        void OnRegisterCustomSchemes(CefRefPtr<CefSchemeRegistrar> registrar) OVERRIDE;

    private:
        CefRefPtr<LLBrowserClient> mBrowserClient;
        CefRefPtr<CefBrowser> mBrowser;
		CefRefPtr<LLContextHandler> mContextHandler;
        int mViewWidth;
        int mViewHeight;
        const int mViewDepth = 4;
        std::function<void(unsigned char*, int, int)> mPageChangedCallbackFunc;
        std::function<void(std::string)> mOnCustomSchemeURLCallbackFunc;
        std::function<void(std::string, std::string, int line)> mOnConsoleMessageCallbackFunc;
        std::function<void(std::string)> mOnStatusMessageCallbackFunc;
		std::function<void(std::string)> mOnTitleChangeCallbackFunc;
		std::function<void()> mOnLoadStartCallbackFunc;
		std::function<void(int)> mOnLoadEndCallbackFunc;
		std::function<void(std::string)> mOnNavigateURLCallbackFunc;
		std::function<bool(const std::string host, const std::string realm, std::string&, std::string&)> mOnHTTPAuthCallbackFunc;

		IMPLEMENT_REFCOUNTING(LLCEFLibImpl);
};

class LLCEFLibAuthCredentials
{
    public:
        LLCEFLibAuthCredentials(bool isProxy,
            const CefString& host,
            int port,
            const CefString& realm,
            const CefString& scheme,
            CefRefPtr<CefAuthCallback> callback) :
                mProxy(isProxy),
                mPort(port),
                mRealm(realm),
                mScheme(scheme),
                mHost(host),
                mCallback(callback)
        {
        }

        void cancel()
        {
            mCallback->Cancel();
        }

        void proceed(const char* username, const char* password)
        {
            mCallback->Continue(username, password);
        }

        int getPort()
        {
            return mPort;
        }

        const char* getRealm()
        {
            return mRealm.c_str();
        }

        const char* getScheme()
        {
            return mScheme.c_str();
        }

        const char* getHost()
        {
            return mHost.c_str();
        }

        bool isProxy()
        {
            return mProxy;
        }

    private:
        bool mProxy;
        int mPort;
        const std::string mRealm;
        const std::string mScheme;
        const std::string mHost;
        CefRefPtr<CefAuthCallback> mCallback;

        IMPLEMENT_REFCOUNTING(LLCEFLibAuthCredentials);
};

#endif // _LLCEFLIBIMPL
