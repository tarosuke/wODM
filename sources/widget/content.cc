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
#include "widget/content.h"
#include "gl/gl.h"



namespace widget {


	/***** Content類
	 */
	const tb::Color PlaneContent::defaultColor(0x00ffffff);
	PlaneContent::PlaneContent(tb::Color c) : color(c) {}
	void PlaneContent::DrawContent(const Frame::R& r) {
		glColor4fv(color);
		glBegin(GL_TRIANGLE_STRIP);
		Vertex(r.Left()[0], r.Left()[1]);
		Vertex(r.Right()[0], r.Left()[1]);
		Vertex(r.Right()[0], r.Right()[1]);
		glEnd();
	}


	TextureContent::TextureContent(
		unsigned w, unsigned h, GL::Texture::Format f) :
		Texture(w, h, f),
		hpc(1.0f / w),
		vpc(1.0f / h) {}

	void TextureContent::DrawContent(const Frame::R& r) {
		Binder b(*this);
		glColor4fv(color);
		glBegin(GL_TRIANGLE_STRIP);
		Vertex(r.Left()[0], r.Left()[1]);
		Vertex(r.Right()[0], r.Left()[1]);
		Vertex(r.Right()[0], r.Right()[1]);
		glEnd();
	}
}
