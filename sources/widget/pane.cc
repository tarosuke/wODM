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
#include "widget/prefs.h"
#include "widget/window.h"



namespace widget {

	Pane::Pane(Frame& parent, tb::Color c, const P& position, const S& spread) :
		Frame(parent, position, spread),
		color(c),
		draw(c.IsTranslucent() ? &Pane::DummyDraw : &Pane::DrawHandler),
		traw(c.IsTranslucent() ? &Pane::TrawHandler : &Pane::DummyTraw) {}
	Pane::Pane(tb::Color c, const S& spread) :
		Frame(P{0.0f, 0.0f}, spread),
		color(c),
		draw(c.IsTranslucent() ? &Pane::DummyDraw : &Pane::DrawHandler),
		traw(c.IsTranslucent() ? &Pane::TrawHandler : &Pane::DummyTraw) {}
	void Pane::DrawHandler(const R& r) {
		const auto& m(GetMask());
		glColor4fv(color);
		glBegin(GL_TRIANGLE_STRIP);
		Vertex(m.Left()[0], m.Left()[1]);
		Vertex(m.Right()[0], m.Left()[1]);
		Vertex(m.Left()[0], m.Right()[1]);
		Vertex(m.Right()[0], m.Right()[1]);
		glEnd();
	}
	void Pane::TrawHandler() {
		const auto& m(GetMask());
		glColor4fv(color);
		glBegin(GL_TRIANGLE_STRIP);
		Vertex(m.Left()[0], m.Left()[1]);
		Vertex(m.Right()[0], m.Left()[1]);
		Vertex(m.Left()[0], m.Right()[1]);
		Vertex(m.Right()[0], m.Right()[1]);
		glEnd();
	}


	TexturePane::TexturePane(
		Frame& parent, const P& position, const tb::Image& image) :
		Pane(parent,
			tb::Color(0xffffff),
			position,
			S{image.Spread()[0], image.Spread()[1], 0U}),
		Texture(image),
		hpc(1.0f / spread[0]),
		vpc(1.0f / spread[1]) {
		Binder b(*this);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.Spread()[0],
			image.Spread()[1], 0, GL_RGBA, GL_BYTE, image.Data());
	};
	void TexturePane::Draw(const R& r) {
		Texture::Binder b(*this);
		(this->*draw)(r);
	};
	void TexturePane::Traw() {
		Texture::Binder b(*this);
		(this->*traw)();
	};



	void CanvasPane::OnCanvasUpdated(const tb::Rect<2, double>& r) {
		// TODO:rが大きすぎるとヒッチングの原因になるし、タイミングが悪いと更新されないので直接には更新せず変更を分解してキューイングしたいところ
		const unsigned w(r.GetSpread()[0]);
		const unsigned h(r.GetSpread()[1]);
		tb::BufferedImage image(
			tb::Canvas::Image(*this), r.Left()[0], r.Left()[1], w, h);
		Binder b(*this);
		glTexSubImage2D(GL_TEXTURE_2D, 0, r.Left()[0], r.Left()[1], w, h,
			GL_RGBA, GL_BYTE, image.Data());
	}



	LineInputPane::LineInputPane(Frame& parent,
		const P& position,
		const S& spread,
		unsigned fontSize,
		const char* prompt) :
		CanvasPane(parent, position, spread),
		fontSize(fontSize),
		prompt(prompt),
		carret(0) {
		// 初期敵内容の描画
		Canvas::GC gc(*this);
#if 1
		gc.Clear(tb::Color(Prefs::backColor));
#else
		gc.Set(tb::Color(Prefs::backColor), tb::Color(Prefs::backColor));
		gc.Rectangle(0.5, 0.5, spread[0] - 0.5, spread[1] - 0.5);
#endif
		// TODO:下線などの装飾、promptを描画＆カーソルの設定
	}
	void LineInputPane::OnKeyDown(const KeyEvent& e) {}
	void LineInputPane::OnClick(const PtEvent& e) {}
}
