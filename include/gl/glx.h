/*****************************************************************************
 * Copyright (C) 2017,2024 tarosuke<webmaster@tarosuke.net>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; either version 3
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */
#pragma once

#include "core.h"
#include "gl/gl.h"
#include <X11/Xlib.h>



/** GLXコンテキストのラッパ
 * 構築したらMakeCurrentでカレントに設定できる
 */
struct GLX : Core {
	void operator=(const GLX&) = delete;

	GLX() : display(XOpenDisplay(0)) {};

	~GLX() {
		glXMakeCurrent(display, None, 0);
		XFree(visual);
		glXDestroyContext(display, context);
		XSetCloseDownMode(display, DestroyAll);
		XCloseDisplay(display);
	};

protected:
	Display* const display;

	void Setup(Drawable drawable, int attributes[] = defaultAttributes) {
		visual = glXChooseVisual(display, DefaultScreen(display), attributes);
		context = glXCreateContext(display, visual, 0, True);
		glXMakeCurrent(display, drawable, context);

		// glew初期化
		if (GLEW_OK != glewInit()) {
			throw "GLEWが使えません";
		}

		// 基本設定
		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_POLYGON_SMOOTH);
		glEnable(GL_CULL_FACE);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_TEXTURE_2D);
	};

private:
	static int defaultAttributes[];

	XVisualInfo* visual;
	GLXContext context;
};
