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

#include "gl/texture.h"
#include "pane/rectPane.h"
#include <tb/canvas.h>



// 描画できるWidget
struct CanvasPane : public RectPane, public tb::Canvas, GL::Texture {
	static constexpr int maxFragmentPixels = 64 * 64;

	CanvasPane(const R&);
	CanvasPane(const S&);

protected:
	struct Fragment : tb::List<Fragment>::Node, public tb::BufferedImage {
		Fragment(
			const tb::Image& origin,
			const tb::Vector<2, int>& offset,
			const S&);
		const P offset;
	};
	void AddFragment(Fragment& f) { updates.Add(f); };

private:
	tb::List<Fragment> updates;
	void Update(const tb::Timestamp&) final;

	void OnCanvasUpdated(const tb::Rect<2, double>&) final;
	Fragment* GetFragment() { return updates.Get(); };
};
