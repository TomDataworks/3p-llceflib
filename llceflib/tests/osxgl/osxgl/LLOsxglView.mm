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

#import "LLOsxglView.h"

#include "llCEFLib.h"

#include "boost/function.hpp"
#include "boost/bind.hpp"

#include <OpenGL/gl.h>

#import <GLKit/GLKit.h>

static const int textureWidth = 1024;
static const int textureHeight = 1024;
static GLuint textureHandle = 0;

static LLOsxglView *gCurrent = nil;

static void onPageChangedCallback(unsigned char *pixels, int x, int y, int width, int height, bool is_popup) {
    [[LLOsxglView current] onPageChangedCallbackPixels:pixels x:x y:y width:width height:height is_popup:is_popup];
}

static void onRequestExitCallback() {
    [LLOsxglView current].needsShutdown = YES;
}

@implementation LLOsxglView

@synthesize openGLContext = _openGLContext;
@synthesize llCefLib = _llCefLib;
@synthesize timer = _timer;
@synthesize isClosing = _isClosing;
@synthesize needsShutdown = _needsShutdown;

+ (LLOsxglView *)current {
    return gCurrent;
}

- (instancetype)initWithCoder:(NSCoder *)coder {
    self = [super initWithCoder:coder];
    if (self) {
        [self setupView];
    }
    return self;
}

- (instancetype)initWithFrame:(NSRect)frameRect {
    self = [super initWithFrame:frameRect];
    if (self) {
        [self setupView];
    }
    return self;
}

- (void)setupView {
    gCurrent = self;

    self.layerContentsRedrawPolicy = NSViewLayerContentsRedrawDuringViewResize;
    self.postsBoundsChangedNotifications = YES;
    self.postsFrameChangedNotifications = YES;
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(viewGlobalFrameDidChange:) name:NSViewGlobalFrameDidChangeNotification object:self];

    NSTrackingAreaOptions options = NSTrackingActiveAlways | NSTrackingInVisibleRect | NSTrackingMouseEnteredAndExited | NSTrackingMouseMoved;
    NSTrackingArea *trackingArea = [[NSTrackingArea alloc] initWithRect:self.bounds
                                                                options:options
                                                                  owner:self
                                                               userInfo:nil];

    [self addTrackingArea:trackingArea];

    _llCefLib = new LLCEFLib();

    _llCefLib->setOnPageChangedCallback(boost::bind(onPageChangedCallback, _1, _2, _3, _4, _5, _6));
    _llCefLib->setOnRequestExitCallback(boost::bind(onRequestExitCallback));

    LLCEFLib::LLCEFLibSettings settings;
    settings.initial_width = 1024;
    settings.initial_height = 1024;
    settings.javascript_enabled = true;
    settings.cache_enabled = true;
    settings.cache_path = "/tmp/cef_cachez";
    settings.cookies_enabled = true;
    settings.cookie_store_path = "/tmp/cef_cookiez";
    settings.user_agent_substring = "osxgl";
    settings.accept_language_list = "en-us";

    bool result = _llCefLib->init(settings);
    if (result) {
        _llCefLib->navigate("https://callum-linden.s3.amazonaws.com/ceftests.html");

        _timer = [[NSTimer scheduledTimerWithTimeInterval:0.016
                                                   target:self
                                                 selector:@selector(update:)
                                                 userInfo:nil
                                                  repeats:YES] retain];
    } else {
        NSLog(@"Failed to init llCefLib.");
        delete _llCefLib;
        _llCefLib = NULL;
    }
}

- (void)dealloc {
    [[NSNotificationCenter defaultCenter] removeObserver:self];

    [_timer invalidate];
    [_timer release];
    _timer = nil;

    gCurrent = nil;

    delete _llCefLib;
    _llCefLib = NULL;

    [super dealloc];
}

- (BOOL)acceptsFirstResponder {
    return YES;
}

- (void)flagsChanged:(NSEvent *)theEvent {
    if (self.llCefLib) {
        self.llCefLib->nativeKeyboardEventOSX(theEvent);
    }
}

- (void)keyDown:(NSEvent *)theEvent {
    if (self.llCefLib) {
        self.llCefLib->nativeKeyboardEventOSX(theEvent);
    }
}

- (void)keyUp:(NSEvent *)theEvent {
    if (self.llCefLib) {
        self.llCefLib->nativeKeyboardEventOSX(theEvent);
    }
}

- (void)mouseDown:(NSEvent *)theEvent {
    if (self.llCefLib) {
        const int x = [self getTextureMouseX:theEvent];
        const int y = [self getTextureMouseY:theEvent];
        self.llCefLib->mouseButton(LLCEFLib::MB_MOUSE_BUTTON_LEFT, LLCEFLib::ME_MOUSE_DOWN, x, y);
        self.llCefLib->setFocus(true);
    }
}

- (void)mouseDragged:(NSEvent *)theEvent {
    if (self.llCefLib) {
        const int x = [self getTextureMouseX:theEvent];
        const int y = [self getTextureMouseY:theEvent];
        self.llCefLib->mouseMove(x, y);
    }
}

- (void)mouseUp:(NSEvent *)theEvent {
    if (self.llCefLib) {
        const int x = [self getTextureMouseX:theEvent];
        const int y = [self getTextureMouseY:theEvent];
        self.llCefLib->mouseButton(LLCEFLib::MB_MOUSE_BUTTON_LEFT, LLCEFLib::ME_MOUSE_UP, x, y);
    }
}

- (void)mouseMoved:(NSEvent *)theEvent {
    if (self.llCefLib) {
        const int x = [self getTextureMouseX:theEvent];
        const int y = [self getTextureMouseY:theEvent];
        self.llCefLib->mouseMove(x, y);
    }
}

- (void)rightMouseUp:(NSEvent *)theEvent {
    if (self.llCefLib) {
    }
}

- (void)scrollWheel:(NSEvent *)theEvent {
    if (self.llCefLib) {
        const int dx = 3 * theEvent.deltaX;
        const int dy = 3 * theEvent.deltaY;
        self.llCefLib->mouseWheel(dx, dy);
    }
}

- (void)viewGlobalFrameDidChange:(NSNotification *)notification {
    [self.openGLContext update];
}

- (void)update:(NSTimer *)timer {
    if (self.llCefLib) {
        if (self.needsShutdown) {
            [self.timer invalidate];
            [self.timer release];
            self.timer = nil;

            [self.window close];

            self.llCefLib->shutdown();

            [[NSApplication sharedApplication] terminate:self];
        } else {
            self.llCefLib->update();
        }
    }
}

- (void)onPageChangedCallbackPixels:(unsigned char *)pixels x:(int)x y:(int)y width:(int)width height:(int)height is_popup:(bool)is_popup {
    if (width != textureWidth) {
        NSLog(@"onPageChagnedCallback width does not match.");
        return;
    }
    if (height != textureHeight) {
        NSLog(@"onPageChangedCallback height does not match.");
        return;
    }

    if (self.openGLContext) {
        [self.openGLContext makeCurrentContext];

        glBindTexture(GL_TEXTURE_2D, textureHandle);

        glTexSubImage2D(GL_TEXTURE_2D, // target
                        0, // level
                        x, // xoffset
                        y, // yoffset
                        width, // width
                        height, // height
                        GL_BGRA_EXT, // format
                        GL_UNSIGNED_BYTE, // type
                        pixels); // pixels

        [self setNeedsDisplay:YES];
    }
}

- (void)drawRect:(NSRect)dirtyRect {
    if (!self.openGLContext) {
        NSOpenGLPixelFormatAttribute attrs[] = {
            NSOpenGLPFADoubleBuffer,
            NSOpenGLPFADepthSize, 32,
            0
        };

        NSOpenGLPixelFormat *openGLPixelFormat = [[NSOpenGLPixelFormat alloc] initWithAttributes:attrs];

        NSOpenGLContext *openGLContext = [[NSOpenGLContext alloc] initWithFormat:openGLPixelFormat
                                                                    shareContext:nil];

        [openGLContext setView:self];

        [openGLContext makeCurrentContext];

        glGenTextures(1, &textureHandle);
        glBindTexture(GL_TEXTURE_2D, textureHandle);
        glTexImage2D(GL_TEXTURE_2D, // target
                     0, // level
                     GL_RGBA, // internalformat
                     textureWidth, // width
                     textureHeight, // height
                     0, // border
                     GL_RGBA, // format
                     GL_UNSIGNED_BYTE, // type
                     NULL); // pixels

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glViewport(0, 0, NSWidth(self.bounds), NSHeight(self.bounds));
        glOrtho(0.0, textureWidth, textureHeight, 0.0, -1.0, 1.0);

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        self.openGLContext = openGLContext;
    }

    [self.openGLContext makeCurrentContext];

    glClear(GL_COLOR_BUFFER_BIT);

    glBindTexture(GL_TEXTURE_2D, textureHandle);
    glEnable(GL_TEXTURE_2D);
    glColor3f(1.0f, 1.0f, 1.0f);

    glBegin(GL_QUADS);

        glTexCoord2f(1.0f, 1.0f);
        glVertex2d(textureWidth, 0.0f);

        glTexCoord2f(0.0f, 1.0f);
        glVertex2d(0.0f, 0.0f);

        glTexCoord2f(0.0f, 0.0f);
        glVertex2d(0.0f, textureHeight);

        glTexCoord2f(1.0f, 0.0f);
        glVertex2d(textureWidth, textureHeight);

    glEnd();

    [self.openGLContext flushBuffer];
}

#pragma mark - NSWindowDelegate

- (BOOL)windowShouldClose:(id)sender {
    if (!self.isClosing) {
        self.isClosing = YES;
        if (self.llCefLib) {
            self.llCefLib->requestExit();
        }
        return NO;
    } else {
        return YES;
    }
}

#pragma mark - Private Methods

- (int)getTextureMouseX:(NSEvent *)theEvent {
    NSPoint locationInView = [self convertPoint:theEvent.locationInWindow fromView:nil];
    return textureWidth * locationInView.x / NSWidth(self.bounds);
}

- (int)getTextureMouseY:(NSEvent *)theEvent {
    NSPoint locationInView = [self convertPoint:theEvent.locationInWindow fromView:nil];
    return textureHeight - (textureHeight * locationInView.y / NSHeight(self.bounds));
}

-(IBAction)setCookie:(id)sender {
    if (self.llCefLib) {
        self.llCefLib->setCookie("http://sasm.com", "my_cookie_sasm", "my_cookie_value_sasm", ".sasm.com", "/", true, true);
    }
}


-(IBAction)showDevTools:(id)sender {
    if (self.llCefLib) {
        self.llCefLib->showDevTools(true);
    }
}

@end
