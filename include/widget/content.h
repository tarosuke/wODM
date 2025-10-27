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

	// 空のコンテント
	struct Content {
		Content() = default;
		virtual void DrawContent(const Frame::R&) {};
		virtual void TrawContent(const Frame::R&) {};
	};

	struct PlaneContent : Content {
		PlaneContent(tb::Color = defaultColor);
		void DrawContent(const Frame::R&) override;

	protected:
		static const tb::Color defaultColor;
		tb::Color color;

	private:
		void Vertex(float x, float y) { glVertex2f(x, y); };
	};

	struct TextureContent : PlaneContent, GL::Texture {
		TextureContent(unsigned width,
			unsigned height,
			GL::Texture::Format format = GL::Texture::RGB);
		void DrawContent(const Frame::R&) override;

	protected:
		// サイズの逆数
		const float hpc;
		const float vpc;

		void Vertex(float x, float y) {
			glTexCoord2f(x * hpc, y * vpc);
			glVertex2f(x, y);
		};
	};
}
