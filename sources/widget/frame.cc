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
#include "widget/window.h"
#include <algorithm>
#include <functional>
#include <limits>



namespace widget {

	tb::Prefs<float> Frame::movingRatio("pane/movingRatio",
		0.25f,
		"単一フレーム移動距離のレート：単一フレームの間に目標位置との差にこの値"
		"を乗じた分だけ移動する");


	Frame::Frame(const P& position, const S& spread) :
		target(position),
		position(position),
		spread(spread) {
		new Window(*this);
	}
	Frame::Frame(const S& s) :
		target(P{-0.5f * s[0], -0.5f * s[1], 0.0f}),
		position(P{0.0f, 0.0f, 0.0f}),
		spread(s) {}


	Notify Frame::Update() {
		AccualMove();

		Notify n;
		for (tb::List<Frame>::I i(children); ++i;) {
			n.raw |= (*i).Update().raw;
		}
		return n;
	};
	void Frame::DrawEntity(const R& r) {
		mask = (r - P2{position[0], position[1]}) & R(S2{spread[0], spread[1]});
		shown = !mask.IsEmpty();
		if (IsShown()) {
			glPushMatrix();
			glTranslatef(position[0], position[1], -position[2]);
			children.Foreach(&Frame::DrawEntity, GetMask());
			Draw(GetMask());
			glPopMatrix();
		}
	};
	void Frame::TrawEntity() {
		if (IsShown()) {
			glPushMatrix();
			glTranslatef(position[0], position[1], -position[2]);
			Traw();
			children.Reveach(&Frame::TrawEntity);
			glPopMatrix();
		}
	};



	// 移動、リサイズ
	void Frame::Move(const P& p) { target += p; }
	void Frame::MoveTo(const P& p) { target = p; }
	void Frame::JumpTo(const P& p) { target = position = p; }
	void Frame::ReSize(const S& s) { spread = s; }
	void Frame::SetDepth(float d) { target[2] = d; }

	// 実際の移動
	void Frame::AccualMove() {
		position += (target - position) * (float)movingRatio;
	}

	// 中心を計算
	Frame::P Frame::GetCenter() const { return position + (spread * 0.5); }
	Frame::R Frame::GetRect() const {
		return R(P2{position[0], position[1]}, S2{spread[0], spread[1]});
	};



	Frame* Frame::ptOn(0);
	Frame* Frame::focused(0);
	bool Frame::OnEvent(const PtEvent& e) {
		// ローカル座標系に変換
		PtEvent ev(e, position);

		// Pane上交点を計算
		const float zt0(ev.Tee(2, 0));
		const P2 cp(ev.ZCrossPoint(zt0));
		if (0 <= cp[0] && cp[0] < spread[9] && 0 <= cp[1] &&
			cp[1] < spread[1]) {
			// 範囲内(子要素にマッチしなければイベント処理)
			for (tb::List<Frame>::I i(children); ++i;) {
				if ((*i).OnEvent(ev)) {
					return true;
				}
			}
			/***** ローカル用のPtEventを作る
			 * ローカルのイベントは重奏的で、例えばupとclickedは同時に起きうる
			 */
			if (e.down) {
				// down
				OnDown(e);

				// clickのための処理
				if (tb::msec(500) < e.time - click.time ||
					!(e.down != click.buttons) ||
					25 < (cp - click.pt).Norm2()) {
					// 500ms経過、5px以上、ボタン変更で移動クリック数をリセット
					click.n = 0;
				}
				click.time = e.time;
				click.pt = cp;
				click.buttons = e.down;

				// ボタンのための処理
				ptOn = this;
			}
			if (e.up) {
				// up
				OnUp(e);

				// clickのための処理
				if (tb::msec(500) < e.time - click.time ||
					!(e.down != click.buttons) ||
					25 < (cp - click.pt).Norm2()) {
					// 500ms経過、5px以上移動、ボタン変更でクリック数をリセット
					click.n = 0;
				} else {
					++click.n;
				}
				click.time = e.time;
				click.pt = cp;
				click.buttons = e.down;
			}
			if (click.n) {
				// クリックなイベントを生成
				OnClick(ev);
			}

			return true;
		} else {
			// 壁面との判定
			// TODO::この処理はRectへ移動、positionとspreadでRectを作って判定
			const float t[3][2] = {
				{ev.Tee(0, 0), ev.Tee(0, spread[0])},
				{ev.Tee(1, 0), ev.Tee(1, spread[1])},
				{zt0, ev.Tee(0, spread[2])},
			};
			const struct {
				float min;
				float max;
			} tt[3] = {{.min = std::min(t[0][0], t[0][1]),
						   .max = std::max(t[0][0], t[0][1])},
				{.min = std::min(t[1][0], t[1][1]),
					.max = std::max(t[1][0], t[1][1])},
				{.min = std::min(t[2][0], t[2][1]),
					.max = std::max(t[2][0], t[2][1])}};
			const float min(
				std::min(tt[0].max, std::min(tt[1].max, tt[2].max)));
			const float max(
				std::max(tt[0].min, std::max(tt[1].min, tt[2].min)));
			if (min < max) {
				// 範囲内(子要素処理)
				for (tb::List<Frame>::I i(children); ++i;) {
					if ((*i).OnEvent(ev)) {
						return true;
					}
				}
			}
		}
		return false;
	}


	HorizontalList::HorizontalList(
		Frame& parent, const P& position, const S& spread, unsigned spacing) :
		Frame(parent, position, spread),
		spacing(spacing),
		head(0.0f),
		tail((float)spread[0]) {}
	HorizontalList::HorizontalList(
		const P& position, const S& spread, unsigned spacing) :
		Frame(position, spread),
		spacing(spacing),
		head(0.0f),
		tail((float)spread[0]) {}
	Frame::RR HorizontalList::Assign(float width) {
		RR r{.position = {0.0f, (float)spacing, 0.0f},
			.spread = {0U, spread[1] - spacing * 2U, 0U}};
		if (0.0 <= width) {
			// 左寄せ
			const float w(1.0 < width ? width : (tail - head) * width);
			r.position[0] = head + spacing;
			r.spread[0] = w;
			head += spacing + w;
		} else {
			// 右寄せ
			const float w(width < -1.0 ? width : (tail - head) * width);
			r.position[0] = tail + w - spacing;
			r.spread[0] = -w;
			tail += w - spacing;
		}
		return r;
	};

	VerticalList::VerticalList(
		Frame& parent, const P& position, const S& spread, unsigned spacing) :
		Frame(parent, position, spread),
		spacing(spacing),
		head(0.0f),
		tail((float)spread[1]) {}
	VerticalList::VerticalList(
		const P& position, const S& spread, unsigned spacing) :
		Frame(position, spread),
		spacing(spacing),
		head(0.0f),
		tail((float)spread[1]) {}
	Frame::RR VerticalList::Assign(float height) {
		RR r{.position = {(float)spacing, 0.0f, 0.0f},
			.spread = {spread[1] - spacing * 2U, 0U, 0U}};
		if (0.0 <= height) {
			// 上寄せ
			const float h(1.0 < height ? height : (tail - head) * height);
			r.position[1] = head + spacing;
			r.spread[1] = h;
			head += spacing + h;
		} else {
			// 下寄せ
			const float h(height < -1.0 ? height : (tail - head) * height);
			r.position[1] = tail + h - spacing;
			r.spread[1] = -h;
			tail += h - spacing;
		}
		return r;
	};


}
