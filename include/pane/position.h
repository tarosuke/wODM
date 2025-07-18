/*****************************************************************************
 * Copyright (C) 2025 tarosuke<webmaster@tarosuke.net>
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

#include <tb/prefs.h>
#include <tb/vector.h>



/***** Paneの位置およびその制御
 */
struct Position {
	using P = tb::Vector<2, float>;

protected:
	Position(const P& p) : position(p), target(p) {};

	void UpdatePosition() {
		position += (target - position) * (float)movingRatio;
	};
	const P& Get() { return position; };

	void MoveTo(const P& p) { target = p; };
	void JumpTo(const P& p) { target = position = p; };

	void SetDepth(float d, float t) {
		target[2] = d;
		thickness = t;
	};
	float GetThickness() { return thickness; };

private:
	static tb::Prefs<float> movingRatio;

	P position;
	P target;
	float thickness; // 当該Paneの厚さ(使える奥行の範囲)

	Position() = delete;
};
