/*
 * Copyright 2010, The Android Open Source Project
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef BaseTile_h
#define BaseTile_h

#if USE(ACCELERATED_COMPOSITING)

#include "HashMap.h"
#include "SkBitmap.h"
#include "SkCanvas.h"
#include "SkRect.h"

#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <GLES2/gl2.h>

namespace WebCore {

class BackedDoubleBufferedTexture;
class TiledPage;

/**
 * An individual tile that is used to construct part of a webpage's BaseLayer of
 * content.  Each tile is assigned to a TiledPage and is responsible for drawing
 * and displaying their section of the page.  The lifecycle of a tile is:
 *
 * 1. Each tile is created on the main GL thread and assigned to a specific
 *    location within a TiledPage.
 * 2. When needed the tile is passed to the background thread where it paints
 *    the BaseLayer's most recent PictureSet to a bitmap which is then uploaded
 *    to the GPU.
 * 3. After the bitmap is uploaded to the GPU the main GL thread then uses the
 *    tile's draw() function to display the tile to the screen.
 * 4. Steps 2-3 are repeated as necessary.
 * 5. The tile is destroyed when the user navigates to a new page.
 *
 */
class BaseTile {
public:
#ifdef DEBUG_COUNT
    static int count();
#endif
    BaseTile(TiledPage* page, int x, int y);
    ~BaseTile();

    void reserveTexture();
    void removeTexture();
    void setUsedLevel(int);
    bool isBitmapReady();
    void draw(float transparency, SkRect& rect);

    // the only thread-safe function called by the background thread
    void paintBitmap();

    float scale() const { return m_scale; }
    void setScale(float scale) { m_scale = scale; }

    TiledPage* page() { return m_page; }
    int x() const { return m_x; }
    int y() const { return m_y; }
    BackedDoubleBufferedTexture* texture() { return m_texture; }

private:
    // these variables are only set when the object is constructed
    TiledPage* m_page;
    int m_x;
    int m_y;

    // these variables can be updated throughout the lifetime of the object
    BackedDoubleBufferedTexture* m_texture;
    float m_scale;

    // used to ensure the variables are consistent between threads
    android::Mutex m_varLock;
};

} // namespace WebCore

#endif // USE(ACCELERATED_COMPOSITING)
#endif // BaseTile_h
