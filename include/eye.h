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
#include <tb/list.h>
#include <tb/matrix.h>
#include <tb/spread.h>



struct Eye : tb::List<Eye>::Node {
	struct Key {
		Key(const Eye& e) : eye(e) { e.Prepare(); };
		~Key() { eye.Postdraw(); };
		const Eye& eye;
	};

	Eye() = delete;
	Eye(unsigned width, unsigned height) :
		width(width),
		height(height),
		min(std::min(width, height)),
		max(std::max(width, height)) {};
	void operator=(const Eye&) = delete;
	virtual ~Eye() {};



	tb::Matrix<4, 4, float> projection; // Transpose(GetProjectionMatrix)
	tb::Matrix<4, 4, float>
		eye2Head; // Transpose(GetEyeToHeadTransform()).InvertAffine()
	unsigned memo;
	const unsigned width;
	const unsigned height;
	const unsigned min;
	const unsigned max;

	/***** 描画前後の処理
	 * 投影行列の設定やフレームバッファのクリアなどの描画前処理
	 * 描画終了処理
	 */
	virtual void Prepare() const = 0;
	virtual void Postdraw() const = 0;

	/***** Model-VIewと内容
	 * glLoadIdentity：画面の端が-1〜1
	 * glscale2f(0.5/width, 0.5/height)：ピクセルbyピクセル
	 * glScale2f(height/width, 1)：高さが-1〜1、アスペクト1
	 * glScale2f(1, width/height)：幅が-1〜1、アスペクト1
	 * glScale2f(min/width, min/height)：短辺が-1〜1、アスペクト1
	 * glScale2f(max/width, max/height)：長辺が-1〜1、アスペクト1
	 * pose：実寸、頭の向き
	 */
	virtual void Identity() const = 0;	   // 画面の端が-1〜1、アスペクト無視
	virtual void PixelByPixel() const = 0; // ピクセルbyピクセル、アスペクト無視
	virtual void Vertical11() const = 0;   // 高さが-1〜1、アスペクト1
	virtual void Horizontal11() const = 0; // 幅が-1〜1、アスペクト1
	virtual void Short11() const = 0;	   // 短辺が-1〜1、アスペクト1
	virtual void Long11() const = 0;	   // 長辺が-1〜1、アスペクト1
	virtual void Pose(const tb::Matrix<4, 4, float>&) const = 0;

private:
	void NotifyListDeleted() final { delete this; };
};
