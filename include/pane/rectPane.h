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

#include "widget.h"



// 範囲のメンテナンスとインターフェイス
struct Geometry {
	Geometry(const Widget::P& position, const Widget::S& size) :
		target(position),
		position(position),
		size(size) {};
	Geometry(const Geometry& o) :
		target(o.position),
		position(o.position),
		size(o.size) {};

	// 設定された目標値に向かって移動
	void Update() { position += (target - position) * (float)moveRatio; };

	// 即時移動
	void Jump(const Widget::P& delta) {
		target += delta;
		position = target;
	};
	void JumpTo(const Widget::P& to) { target = position = to; };

	// 移動先設定
	void Move(const Widget::P& delta) { target += delta; };
	void MoveTo(const Widget::P& to) { target = to; };

	// 即時リサイズ
	void SizeTo(const Widget::S to) { size = to; };

	// 取得
	const Widget::P Center() const { return position + (size * 0.5); };
	const Widget::P& LeftTop() const { return position; };
	const Widget::P RightBottom() const { return position + size; };
	const Widget::S& Size() const { return size; };
	const Widget::R Rect() { return Widget::R(position, size); };

private:
	static tb::Prefs<float> moveRatio;

	// 元値
	Widget::P target;
	Widget::P position;
	Widget::S size;
};


// 範囲だけがあるWidget
struct RectPane : public Widget, Geometry {
	RectPane(const R&);
	RectPane(const P&, const S&);
	RectPane(const S&);

protected:
	// 内部ハンドラ
	void Draw() override { Widget::Draw(Rect()); };
	void Traw() override { Widget::Traw(Rect()); };
	void Draw(const R&) override;
	void Traw(const R&) override;

private:
	static tb::Prefs<unsigned> moveRatio;

	tb::Color color; // 背景色
	bool draw;		 // 背景を描画する
};
