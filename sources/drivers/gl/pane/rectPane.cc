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

#include "pane/rectPane.h"



// Geometry
tb::Prefs<float>
	Geometry::moveRatio("RectPane/MoveRatio", 0.25, "Widget移動速度の係数");


// RectPane

void RectPane::Draw(const R& r) {
	const R rr(r & Rect());
	if (!rr.IsEmpty()) {
		Widget::Draw(rr - LeftTop());
	}
}
void RectPane::Traw(const R& r) {
	const R rr(r & Rect());
	if (!rr.IsEmpty()) {
		Widget::Traw(rr - LeftTop());
	}
}
