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

#include "frame.h"
#include "gl/gl.h"
#include "gl/texture.h"
#include <tb/canvas.h>
#include <tb/color.h>
#include <tb/string.h>



namespace widget {

	// 平面
	struct Pane : Frame {
		Pane(Frame& parent, tb::Color c, const P& position, const S& spread);
		Pane(tb::Color c, const S& spread);

	protected:
		tb::Color color;
		void Draw(const R& r) override { (this->*draw)(r); };
		void Traw() override { (this->*traw)(); };
		virtual void Vertex(float x, float y) { glVertex2f(x, y); };
		void (Pane::*draw)(const R&);
		void (Pane::*traw)();
		void DrawHandler(const R&);
		void TrawHandler();
		void DummyDraw(const R&) {};
		void DummyTraw() {};
	};

	/***** テクスチャ付き
	 * Imageの参照を与えてテクスチャを作ってそれを表示
	 */
	struct TexturePane : Pane, GL::Texture {
		TexturePane(Frame& parent, const P& position, const tb::Image& image);

	protected:
		// サイズの逆数
		const float hpc;
		const float vpc;

		void Draw(const R& r) override;
		void Traw() override;

		void Vertex(float x, float y) override {
			glTexCoord2f(x * hpc, y * vpc);
			glVertex2f(x, y);
		};
	};

	/****** Canvas付き
	 * tb::CanvasとしてGCを作って描画できる
	 */
	struct CanvasPane : tb::Canvas, TexturePane {
		CanvasPane(Frame& parent, const P& position, const S& spread) :
			Canvas(spread[0], spread[1]),
			TexturePane(parent, position, Canvas::Image(*this)) {};
		CanvasPane(Frame& parent, const RR rect) :
			CanvasPane(parent, rect.position, rect.spread) {};

	private:
		void OnCanvasUpdated(const tb::Rect<2, double>&) override;
	};


	/***** 一行入力
	 * IMなしの一行入力コントロール
	 */
	struct LineInputPane : CanvasPane {
		enum Style { normal, password, visibleLastPassword };
		LineInputPane(Frame& parent,
			const P& position,
			const S& spread,
			unsigned fontSize,
			const char* prompt = "");
		LineInputPane(Frame& parent,
			const RR rect,
			unsigned fontSize,
			const char* prompt = "") :
			LineInputPane(
				parent, rect.position, rect.spread, fontSize, prompt) {};

	protected:
		void OnKeyDown(const KeyEvent&) override;
		void OnClick(const PtEvent&) override;

	private:
		const unsigned fontSize;
		const tb::String prompt;
		unsigned carret; // キャレット位置
		tb::String text; // 編集中の文字列
	};
}
