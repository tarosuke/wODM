/*****************************************************************************
 * Copyright (C) 2026 tarosuke<webmaster@tarosuke.net>
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

#include "gl/eye.h"
#include "gl/glx.h"
#include <tb/complex.h>
#include <tb/geometry/vector.h>



/***** 動作確認のためのダミー画面
 * 単眼、小さめの窓を作ってそこへ出力
 * 開発が進むと使わなくなるんだろうが、VRHMDの脱着を繰り返す面倒を回避する。
 */
struct DummyHMD : GLX {
	using P = tb::geometry::Vector<2, int>;

	static const tb::geometry::Spread<2, unsigned> size;
	const int screen;
	::Window window;
	unsigned eIndex;
	Atom wmDeleteNotify;

	DummyHMD();

	void UpdatePose() final;
	void Finish(const Eye&) final;
	void Finish() final;

	static constexpr int cursorMoveRatio = 5;
	P target;
	P position;
	void MoveCursorTo(const P&);
};
