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

#import <Cocoa/Cocoa.h>

class LLCEFLib;

@interface LLOsxglView : NSView <NSWindowDelegate>
{
    NSOpenGLContext* _openGLContext;
    LLCEFLib* _llCefLib;
    NSTimer* _timer;
    BOOL _isClosing;
    BOOL _needsShutdown;
}

@property(retain) NSOpenGLContext* openGLContext;
@property(assign) LLCEFLib* llCefLib;
@property(retain) NSTimer* timer;
@property(assign) BOOL isClosing;
@property(assign) BOOL needsShutdown;

+ (LLOsxglView*)current;

- (void)onPageChangedCallbackPixels:(unsigned char*)pixels x:(int)x y:(int)y width:(int)width height:(int)height is_popup:(bool)is_popup;

@end
