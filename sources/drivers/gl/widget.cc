/*****************************************************************************
 * Copyright (C) 2024,2025 tarosuke<webmaster@tarosuke.net>
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
#include <string.h>



tb::List<Widget> Widget::root;
Widget::P Widget::lookingPoint;
Widget::A Widget::viewMatrix;


void Widget::UpdateAll(
	const tb::Matrix<4, 4, float>& pose, const tb::Timestamp& ts) {
	// lookingPoint算出
	const tb::Vector<3, float> fv((const float[3]){0.0f, 0.0f, 1.0f});
	const tb::Vector<3, float> lv(pose * fv + fv); // 正面と頭の向きの中間
	lookingPoint = lv * (float)vDistance / lv[2];

	// 子要素ののUpdate
	root.Foreach(&Widget::Update, ts);
}
void Widget::DrawAll(const tb::Matrix<4, 4, float>& e2h) {
	// headMatrixとe2hからviewMatrixを算出
	tb::Matrix<4, 4, float> headMatrix = (const float[4][4]){
		{scale, 0.0, 0.0, 0.0},
		{0.0, scale, 0.0, 0.0},
		{0.0, 0.0, 1.0, 0.0},
		{-lookingPoint[0], -lookingPoint[1], 0.0, 0.0}}; // 頭の姿勢
	viewMatrix = e2h * headMatrix;

	// 描画
	glLoadMatrixf(viewMatrix);
	root.Foreach(&Widget::Draw);
}
void Widget::TrawAll() {
	glLoadMatrixf(viewMatrix);
	root.Reveach(&Widget::Traw);
}



tb::Prefs<float> Widget::vDistance(
	"widget/virtualDistance", 1.0f, "一番手前のWidgetが見える奥行き");
tb::Prefs<float> Widget::vStep("widget/vStep", 0.01f, "root直下の窓の奥行差");
tb::Prefs<float> Widget::scale("widget/scale", 0.001f, "1pxのサイズ");



void Widget::Update(const tb::Timestamp& t) {
	OnUpdate();
	children.Foreach(&Widget::Update, t);
}
