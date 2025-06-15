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
tb::Prefs<float> BasePane::movingRatio(
	"pane/movingRatio", 0.25f, "単一フレーム移動距離のレート");
tb::Prefs<float>
	BasePane::baseStep("widget/baseStep", 0.01f, "root直下の窓の奥行差");
tb::Prefs<float>
	BasePane::stepScale("widget/stepScale", 10, "子要素の奥行差の比");


float BasePane::NextStep() { return std::abs(step) / stepScale; }
void BasePane::UpdateDepth() {
	float d(0);
	for (tb::List<Pane>::I i(children); ++i; d -= step) {
		(*i).SetDepth(d);
	}
}
void BasePane::Pick(Pane& p) {
	children.Insert(p);
	UpdateDepth();
}
void BasePane::Away(Pane& p) {
	children.Add(p);
	UpdateDepth();
}
void BasePane::Update(const tb::Timestamp& ts) {
	for (tb::List<Pane>::I i(children); ++i;) {
		(*i).Update(ts);
	}
};
void BasePane::Draw() { children.Foreach(&Pane::Draw); };
void BasePane::Traw() { children.Foreach(&Pane::Traw); };
void BasePane::Draw(const R& r) { children.Foreach(&Pane::Draw, r); };
void BasePane::Traw(const R& r) { children.Foreach(&Pane::Traw, r); };
void BasePane::DrawNavigation(const P& lp) {
	for (tb::List<Pane>::I i(children); ++i;) {
		(*i).DrawNavigation(lp);
	}
};



Pane::Pane(BasePane& parent) : BasePane(parent.NextStep()), parent(parent) {
	Pick(*this);
}
