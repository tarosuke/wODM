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
#include "pane/rootPane.h"
#include "gl/gl.h"
#include "widget.h"
#include <math.h>



namespace {
	RootPane rootPane;
	struct {
		float in;  // 内径
		float out; // 外径
		float th;  // 太さ
	} nav;		   // ナビゲーションリングの諸元
}


tb::Prefs<float> RootPane::vDistance(
	"widget/virtualDistance", 1.0f, "一番手前のWidgetが見える奥行き[m]");
tb::Prefs<float> RootPane::scale("widget/scale", 0.001f, "1pxのサイズ[m]");
tb::Prefs<float> RootPane::navigationAngle(
	"widget/navigationAngle",
	1.0f,
	"ナビゲーションリングの外径視野角[対奥行比]");
tb::Prefs<float> RootPane::navigationThick(
	"widget/navigationAngle", 16.0f, "ナビゲーションリングの太さ[px]");



RootPane::P RootPane::lookingPoint;



void RootPane::UpdateAll(
	const tb::Matrix<4, 4, float>& pose, const tb::Timestamp& ts) {
	// lookingPoint算出
	const tb::Vector<3, float> fv((const float[3]){0.0f, 0.0f, 1.0f});
	const tb::Vector<3, float> lv(pose * fv + fv); // 正面と頭の向きの中間
	lookingPoint = lv * (float)vDistance / lv[2];
}
void RootPane::DrawAll(const A& e2h) {
	// headMatrixとe2hからviewMatrixを算出
	tb::Matrix<4, 4, float> headMatrix = (const float[4][4]){
		{scale, 0.0, 0.0, 0.0},
		{0.0, scale, 0.0, 0.0},
		{0.0, 0.0, 1.0, 0.0},
		{-lookingPoint[0], -lookingPoint[1], 0.0, 0.0}}; // 頭の姿勢

	// Navigation描画
	glLoadMatrixf((const float*)e2h);
	glBegin(GL_POINTS);
	rootPane.DrawNavigation(lookingPoint);
	glEnd();

	// 描画
	glMultMatrixf((const float*)headMatrix);
	rootPane.Draw();
}
void RootPane::TrawAll(const A& e2h) {
	glLoadMatrixf((const float*)e2h);

	// traw widgets
	rootPane.Traw();

	// TODO:traw navigationring
}


/***** 設定からnav経の変換
 */
void RootPane::UpdateNav() {
	nav.out = vDistance * scale * navigationAngle;
	nav.th = navigationThick;
	nav.in = nav.out - nav.th;
};

/***** 引数なしPaneコンストラクタ(Widget専用)
 * NOTE:rootPaneが見える必要があるのでPane.ccではなくここにある
 */
Pane::Pane() : Pane(rootPane) {}


/***** Widgetのメソッド
 * NOTE:ナビゲーションリングの諸元を見るためにここにある
 */
void Widget::DrawNavigation(const P& lookingPoint) {
	tb::Vector<2, float> p(center - lookingPoint);
	const float norm(p.Norm());
	if (norm <= nav.in) {
		// ナビゲーションサークル以内は輝点を表示しない
		return;
	}

	// リングの内外径に内径-無限遠が収まるよう極軸座標系で計算
	const float r(nav.out - nav.th / (norm - nav.in));
	const tb::Vector<2, float> pp(p * r / norm);

	glVertex2f(pp[0], pp[1]); // glBegin/glEndは処理全体
}
