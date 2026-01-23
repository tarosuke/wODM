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
#include "widget/prefs.h"
#include "widget/root.h"



namespace widget {

	const Window::Params Window::defaultParams = {
		.leftTopColor = tb::Color(Prefs::foreColor),
		.rightBottomColor = tb::Color(Prefs::backColor),
		.mergin{.left = 2.0f, .top = 2.0f, .right = 2.0f, .bottom = 2.0f}};



	// childを子要素として生成
	Window::Window(Frame& c, const Params& params) :
		Pane(tb::Color(Prefs::backColor), MakeRect(c, params)),
		mergin{params.mergin},
		leftTopColor(params.leftTopColor),
		rightBottomColor(params.rightBottomColor) {
		c.JumpTo(P({(float)params.mergin.left, (float)params.mergin.top}));
		children.Insert(c);
		UpdateBorder();
		Root::Register(*this);
		Sort();
	}

	Frame::R3 Window::MakeRect(const Frame& c, const Params& p) {
		return ToR3(R(P({(-0.5f * c.Spread()[0]) - p.mergin.left,
						  (-0.5f * c.Spread()[1]) - p.mergin.top}),
			c.Spread() + S({p.mergin.right, p.mergin.bottom})));
	}

	Notify Window::Update() {
		if (children.Top()) {
			return Frame::Update();
		}
		delete this;
		return Notify(); // TODO:この場合の戻り値を決める
	};

	void Window::UpdateBorder() {
		in.left = mergin.left;
		in.top = mergin.top;
		in.right = Spread()[0] - mergin.right;
		in.bottom = Spread()[1] - mergin.bottom;
		out.left = 0;
		out.top = 0;
		out.right = Spread()[0];
		out.bottom = Spread()[1];
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

		Pane::Draw(r);
	}

	void Window::Dot() { Root::Dot(GetCenter()); }

	const tb::Color ResizeableWindow::defaultLeftTopColor(0);
	const tb::Color ResizeableWindow::defaultRightBottomColor(0xffffff);


	void Window::Sort() {
		float d(rect.Spread(2));
		float dd(d * 0.5f);
		for (tb::List<Frame>::I i(children); ++i; d = dd, dd *= 0.5f) {
			(*i).ReDepth(d, dd);
		}
	}
}
