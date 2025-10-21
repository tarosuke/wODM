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

	Frame::P Window::leftTopMergin;
	Frame::S Window::spreadMergin;


	Window::Window(const P& p, const S& s, float depth, float thick) :
		Frame(GetLeftTop(p, s), GetWindowSpread(s), depth, thick) {
		Root::Register(*this);
	};

	Frame::P Window::GetLeftTop(const P& center, const S& contentSpread) {
		return center - contentSpread * 0.5 - leftTopMergin;
	}

	Frame::S Window::GetWindowSpread(const S& contentSpread) {
		return contentSpread + spreadMergin;
	}

	void Window::Draw(const R& r) {
		UpdateMask(r);
		if (IsShown()) {
			glPushMatrix();
			glTranslatef(-position[0], -position[1], -depth);
			controls.Foreach(&Frame::Draw, GetMask());
			Frame::Draw(R(spread));
			glPopMatrix();
		}
	}
	void Window::Traw() {
		if (IsShown()) {
			glPushMatrix();
			glTranslatef(-position[0], -position[1], -depth);
			Frame::Traw();
			controls.Foreach(&Frame::Traw);
			glPopMatrix();
		}
	}

	void Window::Dot() { Root::Dot(GetCenter()); }

}
