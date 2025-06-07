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

#include "gl/gl.h"
#include "pane/rectPane.h"
#include "pane/tabPane.h"



/***** Window
 * DrawNavigationで自らの中心に対応した点を打つ
 * コンテントはtabかセパレートパネルで持つ
 * 基本的にユーザがWidgetをまとめて操作するためのもの
 */
struct Window : RectPane {
	Window(
		const tb::Vector<2, int>& center, const tb::Spread<2, unsigned>& size);

private:
	tb::Vector<2, float> center;

	void DrawNavigation() final {
		tb::Vector<2, float> p(center - lookingPoint);
		const float norm(p.Norm());
		if (norm < navigator.innerRadious) {
			// ナビゲーションサークルより内側なので輝点は表示しない
			return;
		}

		const float r(
			navigator.innerRadious
			+ navigator.thickness / (norm - navigator.innerRadious));
		const tb::Vector<2, float> pp(p * r / norm);

		glVertex2f(pp[0], pp[1]); // glBegin/glEndは処理全体
	};
};
