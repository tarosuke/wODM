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

#include "gl/gl.h"
#include <X11/Xlib.h>



/** GLXコンテキストのラッパ
 * 構築したらMakeCurrentでカレントに設定できる
 */
struct GLX {
	GLX() = delete;
	void operator=(const GLX&) = delete;

	GLX(Display* display,
		Drawable drawable = 0,
		int attributes[] = defaultAttributes)
		: display(display),
		  drawable(drawable ? drawable : DefaultRootWindow(display)) {
		visual = glXChooseVisual(display, DefaultScreen(display), attributes);
		context = glXCreateContext(display, visual, NULL, True);
	};

	~GLX() {
		XFree(visual);
		glXMakeCurrent(display, None, NULL); // コンテキスト解除
		glXDestroyContext(display, context); // コンテキスト破棄
	};

	// コンテキストをカレントにする
	void MakeCurrent() { glXMakeCurrent(display, drawable, context); };

	Display* GetDisplay() { return display; };

private:
	static int defaultAttributes[];

	Display* display;
	Drawable drawable;
	XVisualInfo* visual;
	GLXContext context;
};
