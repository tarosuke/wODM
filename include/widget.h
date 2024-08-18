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
#pragma once

#include <tb/image.h>
#include <tb/list.h>
#include <tb/matrix.h>
#include <tb/prefs.h>
#include <tb/rect.h>
#include <tb/spread.h>
#include <tb/time.h>
#include <tb/vector.h>



struct Widget : tb::List<Widget>::Node {
	static void UpdateAll(const tb::Matrix<4, 4, float>&);
	static void DrawAll();
	static void TrawAll();

	static const tb::Vector<2, float>& LookingPoint() { return lookingPoint; };

	// その他操作
	void SetVisibility(bool v) { visible = v; };

protected:
	Widget() : parent(0), visible(true) {};
	void Register(Widget& w) { children.Insert(w); };

	// ユーザ向けハンドラ
	virtual void OnUpdate() {};

	// 内部ハンドラ
	virtual void Update();
	virtual void Draw();
	virtual void Traw();
	virtual void Draw(const tb::Rect<2, int>&);
	virtual void Traw(const tb::Rect<2, int>&);

private:
	static tb::Prefs<float> vDistance;
	static tb::Prefs<float> scale;
	static tb::List<Widget> root;
	static tb::Vector<2, float> lookingPoint;
	static tb::Matrix<4, 4, float> view;
	Widget* parent;
	tb::List<Widget> children;
	bool visible;
};


// 範囲だけがあるWidget
struct RectWidget : public Widget {
	RectWidget(const tb::Rect<2, int>&);
	RectWidget(const tb::Vector<2, int>&, const tb::Spread<2, unsigned>&);

protected:
	// 移動(Jump:即時/Move:追随, 無印:差分指定/To:目的位置指定)
	void Jump(const tb::Vector<2, int>&);
	void JumpTo(const tb::Vector<2, int>& t) { Jump(leftTop - t); };
	void Move(const tb::Vector<2, int>& d) { target += d; };
	void MoveTo(const tb::Vector<2, int>& t) { target = t; };

	// 内部ハンドラ
	void Update() override;
	void Draw() override;
	void Traw() override;
	void Draw(const tb::Rect<2, int>&) override;
	void Traw(const tb::Rect<2, int>&) override;

private:
	static tb::Prefs<unsigned> moveRatio;

	tb::Rect<2, int> rect;
	tb::Vector<2, int> leftTop;
	tb::Spread<2, unsigned> size;
	tb::Pixel<tb::u8> color;
	bool draw; // 背景を描画する

	tb::Vector<2, int> target; // leftTopと異なる時Updateで移動
};
