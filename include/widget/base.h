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

#include <tb/list.h>
#include <tb/matrix.h>
#include <tb/prefs.h>
#include <tb/rect.h>
#include <tb/spread.h>
#include <tb/time.h>
#include <tb/vector.h>



namespace widget {

	struct Base : tb::List<Base>::Node {
		using P = tb::Vector<2, float>;
		using S = tb::Spread<2, unsigned>;
		using R = tb::Rect<2, float>;
		using M = tb::Matrix<4, 4, float>;

		virtual P GetCenter() { return dummyCenter; };
		void Pick(Base& t) {
			// mouseDownなどでその要素を先頭にする
			children.Insert(t);
		};
		void AddHead(Base& t) { Pick(t); };
		virtual void ReDepth();

	protected:
		tb::List<Base> children;
		Base() {};
		virtual ~Base() {};

		struct Notify {
			Notify() : pick(0), notify(false) {};
			Notify(const Notify&) = default;
			void operator|=(const Notify& n); // Notify合成
			Base* pick;						  // これを返したnodeを先頭に
			bool notify;					  // 通知あり
		} notify;							  // このインスタンスのnotify

		static P lookingPoint;
		virtual Notify Update(const tb::Timestamp&);
		virtual void Draw() { children.Foreach(&Base::Draw); };
		virtual void Traw() { children.Reveach(&Base::Traw); };
		virtual void Draw(const R& r) { children.Foreach(&Base::Draw, r); };
		virtual void ReDepth(float depth, float thick);

	private:
		// つながってるリストがなくなったら一緒に消滅
		void NotifyListDeleted() final { delete this; };

		// GetCenterに返す仮の値
		static const P dummyCenter;
	};
}
