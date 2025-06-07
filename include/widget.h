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
#pragma once

#include "model.h"
#include <tb/canvas.h>
#include <tb/image.h>
#include <tb/list.h>
#include <tb/matrix.h>
#include <tb/prefs.h>
#include <tb/rect.h>
#include <tb/spread.h>
#include <tb/time.h>
#include <tb/vector.h>
#include <wOLIB/message.h>



struct Widget : tb::List<Widget>::Node {
	using P = tb::Vector<2, float>;
	using S = tb::Spread<2, unsigned>;
	using R = tb::Rect<2, float>;
	using A = tb::Matrix<4, 4, float>;

	static void UpdateAll(const A&, const tb::Timestamp&);
	static void DrawAll(const A& eye2Head);
	static void TrawAll();
	static void DrawNavigationAll() { root.Foreach(&Widget::DrawNavigation); };
	static void TrawNavigationAll();

protected:
	static tb::Prefs<float> vDistance;
	static tb::Prefs<float> vStep;
	static tb::Prefs<float> scale;

	// navigation用の点を描画
	float inRadious;
	float outRadious;
	static void Dot(const P&);



	// 描画、制御ハンドラ
	virtual void Update(const tb::Timestamp&);
	virtual void Draw() { children.Foreach(&Widget::Draw); };
	virtual void Traw() { children.Foreach(&Widget::Traw); };
	virtual void Draw(const R& r) { children.Foreach(&Widget::Draw, r); };
	virtual void Traw(const R& r) { children.Foreach(&Widget::Traw, r); };
	virtual void DrawNavigation() {
		children.Foreach(&Widget::DrawNavigation);
	};

	// その他操作
	void SetVisibility(bool v) { visible = v; };

	void OnMessage(const wO::Message&);

protected:
	Widget* parent;
	tb::List<Widget> children;

	Widget() : visible(true) {};
	virtual ~Widget() {};

	void Register(Widget& w) {
		w.parent = this;
		children.Insert(w); // 普通は先頭に
	};



	// ユーザ向けハンドラ
	virtual void OnUpdate() {};


	static struct Navigator {
		float innerRadious; // ナビゲーションサークルの内径(現物と一致)
		float outerRadious; // 同外形(無限遠)
		float thickness;	// 上二つの差
	} navigator;


	static P lookingPoint;

private:
	static A viewMatrix;

	static tb::List<Widget> root;


	bool visible;

	// つながってるリストがなくなったら一緒に消滅
	void NotifyListDeleted() final { delete this; };


	Widget(const Widget&) = delete;
	void operator=(const Widget&) = delete;
};
