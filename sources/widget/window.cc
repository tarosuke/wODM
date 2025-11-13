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
#include "widget/window.h"
#include "gl/gl.h"
#include "widget/root.h"



namespace widget {

	Window::Window(const P& p,
		const S& s,
		float depth,
		float thick,
		const tb::Color& ltc,
		const tb::Color& rbc,
		unsigned lm,
		unsigned tm,
		unsigned rm,
		unsigned bm) :
		Frame(p - s * 0.5, s, depth, thick),
		mergin{.left = (float)lm,
			.top = (float)tm,
			.right = (float)rm,
			.bottom = (float)bm},
		leftTopColor(ltc),
		rightBottomColor(rbc) {
		UpdateBorder();
		Root::Register(*this);
	};

	void Window::UpdateBorder() {
		in.left = 0;
		in.top = 0;
		in.right = (float)spread[0];
		in.bottom = (float)spread[1];
		out.left = -mergin.left;
		out.top = -mergin.top;
		out.right = in.right + mergin.right;
		out.bottom = in.bottom + mergin.bottom;
	}

	void Window::Draw(const R& r) {
		// 窓コントロールの描画
		glBegin(GL_QUADS);
		glColor3f(1, 1, 1);
		glVertex2f(out.left, out.bottom);
		glVertex2f(out.left, out.top);
		glColor3fv(leftTopColor);
		glVertex2f(in.left, in.top);
		glVertex2f(in.left, in.bottom);

		glColor3f(1, 1, 1);
		glVertex2f(out.left, out.top);
		glVertex2f(out.right, out.top);
		glColor3fv(leftTopColor);
		glVertex2f(in.right, in.top);
		glVertex2f(in.left, in.top);

		glColor3fv(rightBottomColor);
		glVertex2f(in.left, in.bottom);
		glVertex2f(in.right, in.bottom);
		glColor3f(0, 0, 0);
		glVertex2f(out.right, out.bottom);
		glVertex2f(out.left, out.bottom);

		glColor3fv(rightBottomColor);
		glVertex2f(in.right, in.bottom);
		glVertex2f(in.right, in.top);
		glColor3f(0, 0, 0);
		glVertex2f(out.right, out.top);
		glVertex2f(out.right, out.bottom);
		glEnd();
	}

	void Window::Dot() { Root::Dot(GetCenter()); }

	const tb::Color ResizeableWindow::defaultLeftTopColor(0);
	const tb::Color ResizeableWindow::defaultRightBottomColor(0xffffff);
}
