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
#include <tb/color.h>



namespace widget {

	// 平面
	struct Pane : Frame {
		Pane(tb::Color c,
			const P& position,
			const S& spread,
			float depth,
			float thick);
		Pane(Frame& parent,
			tb::Color c,
			const P& position,
			const S& spread,
			float depth,
			float thick) :
			Frame(position, spread, depth, thick),
			color(c) {};

	protected:
		tb::Color color;
		void Draw(const R& r) override { (this->*draw)(r); };
		void Traw() override { (this->*traw)(); };
		void Vertex(float x, float y) { glVertex2f(x, y); };
		void (Pane::*draw)(const R&);
		void (Pane::*traw)();
		void DrawHandler(const R&);
		void TrawHandler();
		void DummyDraw(const R&) {};
		void DummyTraw() {};
	};

	// テクスチャ
	struct TexturePane : Frame, GL::Texture {
		TexturePane(const P& position,
			const tb::Image& image,
			float depth,
			float thick) :
			Frame(position, image.Spread(), depth, thick),
			Texture(image),
			hpc(1.0f / spread[0]),
			vpc(1.0f / spread[1]) {};

	protected:
		void Draw(const R& r) override;
		void Traw() override;

		// サイズの逆数
		const float hpc;
		const float vpc;

		void Vertex(float x, float y) {
			glTexCoord2f(x * hpc, y * vpc);
			glVertex2f(x, y);
		};
	};
}
