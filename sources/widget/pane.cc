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
#include "widget/pane.h"
#include "gl/gl.h"
#include "widget/window.h"



namespace widget {

	Pane::Pane(tb::Color c,
		const P& position,
		const S& spread,
		float depth,
		float thick) :
		Frame(new Window(position, spread), position, spread, depth, thick),
		color(c) {}

	void Pane::Draw(const R& r) {
		if (UpdateMask(r)) {
			const auto& m(GetMask());
			Frame::Draw(m);
			glColor4fv(color);
			glBegin(GL_TRIANGLE_STRIP);
			Vertex(m.Left()[0], m.Left()[1]);
			Vertex(m.Right()[0], m.Left()[1]);
			Vertex(m.Right()[0], m.Right()[1]);
			glEnd();
		}
	};
	void Pane::Traw() {
		if (IsShown()) {
			const auto& m(GetMask());
			glColor4fv(color);
			glBegin(GL_TRIANGLE_STRIP);
			Vertex(m.Left()[0], m.Left()[1]);
			Vertex(m.Right()[0], m.Left()[1]);
			Vertex(m.Right()[0], m.Right()[1]);
			glEnd();
			Frame::Traw();
		}
	};
}
