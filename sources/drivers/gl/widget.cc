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
#include <math.h>



tb::Prefs<float> Widget::vDistance(
	"widget/virtualDistance", 1.0f, "一番手前のWidgetが見える奥行き");
tb::Prefs<float> Widget::scale("widget/scale", 0.001f, "1pxのサイズ");

tb::List<Widget> Widget::root;
tb::Vector<2, float> Widget::lookingPoint;
tb::Matrix<4, 4, float> Widget::view;

void Widget::UpdateAll(const tb::Matrix<4, 4, float>& pose) {
	// lookingPoint算出
	const tb::Vector<3, float> fv((const float[3]){0.0f, 0.0f, 1.0f});
	const tb::Vector<3, float> lv(pose * fv + fv);
	const tb::Vector<3, float> lp(lv * (float)vDistance / lv[2]);
	if (isfinite(lp[0]) && isfinite(lp[1])) {
		lookingPoint[0] = lp[0];
		lookingPoint[1] = lp[1];
	}

	// WidgetのUpdate
	root.Foreach(&Widget::Update);
}


void Widget::DrawAll() {
	glLoadIdentity();
	glTranslatef(-lookingPoint[0], -lookingPoint[1], 0);
	glScalef(scale, (float)scale, 1);
	glGetFloatv(GL_MODELVIEW_MATRIX, view);
	root.Foreach(&Widget::Draw);
}
void Widget::TrawAll() {
	glLoadMatrixf(view);
	root.Reveach(&Widget::Traw);
}
