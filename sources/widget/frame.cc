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
#include "widget/frame.h"
#include "core.h"
#include "gl/gl.h"



namespace widget {

	tb::Prefs<float> Frame::movingRatio("pane/movingRatio",
		0.25f,
		"単一フレーム移動距離のレート：単一フレームの間に目標位置との差にこの値"
		"を乗じた分だけ移動する");

	Notify Frame::Update() {
		Notify n;
		for (tb::List<Frame>::I i(children); ++i;) {
			n.raw |= (*i).Update().raw;
		}
		return n;
	};
	void Frame::DrawEntity(const R& r) {
		mask = (r - position) & R(spread);
		shown = !mask.IsEmpty();
		if (IsShown()) {
			glPushMatrix();
			glTranslatef(position[0], position[1], -depth);
			children.Foreach(&Frame::DrawEntity, GetMask());
			Draw(GetMask());
			glPopMatrix();
		}
	};
	void Frame::TrawEntity() {
		if (IsShown()) {
			glPushMatrix();
			glTranslatef(position[0], position[1], -depth);
			Traw();
			children.Reveach(&Frame::TrawEntity);
			glPopMatrix();
		}
	};



	// 移動、リサイズ
	void Frame::Move(const P& p) { target += p; }
	void Frame::MoveTo(const P& p) { target = p; }
	void Frame::ReSize(const S& s) { spread = s; }
	void Frame::SetDepth(float d) { depthTarget = d; }

	// 実際の移動
	void Frame::AccualMove() {
		position += (target - position) * (float)movingRatio;
		depth += (depthTarget - depth) * (float)movingRatio;
	}

	// 中心を計算
	Frame::P Frame::GetCenter() const { return position + (spread * 0.5); }

}
