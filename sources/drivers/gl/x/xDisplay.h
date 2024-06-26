/** X Display
 * Copyright (C) 2016,2019,2024 tarosuke<webmaster@tarosuke.net>
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

#if 0
#include "gl/gl.h"
#include "gl/glx.h"
#include <sys/types.h>
#include <tb/list.h>



/** 表示用画面
 */
struct XDisplay {
	XDisplay();
	XDisplay(const XDisplay&) = delete;
	void operator=(const XDisplay&) = delete;

public:
	XDisplay(const Profile&);
	~XDisplay();

	void Run();
	void Update();

	// 画面の状況
	struct Spec {
		::Display* display;
		::Window root;
		::Window wODMroot;
		GLX* rootGLX;
		int xOffset;
		int yOffset;
		unsigned width;
		unsigned height;
	};
	static const Spec& GetSpec() { return spec; };

	static void RegisterModule(XModule&);

private:
	static ::Display* display;
	static Window root;

	int pid; // DM動作時のX鯖

	// ローカルX用
	static Spec spec;
	static int XErrorHandler(::Display*, XErrorEvent*);
};
#endif
