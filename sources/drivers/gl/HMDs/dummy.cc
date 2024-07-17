/*****************************************************************************
 * Copyright (C) 2024 tarosuke<webmaster@tarosuke.net>
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
#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include <tb/factory.h>
#include <tb/input.h>
#include <tb/prefs.h>

#include "core.h"
#include "gl/gl.h"
#include "gl/glx.h"



/***** 動作確認のためのダミー画面
 * 単眼、小さめの窓を作ってそこへ出力
 * 開発が進むと使わなくなるんだろうが、VRHMDの脱着を繰り返す面倒を回避する。
 */
class DummyHMD : GLX {
	static const tb::Spread<2, unsigned> size;
	::Window window;
	unsigned eIndex;
	tb::Matrix<4, 4, float> projection;
	tb::Matrix<4, 4, float> view;
	Atom wmDeleteNotify;

	struct F : tb::Factory<Core> {
		uint Score() final { return useDummyHMD ? ~0 : 0; };
		Core* New() final { return new DummyHMD; };
	};

	DummyHMD()
		: GLX(), window(XCreateSimpleWindow(
					 display,
					 RootWindow(display, screen),
					 0,
					 0,
					 size[0],
					 size[1],
					 1,
					 BlackPixel(display, screen),
					 BlackPixel(display, screen))),
		  eIndex(0),
		  wmDeleteNotify(XInternAtom(display, "WM_DELETE_WINDOW", False)) {
		XSelectInput(
			display,
			window,
			StructureNotifyMask | SubstructureNotifyMask);
		XSetStandardProperties(
			display,
			window,
			"Sample",
			"Sample",
			None,
			0,
			0,
			0);
		// WondowManagerが勝手に窓を閉じる前に通知させる
		XSetWMProtocols(display, window, &wmDeleteNotify, 1);

		XMapWindow(display, window);
		SetDrawable(window);

		static constexpr double near = 0.05;
		static constexpr double far = 10000.0;
		static constexpr double width = 1.0;
		static constexpr double w = width * near;
		static const double h = w * size[1] / size[1];

		glMatrixMode(GL_PROJECTION);
		glFrustum(-w, w, -h, h, near, far);
		glGetFloatv(GL_PROJECTION, projection);

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		view.Identity();
	};

	const tb::Matrix<4, 4, float>& Pose() { return view; };
	const tb::Matrix<4, 4, float>* NextEye() final {
		return (!!(eIndex = !eIndex) ? &projection : 0);
	};
	void Finish() final {
		eIndex = 0;

		// Xのイベントを処理
		while (XPending(display)) {
			XEvent e;
			XNextEvent(display, &e);
			switch (e.type) {
			case DestroyNotify:
				// 窓が閉じられたので終了
				Quit();
				break;
			case ClientMessage:
				// WMが窓を閉じようとしている
				if ((Atom)e.xclient.data.l[0] == wmDeleteNotify) {
					Quit();
				}
				break;
			default:
				break;
			}
		}

		// 窓へ出力
		glFinish();
	};

	static tb::Prefs<bool> useDummyHMD;
	static class Factory : tb::Factory<Core> {
		uint Score() {
			return DummyHMD::useDummyHMD ? Certitude::passiveMatch : 0;
		};
		Core* New() { return new DummyHMD; };
	} factory;
};


tb::Prefs<bool> DummyHMD::useDummyHMD(
	"--GLDummyHMD", "ダミーのHMDを使う", tb::CommonPrefs::nosave);
DummyHMD::Factory DummyHMD::factory;
const tb::Spread<2, unsigned> DummyHMD::size(1280u, 720u);
