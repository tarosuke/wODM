/*****************************************************************************
 * Copyright (C) 2024 tarosuke<webmaster@tarosuke.net>
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
#include "widget.h"
#include "core.h"
#include "gl/gl.h"
#include <tb/list.h>
#include <tb/matrix.h>
#include <tb/prefs.h>
#include <tb/time.h>



namespace {
	tb::Prefs<float> vDistance("widget/virtualDistance", 1.0f);
}


tb::List<Widget> Widget::root;
tb::Vector<2, float> Widget::lookingPoint;

void Widget::UpdateAll(const tb::Matrix<4, 4, float>& pose) {
	// lookingPoint算出
	const tb::Vector<3, float> fv((const float[3]){0.0f, 0.0f, 1.0f});
	const tb::Vector<3, float> lv(pose * fv);
	const tb::Vector<3, float> lp(lv * vDistance / lv[2]);
	lookingPoint[0] = lp[0];
	lookingPoint[1] = lp[1];

	// WidgetのUpdate
	root.Foreach(&Widget::Update);
}


void Widget::DrawAll() {
	// TODO:View行列設定
	root.Foreach(&Widget::Draw);
}
void Widget::TrawAll() {
	// TODO:View行列設定
	root.Reveach(&Widget::Traw);
}
