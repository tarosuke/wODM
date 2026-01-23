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
		Pane(Frame& parent, tb::Color c, const R3&);
		Pane(tb::Color c, const R3&);

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
		/***** Imageからテクスチャへ転送して生成
		 * 1. Pane自体の位置や大きさはrectによる
		 * 2. テクスチャサイズはimageのサイズと同じ
		 * ※imageは保持されないのでUpdateするときは都度与える
		 */
		TexturePane(Frame& parent, const R3& rect, const tb::Image& image);
		template <typename U>
		TexturePane(Frame& parent,
			const tb::geometry::Rect<3, U>& rect,
			const tb::Image& image) :
			TexturePane(parent, P({rect.Origin(1), rect.Origin(1)}), image){};

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
		// 更新範囲(OnCanvasUpdateで受けたRectを細分化して保存しUpdateで更新)
		// 分割は転送単位を最大化するために行単位
		struct UpdateRect : tb::geometry::Rect<2, unsigned>,
							tb::List<UpdateRect> {};

		CanvasPane(Frame& parent, const R3& rect) :
			Canvas(rect.Spread()),
			TexturePane(parent, rect, Canvas::Image(*this)) {};

	private:
		static constexpr unsigned nUpdatePixel = 64 * 64;

		tb::List<UpdateRect> updates;
		void OnCanvasUpdated(const tb::geometry::Rect<2, double>&) override;
	};


	/***** 一行入力
	 * IMなしの一行入力コントロール
	 */
	struct LineInputPane : CanvasPane {
		enum Style { normal, password, visibleLastPassword };
		LineInputPane(Frame& parent,
			const R3& rect,
			unsigned fontSize,
			const char* prompt = "");

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
