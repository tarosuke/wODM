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
#include "pane/pane.h"
#include "pane/rootPane.h"
#include <cmath>



/***** 設定
 */
tb::Prefs<float> Position::movingRatio(
	"pane/movingRatio", 0.25f, "単一フレーム移動距離のレート");



/***** 子要素管理
 */
void Pane::AddHead(Pane& p) {
	children.Insert(p);
	ReDepth();
};
void Pane::AddTail(Pane& p) {
	children.Add(p);
	ReDepth();
};
void Pane::ReDepth() {
	// 子要素をReDepthするのみ
	children.Foreach(&Pane::ReDepth);
}


/***** 周期処理
 * Paneは奥行制御はしないので戻り値のNotifyは転送するのみ
 * pickリクエストがあっても並び替えたりはしない(奥行ではなく平面の順番の場合、それらは奥行としては一つのものであり、並び替えてはいけない-並び替えるのはWidgetのみである)
 */
Pane::Notify Pane::Update(const tb::Timestamp& ts) {
	Notify r(notify);
	for (tb::List<Pane>::I i(children); ++i;) {
		r.raw |= (*i).Update(ts).raw;
	}

	return r;
};
void Pane::Draw() { children.Foreach(&Pane::Draw); };
void Pane::Traw() { children.Foreach(&Pane::Traw); };
void Pane::Draw(const R& r) { children.Foreach(&Pane::Draw, r); };
void Pane::Traw(const R& r) { children.Foreach(&Pane::Traw, r); };


const Pane::P Pane::dummyCenter;
