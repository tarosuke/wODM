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
#include "widget/rect.h"
#include "gl/gl.h"



namespace widget {

	tb::Prefs<float> Rect::movingRatio(
		"pane/movingRatio",
		0.25f,
		"単一フレーム移動距離のレート：単一フレームの間に目標位置との差にこの値"
		"を乗じた分だけ移動する");


	// 移動、リサイズ
	void Rect::Move(const P& p) { target += p; }
	void Rect::MoveTo(const P& p) { target = p; }
	void Rect::Jump(const P& p) {
		target += p;
		position = target;
	}
	void Rect::JumpTo(const P& p) { target = position = p; }
	void Rect::ReSize(const S& s) { spread = s; }
	void Rect::ReDepth(float d, float t) {
		target[2] = d;
		thick = t;
		for (tb::List<Base>::I i(children); ++i;) {
			// 子要素は同じ奥行
			ReDepth(0, t);
		}
	}



	// 周期処理
	Base::Notify Rect::Update(const tb::Timestamp& ts) {
		// 移動処理
		position += (target - position) * (float)movingRatio;

		// 子要素はBase任せ
		return Base::Update(ts);
	}

	void Rect::Draw() {
		rect = R(spread);
		Base::Draw(rect);
	}

	void Rect::Draw(const R& r) {
		rect = R(spread) & (r - position);
		if (rect.IsEmpty()) {
			// 見えていないので何もせず戻る
			return;
		}
		glPushMatrix();
		glTranslatef(-position[0], -position[1], -position[2]);
		Base::Draw(rect); // 子要素描画
		DrawContent();	  // 子要素より後に描画
		glPopMatrix();
	}

	void Rect::Traw() {
		if (rect.IsEmpty()) { // NOTE:rectはDrawで設定済み
			return;
		}
		glPushMatrix();
		glTranslatef(-position[0], -position[1], -position[2]);
		TrawContent(); // 子要素より前に描画
		Base::Traw();  // 子要素描画
		glPopMatrix();
	}

}
