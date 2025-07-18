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
#include <cmath>
#include <tb/canvas.h>



tb::Prefs<float> RootPane::pDistance(
	"widget/paneDistance", 1.0f, "一番手前のWidgetが見える奥行き[m]");
tb::Prefs<float> RootPane::vDistance(
	"widget/virtualDistance",
	1.0f,
	"視野中心を決めるための視点の投影面までの距離[m]");
tb::Prefs<float> RootPane::scale("widget/scale", 0.001f, "1pxのサイズ[m]");
tb::Prefs<float> RootPane::navigationAngle(
	"widget/navigationAngle",
	0.56f,
	"ナビゲーションリングの外径視野角[対奥行比]");
tb::Prefs<float> RootPane::navigationThick(
	"widget/navigationAngle", 64.0f, "ナビゲーションリングの太さ[px]");



RootPane::P RootPane::lookingPoint;
RootPane::Nav RootPane::nav;

void RootPane::UpdateAll(
	const tb::Matrix<4, 4, float>& pose, const tb::Timestamp& ts) {
	// lookingPoint算出
	const tb::Vector<3, float> fv((const float[3]){0.0f, 0.0f, 1.0f});
	const tb::Vector<3, float> lv(pose * fv + fv); // 正面と頭の向きの中間
	lookingPoint = {
		lv[0] * (float)vDistance / (lv[2] * scale),
		lv[1] * (float)vDistance / (lv[2] * scale)};

	// 子要素Update
	lastNotified = lastPicked = 0;
	Notify n(Update(ts));
	if (n.bits.pick && lastPicked) {
		// 再配置が必要な最も根に近いインスタンスから再配置
		lastPicked->ReDepth();
	}
}
void RootPane::DrawAll(const A& e2h) {
	// Navigation描画
	glLoadMatrixf((const float*)e2h);
	glColor3f(1, 0, 0);
	glPointSize(3);
	glScalef(scale, scale, 1.0);
	glPushMatrix();
	glTranslatef(0, 0, -pDistance);

	glBegin(GL_POINTS);
	DrawNavigation(lookingPoint);
	// glVertex3f(0, 0, -pDistance);
	glEnd();
	glPopMatrix();

	// 描画
	// glMultMatrixf((const float*)headMatrix);
	glTranslatef(lookingPoint[0], lookingPoint[1], -pDistance);

	// 窓はNevより遠いので輝点のあとに描画
	Draw();
}
void RootPane::TrawAll(const A& e2h) {
	glLoadMatrixf((const float*)e2h);
	glTranslatef(0, 0, -pDistance);

	// traw widgets
	Traw();
}

void RootPane::Traw() {
	// 窓はNavより遠いので先に描画(透過率なのであまり関係ないが)
	Pane::Traw();

	// TODO:traw navigationring
	glColor3f(0.8, 0.9, 0.8);
	navigationPanel.Draw();
	// GL::Texture::Binder b(texture);
	// glBegin(GL_TRIANGLE_FAN);
	// glVertex2f(0, 0);
	// glVertex2f(-nav.out, 0);
	// glVertex2f(0, -nav.out);
	// glVertex2f(nav.out, 0);
	// glVertex2f(0, nav.out);
	// glEnd();
}


RootPane::RootPane() {
	tb::Canvas c(512, 512);
	{
		tb::Canvas::GC gc(c);
		gc.Clear(tb::Color(0xffffffff));
		tb::Color c(0xfff0faf0);
		gc.Set(tb::Canvas::GC::cap_round);
		gc.SetFill(c);
		gc.SetStroke(c);
		gc.Arc(0.0, 0.0, 256, -std::numbers::pi, std::numbers::pi);
	}
}

/***** 設定からnav経の変換
 */
void RootPane::UpdateNav() {
	nav.out = vDistance * navigationAngle / scale;
	nav.th = navigationThick;
	nav.in = nav.out - nav.th;
};

void RootPane::DotNavigation(const P& lookingPoint, const P& center) {
	tb::Vector<2, float> p(lookingPoint - center);
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
