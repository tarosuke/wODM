/*****************************************************************************
 * Copyright (C) 2025,2026 tarosuke<webmaster@tarosuke.net>
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
#include "widget/prefs.h"
#include "widget/window.h"
#include <algorithm>
#include <functional>
#include <limits>



namespace widget {

	tb::Prefs<float> Frame::movingRatio("pane/movingRatio",
		0.25f,
		"単一フレーム移動距離のレート：単一フレームの間に目標位置との差にこの値"
		"を乗じた分だけ移動する");


	/***** 親ありフル指定
	 * 親ありの基本構築子
	 */
	Frame::Frame(Frame& parent, const R3& r) : rect(r), target(r.Origin()) {
		parent.Add(*this);
	}

	/***** フル指定Window用
	 * NOTE:戻ってからRootに登録すること
	 */
	Frame::Frame(const R3& r) : rect(r), target(r.Origin()) {}

	/***** Windowの中身用
	 * 平面サイズだけを与える
	 * 平面サイズだけが設定され、残りはWindowの子になった時に設定される
	 */
	Frame::Frame(const S& spread) :
		rect(R3(S3({spread[0], spread[1], 0.0f}))),
		target(P3({0.0f, 0.0f, 0.0f})) {
		new Window(*this);
	}



	Notify Frame::Update() {
		AccualMove();

		Notify n;
		for (tb::List<Frame>::I i(children); ++i;) {
			n.raw |= (*i).Update().raw;
		}
		return n;
	};
	void Frame::DrawEntity(const R& r) {
		mask = ToR(rect);
		mask &= r;
		shown = !!mask;
		if (IsShown()) {
			glPushMatrix();
			glTranslatef(rect.Origin()[0], rect.Origin()[1], -rect.Origin()[2]);
			children.Foreach(&Frame::DrawEntity, GetMask());
			Draw(GetMask());
			glPopMatrix();
		}
	};
	void Frame::TrawEntity() {
		if (IsShown()) {
			glPushMatrix();
			glTranslatef(rect.Origin()[0], rect.Origin()[1], -rect.Origin()[2]);
			Traw();
			children.Reveach(&Frame::TrawEntity);
			glPopMatrix();
		}
	};



	// 移動、リサイズ
	void Frame::Move(const P& p) { target += p; }
	void Frame::MoveTo(const P& p) { target = ToP3(p); }
	void Frame::JumpTo(const P& p) { rect.Origin(target = ToP3(p)); }
	void Frame::ReSize(const S& s) { rect.Spread(ToS3(s)); }
	void Frame::SetDepth(float d) { target[2] = d; }
	void Frame::ReDepth(float d, float t) {
		target[2] = d;
		if (t != rect.Spread(2)) {
			rect.Spread(2, t);

			// 奥行きが変更されたら場合子要素も再配置
			float d(0.0);
			float dd(rect.Spread(2) * 0.5);
			for (tb::List<Frame>::I i(children); ++i; d += dd, dd *= 0.5) {
				(*i).ReDepth(d, dd);
			}
		}
	}

	// 実際の移動
	void Frame::AccualMove() {
		rect += (target - rect.Origin()) * (float)movingRatio;
	}

	// 中心を計算
	Frame::P Frame::GetCenter() const {
		return P{{rect.Origin(0) + rect.Spread(0) * 0.5f,
			rect.Origin(1) + rect.Spread(1) * 0.5f}};
	}



	Frame* Frame::ptOn(0);
	Frame* Frame::focused(0);
	bool Frame::OnEvent(const PtEvent& e) {
		// ローカル座標系に変換
		PtEvent ev(e, rect.Origin());

		// Pane上交点を計算
		const float zt0(ev.Tee(2, 0));
		const P cp(ev.ZCrossPoint(zt0));
		if (0 <= cp[0] && cp[0] < rect.Spread(9) && 0 <= cp[1] &&
			cp[1] < rect.Spread(1)) {
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
				{ev.Tee(0, 0), ev.Tee(0, rect.Spread(0))},
				{ev.Tee(1, 0), ev.Tee(1, rect.Spread(1))},
				{zt0, ev.Tee(0, rect.Spread(2))},
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
		Frame& parent, const R& rect, unsigned spacing) :
		Frame(parent, rect),
		spacing(spacing),
		head(0.0f),
		tail(rect.Spread(0)) {}
	HorizontalList::HorizontalList(const R& rect, unsigned spacing) :
		Frame(ToS3(rect.Spread())),
		spacing(spacing),
		head(0.0f),
		tail(rect.Spread(0)) {}
	Frame::R3 HorizontalList::Assign(float width) {
		R3 r(P3({0.0f, (float)spacing, rect.Spread(2) * 0.5f}),
			S3({0.0f, rect.Spread(1) - spacing * 2, rect.Spread(2)}));
		if (0.0 <= width) {
			// 左寄せ
			const float w(1.0 < width ? width : (tail - head) * width);
			r.Origin(0, head + spacing);
			r.Spread(0, w);
			head += spacing + w;
		} else {
			// 右寄せ
			const float w(width < -1.0 ? width : (tail - head) * width);
			r.Origin(0, tail + w - spacing);
			r.Spread(0, -w);
		}
		return r;
	};

	VerticalList::VerticalList(Frame& parent, const R& rect, unsigned spacing) :
		Frame(parent, rect),
		spacing(spacing),
		head(0.0f),
		tail(rect.Spread(1)) {}
	VerticalList::VerticalList(const S& spread, unsigned spacing) :
		Frame(spread),
		spacing(spacing),
		head(0.0f),
		tail(rect.Spread(1)) {}
	Frame::R3 VerticalList::Assign(float height) {
		// 領域を割り当てる
		R3 r(P3({(float)spacing, 0.0f, rect.Spread(2) * 0.5f}),
			S3({rect.Spread(1) - spacing * 2, 0.0f, rect.Spread(2)}));
		if (0.0 <= height) {
			// 上寄せ
			const float h(1.0 < height ? height : (tail - head) * height);
			r.Origin(1, head + spacing);
			r.Spread(1, h);
			head += spacing + h;
		} else {
			// 下寄せ
			const float h(height < -1.0 ? height : (tail - head) * height);
			r.Origin(1, tail + h - spacing);
			r.Spread(1, -h);
			tail += h - spacing;
		}
		return r;
	};
}
