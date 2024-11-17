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
#include <string.h>



tb::Prefs<float> Widget::vDistance(
	"widget/virtualDistance", 1.0f, "一番手前のWidgetが見える奥行き");
tb::Prefs<float> Widget::scale("widget/scale", 0.001f, "1pxのサイズ");

tb::List<Widget> Widget::roots;
tb::Vector<2, float> Widget::lookingPoint;
tb::Matrix<4, 4, float> Widget::view;


void Widget::RegisterRoot(Widget& w) {
	for (auto r : roots) {
		r->SetVisibility(false);
	}
	roots.Insert(w);
}

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
	Root().children.Foreach(&Widget::Update);
}


void Widget::DrawAll() {
	glLoadIdentity();
	glTranslatef(-lookingPoint[0], -lookingPoint[1], 0);
	glScalef(scale, (float)scale, 1);
	glGetFloatv(GL_MODELVIEW_MATRIX, view);
	Root().children.Foreach(&Widget::Draw);
}
void Widget::TrawAll() {
	glLoadMatrixf(view);
	Root().children.Reveach(&Widget::Traw);
}


void Widget::Update() {
	OnUpdate();
	children.Foreach(&Widget::Update);
}
void Widget::Draw() { children.Foreach(&Widget::Draw); }
void Widget::Traw() { children.Reveach(&Widget::Traw); }
void Widget::Draw(const tb::Rect<2, int>& r) {
	children.Foreach(&Widget::Draw, r);
}
void Widget::Traw(const tb::Rect<2, int>& r) {
	children.Reveach(&Widget::Traw, r);
}


///// RectWidget
void RectWidget::Jump(const tb::Vector<2, int>& d) {
	leftTop += d;
	rect += d;
}
tb::Prefs<unsigned>
	RectWidget::moveRatio("RectWidget/MoveRatio", 5, "Widget移動速度の係数");
void RectWidget::Update() {
	// 移動処理
	if (const auto d = ((target - leftTop).Norm2())) {
		// 目標店まで差があるので移動
		if (d < (int)(moveRatio * moveRatio)) {
			JumpTo(target);
		} else {
			Jump(d / moveRatio);
		}
	}

	// 子要素にも適用
	Widget::Update();
}
void RectWidget::Draw() { Widget::Draw(rect); }
void RectWidget::Traw() { Widget::Traw(rect); }
void RectWidget::Draw(const tb::Rect<2, int>& r) {
	const tb::Rect<2, int> rr(r & rect);
	if (rr) {
		Widget::Draw(rr);
	}
}
void RectWidget::Traw(const tb::Rect<2, int>& r) {
	const tb::Rect<2, int> rr(r & rect);
	if (rr) {
		Widget::Traw(rr);
	}
}



CanvasWidget::Fragment::Fragment(
	const tb::Image& origin,
	const tb::Vector<2, int>& offset,
	const tb::Spread<2, int>& size)
	: BufferedImage<tb::ImageARGB32>(size[0], size[1]), offset(offset) {
	// 画像の転送
}



///// CanvasWidget
void CanvasWidget::OnCanvasUpdated(const tb::Rect<2, double>& from) {
	tb::Canvas::Image image(*this);
	const tb::Rect<2, int> f(from);

	const int step(
		maxFragmentPixels <= f.GetSpread()[0]
			? 1
			: maxFragmentPixels / f.GetSpread()[0]);
	const int bottom(f.Right()[1]);

	for (int y(f.Left()[1]); y < bottom; y += step) {
		const tb::Spread<2, int> fs(
			f.GetSpread()[0],
			y + step <= bottom ? step : bottom - y);
		const tb::Vector<2, int> ff(from.Left()[0], y);

		AddFragment(
			*new Fragment(image, tb::Vector<2, int>(from.Left()[0], y), fs));
	}
}

void CanvasWidget::Update() {
	while (Fragment* const f = updates.Get()) {
		Texture::Update(*f, f->offset);
	}
}
