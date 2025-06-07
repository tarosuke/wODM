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
#include <algorithm>



///// CanvasPane
void CanvasPane::OnCanvasUpdated(const tb::Rect<2, double>& from) {
	tb::Canvas::Image image(*this);
	const tb::Rect<2, unsigned> f(from);

	// 一度に転送する高さと一番下
	const unsigned step(std::max(maxFragmentPixels / f.GetSpread()[0], 1U));
	const unsigned bottom(from.Right()[1]);

	S fragmentSpread(f.GetSpread()[0], step);

	Fragment* (*newFragment)(
		const tb::Image&, const tb::Vector<2, unsigned>&, const S&) =
		from.GetSpread()[0] == image.Width() ? NewFragment
											 : NweBufferedFragment;

	for (unsigned y(f.Left()[1]); y < bottom; y += step) {
		if (bottom < y + step) {
			fragmentSpread[1] = bottom - y;
		}

		AddFragment(
			*newFragment(image, tb::Vector<2, unsigned>(0, y), fragmentSpread));
	}
}

CanvasPane::Fragment* CanvasPane::NewFragment(
	const tb::Image& image,
	const tb::Vector<2, unsigned>& offset,
	const S& size) {
	return new Fragment(image, offset, size);
}
CanvasPane::Fragment* CanvasPane::NweBufferedFragment(
	const tb::Image& image,
	const tb::Vector<2, unsigned>& offset,
	const S& size) {
	return new BufferedFragment(image, offset, size);
}
