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

#include "llrenderhandler.h"

#include "llceflibimpl.h"

LLRenderHandler::LLRenderHandler(LLCEFLibImpl* parent) :
    mParent(parent)
{
}

bool LLRenderHandler::GetViewRect(CefRefPtr<CefBrowser> browser, CefRect &rect)
{
    int width, height;
    mParent->getSize(width, height);

    rect = CefRect(0, 0, width, height);
    return true;
}

void LLRenderHandler::OnPaint(CefRefPtr<CefBrowser> browser, PaintElementType type, const RectList &dirtyRects, const void *buffer, int width, int height)
{
    // TODO: send back dirty rects?

    //for (size_t i = 0; i < dirtyRects.size(); ++i)
    //{
    //  std::cout << "    >>> rect " << i << ": " << dirtyRects[i].x << " x " << dirtyRects[i].y << " -- " << dirtyRects[i].width << " x" << dirtyRects[i].height << std::endl;
    //}
    mParent->pageChanged((unsigned char*)(buffer), width, height);
}
