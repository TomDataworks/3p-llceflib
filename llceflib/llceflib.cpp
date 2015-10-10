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

#include "llceflib.h"
#include "llceflibimpl.h"

LLCEFLib::LLCEFLib()
    :mImpl(new LLCEFLibImpl())
{
	mImpl->AddRef();
}

LLCEFLib::~LLCEFLib()
{
	mImpl->Release();

}

bool LLCEFLib::init(LLCEFLib::LLCEFLibSettings& user_settings)
{
    return mImpl->init(user_settings);
}

void LLCEFLib::update()
{
    mImpl->update();
}

void LLCEFLib::setSize(int width, int height)
{
    mImpl->setSize(width, height);
}

void LLCEFLib::getSize(int& width, int& height)
{
    mImpl->getSize(width, height);
}

void LLCEFLib::navigate(std::string url)
{
    if (url.length() > 0)
    {
        mImpl->navigate(url);
    }
}

void LLCEFLib::setPageZoom(double zoom_val)
{
	mImpl->setPageZoom(zoom_val);
}

void LLCEFLib::setOnPageChangedCallback(std::function<void(unsigned char*, int, int)> callback)
{
    mImpl->setOnPageChangedCallback(callback);
}

void LLCEFLib::setOnCustomSchemeURLCallback(std::function<void(std::string url)> callback)
{
    mImpl->setOnCustomSchemeURLCallback(callback);
}

void LLCEFLib::setOnConsoleMessageCallback(std::function<void(std::string, std::string, int)> callback)
{
    mImpl->setOnConsoleMessageCallback(callback);
}

void LLCEFLib::setOnAddressChangeCallback(std::function<void(std::string value)> callback)
{
	mImpl->setOnAddressChangeCallback(callback);
}

void LLCEFLib::setOnStatusMessageCallback(std::function<void(std::string value)> callback)
{
	mImpl->setOnStatusMessageCallback(callback);
}

void LLCEFLib::setOnTitleChangeCallback(std::function<void(std::string title)> callback)
{
	mImpl->setOnTitleChangeCallback(callback);
}

void LLCEFLib::setOnLoadStartCallback(std::function<void()> callback)
{
	mImpl->setOnLoadStartCallback(callback);
}

void LLCEFLib::setOnRequestExitCallback(std::function<void()> callback)
{
	mImpl->setOnRequestExitCallback(callback);
}

void LLCEFLib::setOnCursorChangedCallback(std::function<void(LLCEFLib::ECursorType type, size_t)> callback)
{
	mImpl->setOnCursorChangedCallback(callback);
}

void LLCEFLib::setOnLoadEndCallback(std::function<void(int)> callback)
{
	mImpl->setOnLoadEndCallback(callback);
}

void LLCEFLib::setOnNavigateURLCallback(std::function<void(std::string url, std::string target)> callback)
{
	mImpl->setOnNavigateURLCallback(callback);
}

void LLCEFLib::setOnHTTPAuthCallback(std::function<bool(const std::string host, const std::string realm, std::string& username, std::string& password)> callback)
{
	mImpl->setOnHTTPAuthCallback(callback);
}

void LLCEFLib::setCustomSchemes(std::vector<std::string> custom_schemes)
{
	mImpl->setCustomSchemes(custom_schemes);
}

void LLCEFLib::reset()
{
    mImpl->reset();
}

void LLCEFLib::mouseButton(EMouseButton mouse_button, EMouseEvent mouse_event, int x, int y)

{
    mImpl->mouseButton(mouse_button, mouse_event, x, y);
}

void LLCEFLib::mouseMove(int x, int y)
{
    mImpl->mouseMove(x, y);
}

void LLCEFLib::nativeKeyboardEvent(uint32_t msg, size_t wparam, ptrdiff_t lparam)
{
	mImpl->nativeKeyboardEvent(msg, wparam, lparam);
}

void LLCEFLib::keyboardEvent(
	EKeyEvent key_event,
	uint32_t key_code,
	const char *utf8_text,
	EKeyboardModifier modifiers,
	uint32_t native_scan_code,
	uint32_t native_virtual_key,
	uint32_t native_modifiers)
{
	mImpl->keyboardEvent(key_event, key_code, utf8_text, modifiers, native_scan_code, native_virtual_key, native_modifiers);
}

void LLCEFLib::mouseWheel(int deltaY)
{
    mImpl->mouseWheel(deltaY);
}

void LLCEFLib::setFocus(bool focus)
{
    mImpl->setFocus(focus);
}

void LLCEFLib::stop()
{
    mImpl->stop();
}

void LLCEFLib::reload(bool ignore_cache)
{
    mImpl->reload(ignore_cache);
}

bool LLCEFLib::canGoBack()
{
    return mImpl->canGoBack();
}

void LLCEFLib::goBack()
{
    mImpl->goBack();
}

bool LLCEFLib::canGoForward()
{
    return mImpl->canGoForward();
}

void LLCEFLib::goForward()
{
    mImpl->goForward();
}

bool LLCEFLib::isLoading()
{
	return mImpl->isLoading();
}


bool LLCEFLib::editCanCopy()
{
	return mImpl->editCanCopy();
}

bool LLCEFLib::editCanCut()
{
	return mImpl->editCanCut();
}

bool LLCEFLib::editCanPaste()
{
	return mImpl->editCanPaste();
}

void LLCEFLib::editCopy()
{
	mImpl->editCopy();
}

void LLCEFLib::editCut()
{
	mImpl->editCut();
}

void LLCEFLib::editPaste()
{
	mImpl->editPaste();
}
