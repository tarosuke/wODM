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
#include <widget/base.h>



namespace widget {

	const Base::P Base::dummyCenter;


	Base::Notify Base::Update(const tb::Timestamp& ts) {
		Notify n(notify);
		for (tb::List<Base>::I i(children); ++i;) {
			const Notify nn((*i).Update(ts));
			n |= nn;
		}

		if (n.pick) {
			// pickされていたのでそれを先頭にする
			if (children.Top() != n.pick) {
				children.Insert(*n.pick);
				n.pick = this; // 並べ替えたので再配置はthisから
			}
		}

		return n;
	}

	void Base::ReDepth() {
		children.Foreach(&Base::ReDepth);
		notify.pick = 0;
	}

	void Base::Notify::operator|=(const Notify& n) {
		notify |= n.notify;
		if (n.pick) {
			pick = n.pick;
		}
	}


	void Base::ReDepth(float d, float t) {
		for (tb::List<Base>::I i(children); ++i;) {
			// 子要素は同じ奥行
			ReDepth(0, t);
		}
	}
}
