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
#include "pane/canvasPane.h"



///// CanvasPane
void CanvasPane::OnCanvasUpdated(const tb::Rect<2, double>& from) {
	tb::Canvas::Image image(*this);
	const tb::Rect<2, unsigned> f(from);

	const unsigned step(
		maxFragmentPixels <= f.GetSpread()[0]
			? 1
			: maxFragmentPixels / f.GetSpread()[0]);
	const unsigned bottom(f.Right()[1]);

	for (unsigned y(f.Left()[1]); y < bottom; y += step) {
		const S fs(f.GetSpread()[0], y + step <= bottom ? step : bottom - y);
		const S ff(from.Left()[0], y);

		AddFragment(*new Fragment(
			image, tb::Vector<2, int>(from.Left()[0], (int)y), fs));
	}
}
