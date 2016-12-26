/**
 * @brief LLCEFLib - Wrapper for CEF SDK for use in LL Web Media Plugin
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

#include "llceflibimpl.h"
#include "llceflib.h"

#include "llceflibplatform.h"

#include "llschemehandler.h"
#include "llrenderhandler.h"
#include "llbrowserclient.h"
#include "llcontexthandler.h"

#include "include/base/cef_bind.h"
#include "include/wrapper/cef_closure_task.h"

#ifdef __APPLE__
#import <Cocoa/Cocoa.h>
#endif

#if __linux__
#include <libgen.h>
#include <sys/types.h>
#include <pwd.h>
#endif

LLCEFLibImpl::LLCEFLibImpl() :
    mBrowser(nullptr),
    mViewWidth(0),
    mViewHeight(0),
    mRequestedZoom(0.0),
    mSystemFlashEnabled(false),
    mMediaStreamEnabled(false)
{
    // default is second life scheme
    mCustomSchemes = { "secondlife://", "x-grid-location-info://" };

    mFlushStoreCallback = new FlushStoreCallback();
}

LLCEFLibImpl::~LLCEFLibImpl()
{
}

void LLCEFLibImpl::OnBeforeCommandLineProcessing(const CefString& process_type, CefRefPtr<CefCommandLine> command_line)
{
    if (process_type.empty())
    {
        command_line->AppendSwitch("disable-surfaces");     // for PDF files
        command_line->AppendSwitch("enable-begin-frame-scheduling"); // Synchronize the frame rate between all processes.

        if (mMediaStreamEnabled)                    // for webcam/media access
        {
            command_line->AppendSwitch("enable-media-stream");
        }

        if (mSystemFlashEnabled)                    // for Flash
        {
            command_line->AppendSwitch("enable-system-flash");
        }
    }
}

bool LLCEFLibImpl::init(LLCEFLib::LLCEFLibSettings& user_settings)
{
#ifdef LLCEFLIB_DEBUG
    std::cout << "Starting.." << std::endl;
#endif

#ifdef WIN32
    CefMainArgs args(GetModuleHandle(NULL));
#else
    CefMainArgs args(0, NULL);
#endif

    CefSettings settings;
    settings.no_sandbox = true;
    settings.windowless_rendering_enabled = true;
#ifdef WIN32
    settings.multi_threaded_message_loop = false;
    std::string host("llceflib_host.exe");
   cef_string_utf8_to_utf16(host.c_str(), host.size(), &settings.browser_subprocess_path);
#elif __APPLE__
    NSString* appBundlePath = [[NSBundle mainBundle] bundlePath];
    CefString(&settings.browser_subprocess_path) = [[NSString stringWithFormat: @"%@/Contents/Frameworks/LLCefLib Helper.app/Contents/MacOS/LLCefLib Helper", appBundlePath] UTF8String];
#elif __linux__
    std::string plugin_process_path;
    char path[ 4096 ];
    int len = readlink ("/proc/self/exe", path, sizeof(path));
    if(len != -1)
    {
        path[len] = 0;
        plugin_process_path = dirname(path) ;
    }
    CefString(&settings.browser_subprocess_path) = plugin_process_path + "/llceflib_host";
    // Disable sandbox for now
    settings.no_sandbox = true;
#endif

    // change settings based on what was passed in
    // Only change user agent if user wants to
    if (!user_settings.user_agent_substring.empty())
    {
        std::string user_agent(user_settings.user_agent_substring);
        cef_string_utf8_to_utf16(user_agent.c_str(), user_agent.size(), &settings.product_version);
    }

    // list of language locale codes used to configure the Accept-Language HTTP header value
    if (!user_settings.accept_language_list.empty())
    {
        std::string accept_language_list(user_settings.accept_language_list);
        cef_string_utf8_to_utf16(accept_language_list.c_str(), accept_language_list.size(), &settings.accept_language_list);
    }

    // Set the proper locale for cef internals
    if (!user_settings.locale.empty())
    {
        std::string locale(user_settings.locale);
        cef_string_utf8_to_utf16(locale.c_str(), locale.size(), &settings.locale);
    }

    // set path to cache if enabled and set
    if (user_settings.cache_enabled && !user_settings.cache_path.empty())
    {
        std::string cache_path(user_settings.cache_path);
        cef_string_utf8_to_utf16(cache_path.c_str(), cache_path.size(), &settings.cache_path);
    }

    // Control logging output and location
    settings.log_severity = LOGSEVERITY_DISABLE;
    if (user_settings.debug_output && !user_settings.log_file.empty())
    {
        settings.log_severity = LOGSEVERITY_VERBOSE;
        std::string log_file(user_settings.log_file);
        cef_string_utf8_to_utf16(log_file.c_str(), log_file.size(), &settings.log_file);
    }

    mSystemFlashEnabled = user_settings.plugins_enabled;
    mMediaStreamEnabled = user_settings.media_stream_enabled;

#ifdef WIN32
    // turn on only for Windows 7+
    CefEnableHighDPISupport();
#endif

    bool result = CefInitialize(args, settings, this, NULL);
    if (! result)
    {
        return false;
    }

    // removed for now since the scheme handler as implemented is broken in rev 2357
    // now schemes are caught via onBeforeBrowse override - when 2357 is fixed the code
    // should revert to using a scheme hander vs a URL parser
    //scheme_handler::RegisterSchemeHandlers(this);

    setSize(user_settings.initial_width, user_settings.initial_height);

    CefWindowInfo window_info;
    window_info.windowless_rendering_enabled = true;

    CefBrowserSettings browser_settings;
    browser_settings.windowless_frame_rate = 60; // 30 is default - hook into LL media scheduling system?
    browser_settings.webgl = STATE_ENABLED;

    // change settings based on what was passed in
    browser_settings.javascript = user_settings.javascript_enabled ? STATE_ENABLED : STATE_DISABLED;
    browser_settings.plugins = STATE_ENABLED;

    // set page zoom (won't be acted up until later but tha'ts okay)
    mRequestedZoom = user_settings.page_zoom_factor;

    // CEF handler classes
    LLRenderHandler* renderHandler = new LLRenderHandler(this);
    mBrowserClient = new LLBrowserClient(this, renderHandler);

    // if this is NULL for CreateBrowserSync, the global request context will be used
    CefRefPtr<CefRequestContext> rc = NULL;

    // Add a custom context handler that implements a
    // CookieManager so cookies will persist to disk.
    // (if cookies enabled)
    if (user_settings.cookies_enabled)
    {
#ifdef WIN32
        std::string cookiePath = ".\\cookies";
#elif __APPLE__ || __linux__
        std::string cookiePath = "./cookies";
#endif
        if (!user_settings.cookie_store_path.empty())
        {
            cookiePath = std::string(user_settings.cookie_store_path);
        }

        mContextHandler = new LLContextHandler(cookiePath.c_str());

        CefRequestContextSettings contextSettings;
        if (user_settings.cache_enabled && !user_settings.cache_path.empty())
        {
            cef_string_utf8_to_utf16(user_settings.cache_path.c_str(), user_settings.cache_path.size(), &contextSettings.cache_path);
        }
        rc = CefRequestContext::CreateContext(contextSettings, mContextHandler.get());
    }

    CefString url = "";
    mBrowser = CefBrowserHost::CreateBrowserSync(window_info, mBrowserClient.get(), url, browser_settings, rc);

    return true;
}

void LLCEFLibImpl::update()
{
    CefDoMessageLoopWork();
}

void LLCEFLibImpl::shutdown()
{
#if !defined(__APPLE__)
    CefShutdown();
#endif
}

void LLCEFLibImpl::setOnPageChangedCallback(std::function<void(unsigned char*, int, int, int, int, bool)> callback)
{
    mOnPageChangedCallbackFunc = callback;
}

void LLCEFLibImpl::setOnCustomSchemeURLCallback(std::function<void(std::string)> callback)
{
    mOnCustomSchemeURLCallbackFunc = callback;
}

void LLCEFLibImpl::setOnConsoleMessageCallback(std::function<void(std::string, std::string, int)> callback)
{
    mOnConsoleMessageCallbackFunc = callback;
}

void LLCEFLibImpl::setOnAddressChangeCallback(std::function<void(std::string)> callback)
{
    mOnAddressChangeCallbackFunc = callback;
}

void LLCEFLibImpl::setOnStatusMessageCallback(std::function<void(std::string)> callback)
{
    mOnStatusMessageCallbackFunc = callback;
}

void LLCEFLibImpl::setOnTitleChangeCallback(std::function<void(std::string)> callback)
{
    mOnTitleChangeCallbackFunc = callback;
}

void LLCEFLibImpl::setOnLoadStartCallback(std::function<void()> callback)
{
    mOnLoadStartCallbackFunc = callback;
}

void LLCEFLibImpl::setOnRequestExitCallback(std::function<void()> callback)
{
    mOnRequestExitCallbackFunc = callback;
}

void LLCEFLibImpl::setOnCursorChangedCallback(std::function<void(LLCEFLib::ECursorType type, unsigned int)> callback)
{
    mOnCursorChangedCallbackFunc = callback;
}

void LLCEFLibImpl::setOnLoadEndCallback(std::function<void(int)> callback)
{
    mOnLoadEndCallbackFunc = callback;
}

void LLCEFLibImpl::setOnNavigateURLCallback(std::function<void(std::string, std::string)> callback)
{
    mOnNavigateURLCallbackFunc = callback;
}

void LLCEFLibImpl::setOnHTTPAuthCallback(std::function<bool(const std::string host, const std::string realm, std::string&, std::string&)> callback)
{
    mOnHTTPAuthCallbackFunc = callback;
}

void LLCEFLibImpl::setOnFileDownloadCallback(std::function<void(const std::string filename)> callback)
{
    mOnFileDownloadCallbackFunc = callback;
}

void LLCEFLibImpl::setOnFileDialogCallback(std::function<const std::string()> callback)
{
    mOnFileDialogCallbackFunc = callback;
}

void LLCEFLibImpl::setSize(int width, int height)
{
    mViewWidth = width;
    mViewHeight = height;

    if (mBrowser && mBrowser->GetHost())
    {
        mBrowser->GetHost()->WasResized();
    }
}

void LLCEFLibImpl::getSize(int& width, int& height)
{
    width = mViewWidth;
    height = mViewHeight;
}

void LLCEFLibImpl::onPageChanged(unsigned char* pixels, int x, int y, int width, int height, bool is_popup)
{
    if (mOnPageChangedCallbackFunc)
    {
        mOnPageChangedCallbackFunc(pixels, x, y, width, height, is_popup);
    }
}

void LLCEFLibImpl::onCustomSchemeURL(std::string url)
{
    if (mOnCustomSchemeURLCallbackFunc)
    {
        mOnCustomSchemeURLCallbackFunc(url);
    }
}

void LLCEFLibImpl::onConsoleMessage(std::string message, std::string source, int line)
{
    if (mOnConsoleMessageCallbackFunc)
    {
        mOnConsoleMessageCallbackFunc(message, source, line);
    }
}

void LLCEFLibImpl::onAddressChange(std::string new_url)
{
    if (mOnAddressChangeCallbackFunc)
    {
        mOnAddressChangeCallbackFunc(new_url);
    }
}

void LLCEFLibImpl::onStatusMessage(std::string value)
{
    if (mOnStatusMessageCallbackFunc)
    {
        mOnStatusMessageCallbackFunc(value);
    }
}

void LLCEFLibImpl::onTitleChange(std::string title)
{
    if (mOnTitleChangeCallbackFunc)
    {
        mOnTitleChangeCallbackFunc(title);
    }
}

void LLCEFLibImpl::onLoadStart()
{
    if (mBrowser && mBrowser->GetHost())
    {
        double cur_zoom = convertZoomLevel(mBrowser->GetHost()->GetZoomLevel());
        if (fabs(convertZoomLevel(mRequestedZoom)) - fabs(cur_zoom) > 0.001)
        {
            mBrowser->GetHost()->SetZoomLevel(convertZoomLevel(mRequestedZoom));
        };
    }

    if (mOnLoadStartCallbackFunc)
    {
        mOnLoadStartCallbackFunc();
    }
}

void LLCEFLibImpl::onLoadEnd(int httpStatusCode)
{
    if (mOnLoadEndCallbackFunc)
    {
        mOnLoadEndCallbackFunc(httpStatusCode);
    }
}

void LLCEFLibImpl::onNavigateURL(std::string url, std::string target)
{
    if (mOnNavigateURLCallbackFunc)
    {
        mOnNavigateURLCallbackFunc(url, target);
    }
}

void LLCEFLibImpl::onRequestExit()
{
    if (mOnRequestExitCallbackFunc)
    {
        mOnRequestExitCallbackFunc();
    }
}

void LLCEFLibImpl::onCursorChanged(LLCEFLib::ECursorType type, unsigned int cursor)
{
    if (mOnCursorChangedCallbackFunc)
    {
        mOnCursorChangedCallbackFunc(type, cursor);
    }
}

bool LLCEFLibImpl::onHTTPAuth(const std::string host, const std::string realm, std::string& username, std::string& password)
{
    if (mOnHTTPAuthCallbackFunc)
    {
        return mOnHTTPAuthCallbackFunc(host, realm, username, password);
    }

    return false;
}


void LLCEFLibImpl::onFileDownload(const std::string filename)
{
    if (mOnFileDownloadCallbackFunc)
    {
        mOnFileDownloadCallbackFunc(filename);
    }
}

const std::string LLCEFLibImpl::onFileDialog()
{
    if (mOnFileDialogCallbackFunc)
    {
        return mOnFileDialogCallbackFunc();
    }

    return std::string();
}

int LLCEFLibImpl::getDepth()
{
    return mViewDepth;
}

void LLCEFLibImpl::navigate(std::string url)
{
    if (mBrowser && mBrowser->GetMainFrame() && url.length() > 0)
    {
        mBrowser->GetMainFrame()->LoadURL(url);
    }
}

void LLCEFLibImpl::postData(std::string url, std::string data, std::string headers)
{
    if (mBrowser)
    {
        if (mBrowser->GetMainFrame())
        {
            CefRefPtr<CefRequest> request = CefRequest::Create();

            request->SetURL(url);
            request->SetMethod("POST");

            // TODO - get this from the headers parameter
            CefRequest::HeaderMap headerMap;
            headerMap.insert(
                std::make_pair("Accept", "*/*"));
            headerMap.insert(
                std::make_pair("Content-Type", "application/x-www-form-urlencoded"));
            request->SetHeaderMap(headerMap);

            const std::string& upload_data = data;
            CefRefPtr<CefPostData> postData = CefPostData::Create();
            CefRefPtr<CefPostDataElement> element = CefPostDataElement::Create();
            element->SetToBytes(upload_data.size(), upload_data.c_str());
            postData->AddElement(element);
            request->SetPostData(postData);

            mBrowser->GetMainFrame()->LoadRequest(request);
        }
    }
}

void LLCEFLibImpl::setCookie(std::string url, std::string name, std::string value, std::string domain, std::string path)
{
    CefRefPtr<CefCookieManager> manager = mContextHandler->GetCookieManager();
    CefCookie cookie;
    CefString(&cookie.name) = name;
    CefString(&cookie.value) = value;
    CefString(&cookie.domain) = domain;
    CefString(&cookie.path) = path;
    cookie.httponly = true;     // IMPORTANT: these 2 fields are always set to true for now and do
    cookie.secure = true;       // do not use the parameters from llceflib.h because of limitation of CEF::Bind() # params

    // TODO set from input
    cookie.has_expires = true;
    cookie.expires.year = 2064;
    cookie.expires.month = 4;
    cookie.expires.day_of_week = 5;
    cookie.expires.day_of_month = 10;

    manager->SetCookie(url, cookie, nullptr);
    manager->FlushStore(nullptr);
}

void LLCEFLibImpl::setPageZoom(double zoom_val)
{
    if (mBrowser && mBrowser->GetHost())
    {
        mBrowser->GetHost()->SetZoomLevel(convertZoomLevel(zoom_val));
        mRequestedZoom = zoom_val;
    }
}

void LLCEFLibImpl::mouseButton(LLCEFLib::EMouseButton mouse_button, LLCEFLib::EMouseEvent mouse_event, int x, int y)
{
	// modify coords based on rules (Y flipped, scaled etc.)
	convertInputCoords(x, y);

    // select click location
    CefMouseEvent cef_mouse_event;
    cef_mouse_event.x = x;
    cef_mouse_event.y = y;
    cef_mouse_event.modifiers = EVENTFLAG_LEFT_MOUSE_BUTTON;

    // select button
    CefBrowserHost::MouseButtonType btnType = MBT_LEFT;
    if (mouse_button == LLCEFLib::MB_MOUSE_BUTTON_RIGHT)
    {
        btnType = MBT_RIGHT;
    }
    if (mouse_button == LLCEFLib::MB_MOUSE_BUTTON_MIDDLE)
    {
        btnType = MBT_MIDDLE;
    }

    // TODO: set this properly
    int last_click_count = 1;

    // action TODO: extend to include "move" although this might be enough
    bool is_down = false;
    if (mouse_event == LLCEFLib::ME_MOUSE_DOWN)
    {
        is_down = true;
    }
    else if (mouse_event == LLCEFLib::ME_MOUSE_UP)
    {
        is_down = false;
    }
    else if (mouse_event == LLCEFLib::ME_MOUSE_DOUBLE_CLICK)
    {
        is_down = true;
        last_click_count = 2;
    }

    // send to CEF
    if (mBrowser && mBrowser->GetHost())
    {
        mBrowser->GetHost()->SendMouseClickEvent(cef_mouse_event, btnType, is_down ? false : true, last_click_count);
    }
};

void LLCEFLibImpl::mouseMove(int x, int y)
{
	// modify coords based on rules (Y flipped, scaled etc.)
	convertInputCoords(x, y);

    CefMouseEvent mouse_event;
    mouse_event.x = x;
    mouse_event.y = y;
    mouse_event.modifiers = EVENTFLAG_LEFT_MOUSE_BUTTON;

    if (mBrowser && mBrowser->GetHost())
    {
        mBrowser->GetHost()->SendMouseMoveEvent(mouse_event, false);
    }
};

void LLCEFLibImpl::mouseWheel(int deltaX, int deltaY)
{
    if (mBrowser && mBrowser->GetHost())
    {
        CefMouseEvent mouse_event;
        mouse_event.modifiers = 0;
        mBrowser->GetHost()->SendMouseWheelEvent(mouse_event, deltaX, deltaY);
    }
}

void LLCEFLibImpl::setFocus(bool focus)
{
    if (mBrowser && mBrowser->GetHost())
    {
        mBrowser->GetHost()->SendFocusEvent(focus);
    }
}

void LLCEFLibImpl::requestExit()
{
    if (mContextHandler && mContextHandler->GetCookieManager())
    {
        mContextHandler->GetCookieManager()->FlushStore(mFlushStoreCallback);
    }

    if (mBrowser && mBrowser->GetHost())
    {
        bool force_close = false;
        mBrowser->GetHost()->CloseBrowser(force_close);
    }
}

void LLCEFLibImpl::OnRegisterCustomSchemes(CefRefPtr<CefSchemeRegistrar> registrar)
{
    // removed for now since the scheme handler as implemented is broken in rev 2357
    // now schemes are caught via onBeforeBrowse override - when 2357 is fixed the code
    // should revert to using a scheme hander vs a URL parser
    //scheme_handler::RegisterCustomSchemes(registrar);
}

void LLCEFLibImpl::setCustomSchemes(std::vector<std::string> custom_schemes)
{
    mCustomSchemes = custom_schemes;
}

std::vector<std::string>& LLCEFLibImpl::getCustomSchemes()
{
    return mCustomSchemes;
}

void LLCEFLibImpl::stop()
{
    if (mBrowser && mBrowser->GetHost())
    {
        mBrowser->StopLoad();
    }
}

void LLCEFLibImpl::reload(bool ignore_cache)
{
    if (mBrowser && mBrowser->GetHost())
    {
        if (ignore_cache)
        {
            mBrowser->ReloadIgnoreCache();
        }
        else
        {
            mBrowser->Reload();
        }
    }
}

bool LLCEFLibImpl::canGoBack()
{
    if (mBrowser && mBrowser->GetHost())
    {
        return mBrowser->CanGoBack();
    }

    // default to allow so if UI is relies on it, this is still sensible
    return true;
}

void LLCEFLibImpl::goBack()
{
    if (mBrowser && mBrowser->GetHost())
    {
        mBrowser->GoBack();
    }
}

bool LLCEFLibImpl::canGoForward()
{
    if (mBrowser && mBrowser->GetHost())
    {
        return mBrowser->CanGoForward();
    }

    // default to allow so if UI is relies on it, this is still sensible
    return true;
}

void LLCEFLibImpl::goForward()
{
    if (mBrowser && mBrowser->GetHost())
    {
        mBrowser->GoForward();
    }
}

bool LLCEFLibImpl::isLoading()
{
    if (mBrowser && mBrowser->GetHost())
    {
        return mBrowser->IsLoading();
    }

    // default to allow so if UI is relies on it, this is still sensible
    return true;
}

bool LLCEFLibImpl::editCanCopy()
{
    // TODO: ask CEF if we can do this
    return true;
}

bool LLCEFLibImpl::editCanCut()
{
    // TODO: ask CEF if we can do this
    return true;
}

bool LLCEFLibImpl::editCanPaste()
{
    // TODO: ask CEF if we can do this
    return true;
}

void LLCEFLibImpl::editCopy()
{
    if (mBrowser && mBrowser->GetFocusedFrame())
    {
        mBrowser->GetFocusedFrame()->Copy();
    }
}

void LLCEFLibImpl::editCut()
{
    if (mBrowser && mBrowser->GetFocusedFrame())
    {
        mBrowser->GetFocusedFrame()->Cut();
    }
}

void LLCEFLibImpl::editPaste()
{
    if (mBrowser && mBrowser->GetFocusedFrame())
    {
        mBrowser->GetFocusedFrame()->Paste();
    }
}

void LLCEFLibImpl::showDevTools(bool show)
{
    if (mBrowser.get() && mBrowser->GetHost())
    {
        if (show)
        {
            CefWindowInfo window_info;
            window_info.x = 0;
            window_info.y = 0;
            window_info.width = 400;
            window_info.height = 400;
#ifdef WIN32
            window_info.SetAsPopup(NULL, "LLCEFLib Dev Tools");
#endif
            CefRefPtr<CefClient> client = mBrowserClient;
            CefBrowserSettings browser_settings;
            CefPoint inspect_element_at;
            mBrowser->GetHost()->ShowDevTools(window_info, client, browser_settings, inspect_element_at);
        }
        else
        {
            mBrowser->GetHost()->CloseDevTools();
        }
    }
}

CefRefPtr<CefBrowser> LLCEFLibImpl::getBrowser()
{
    return mBrowser;
}

void LLCEFLibImpl::setBrowser(CefRefPtr<CefBrowser> browser)
{
    mBrowser = browser;
}

std::string LLCEFLibImpl::makeCompatibleUserAgentString(const std::string base)
{
    std::string frag = "(" + base + ")" + " Chrome/";
#ifdef WIN32
    frag += CEF_CHROME_VERSION_WIN;
#elif __linux__
	frag += CEF_CHROME_VERSION_LINUX;
#elif __APPLE__
    frag += CEF_CHROME_VERSION_OSX;
#else
#error "Platform not supported.
#endif
    return frag;
}

void LLCEFLibImpl::convertInputCoords(int& x, int& y)
{
#ifdef FLIP_OUTPUT_Y
	y = mViewHeight - y;
#endif
}

// convert linear zoom (1.0, 2.0, 3.0 etc.) to log based zoom CEF uses
// where 0.0 is 100%, 1.0 is 120%, 2.0 is 144% etc. (each 1.0 == 20% more)
double LLCEFLibImpl::convertZoomLevel(double linear_zoom)
{
    if (linear_zoom == 0)
    {
        return 0.0;
    }

    double cef_zoom = log(linear_zoom) / log(1.2);

    return cef_zoom;
}
