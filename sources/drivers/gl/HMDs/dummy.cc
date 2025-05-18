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
#include <syslog.h>

#undef Complex
#include <tb/complex.h>
#include <tb/factory.h>
#include <tb/input.h>
#include <tb/linux/input.h>
#include <tb/prefs.h>

#include "gl/glx.h"



/***** 動作確認のためのダミー画面
 * 単眼、小さめの窓を作ってそこへ出力
 * 開発が進むと使わなくなるんだろうが、VRHMDの脱着を繰り返す面倒を回避する。
 */
class DummyHMD : GLX, tb::linux::Input {
	static const tb::Spread<2, unsigned> size;
	const int screen;
	Window window;
	unsigned eIndex;
	Atom wmDeleteNotify;

	Eyes eyes;

	DummyHMD() :
		GLX(eyes),
		screen(DefaultScreen(display)),
		window(XCreateSimpleWindow(
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
		Setup(window);

		XSelectInput(
			display, window, StructureNotifyMask | SubstructureNotifyMask);
		XSetStandardProperties(
			display, window, "Sample", "Sample", None, 0, 0, 0);
		// WondowManagerが勝手に窓を閉じる前に通知させる
		XSetWMProtocols(display, window, &wmDeleteNotify, 1);

		XMapWindow(display, window);

		static constexpr double near = 0.05;
		static constexpr double far = 10000.0;
		static constexpr double width = 1.0;
		static constexpr double w = width * near;
		static const double h = w * size[1] / size[0];

		Eye eye;
		eye.width = size[0];
		eye.height = size[1];
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glFrustum(-w, w, -h, h, near, far);
		glGetFloatv(GL_PROJECTION_MATRIX, eye.projection);

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		eye.eye2Head.Identity();
		eyes.emplace_back(std::move(eye));
	};

	void UpdatePose() final { GetInput(); };
	void Finish(Eye&) final {
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
	};
	void Finish() final { glFinish(); };

	static tb::Prefs<bool> useDummyHMD;
	static class Factory : tb::Factory<Core> {
		uint Score() {
			return DummyHMD::useDummyHMD ? Certitude::uniqueMatch : 0;
		};
		Core* New() { return new DummyHMD; };
	} factory;

	void OnAbsMoved(const tb::Timestamp&, const AxisReport& a) final {
		if (a.moved & 0x18) {
			// 右スティッが動かされた
			const float angle[3] = {(float)a.value[4], (float)a.value[3], 0};
			tb::Complex<4, float> qon(angle, M_PI / 32769);
			pose = qon;

#if 0
			printf("%+6d %+6d\n", a.value[3], a.value[4]);
#endif
#if 0
			printf("%5f; %+5f %+5f %+5f\n", qon[0], qon[1], qon[2], qon[3]);
#endif
#if 0
			for (unsigned r(0); r < 4; ++r) {
				for (unsigned c(0); c < 4; ++c) {
					printf("%s%+5f ", !c ? " " : "", view[r][c]);
				}
			}
			puts("");
#endif
		}
	};
};


tb::Prefs<bool> DummyHMD::useDummyHMD(
	"--GLDummyHMD", false, "ダミーのHMDを使う", tb::CommonPrefs::nosave);
DummyHMD::Factory DummyHMD::factory;
const tb::Spread<2, unsigned> DummyHMD::size(1280u, 720u);
