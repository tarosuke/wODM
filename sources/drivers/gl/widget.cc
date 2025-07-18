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
#include "widget.h"



tb::Prefs<float>
	Widget::baseThickness("pane/baseThickness", 0.01f, "root直下の窓の厚さ");
tb::Prefs<float>
	Widget::thickRatio("pane/thickRatio", 0.125, "厚さの親要素との比");


void Widget::ReDepth() {
	unsigned n(1);
	float d(0);
	for (tb::List<Pane>::I i(children); ++i; ++n) {
		/***** n番目の子要素1.0 - はn/(n+1)の奥行
		 * 例えば0(使える領域の手前の端)、1/2、2/3...と配置される
		 */
		(*i).SetDepth(d, thickness * (1.0f - 1.0f / n));
		(*i).ReDepth();
	}
}


Pane* Pane::lastPicked(0);
Pane* Pane::lastNotified(0);
Pane::Notify Widget::Update(const tb::Timestamp& ts) {
	Notify r(notify);
	for (tb::List<Pane>::I i(children); ++i;) {
		const Notify rr((*i).Update(ts));
		if (rr.bits.pick && children.Top() != i) {
			// 最前面へ再配置
			AddHead(*i);
			lastPicked =
				this; // 最も根に近いAddHeadしたインスタンスが残るので、根まで戻ったらlastPicked->ReDepthして奥行を再帰的に再計算する
		}

		// リクエストは根へ向かって継承される
		r |= rr;
	}
	notify.bits.pick = 0;
	return r;
};
