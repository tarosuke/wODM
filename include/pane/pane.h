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
#include <wOLIB/message.h>



struct BasePane {
	using P = tb::Vector<2, float>;
	using S = tb::Spread<2, unsigned>;
	using R = tb::Rect<2, float>;
	using A = tb::Matrix<4, 4, float>;

	virtual float NextStep(); // 子要素の奥行差を返す
	virtual void UpdateDepth();

protected:
	tb::List<struct Pane> children;

	BasePane(float step) : step(step) {};
	BasePane() = delete;
	BasePane(const BasePane&) = delete;
	void operator=(const BasePane&) = delete;
	virtual ~BasePane() {};

	/***** 設定
	 */
	static tb::Prefs<float> movingRatio;
	static tb::Prefs<float> baseStep;  // 根直下の要素感の奥行きの差
	static tb::Prefs<float> stepScale; // 子要素との奥行差の比

	/***** 奥行、子要素
	 */
	float step; // 要素間の奥行きの差
	virtual void SetDepth(float) {};
	void Pick(struct Pane&); // 最前面へ移動/追加
	void Away(struct Pane&); // 最背面へ移動/追加

	/***** 周期処理
	 * デフォルトの、子要素全てを再帰で呼ぶハンドラ
	 */
	virtual void Update(const tb::Timestamp&);
	virtual void Draw();
	virtual void Traw();
	virtual void Draw(const R&);
	virtual void Traw(const R&);
	virtual void DrawNavigation(const P&);


#if 0
	// イベント仕分けハンドラ
	struct Event {
		tb::Timestamp when;
	};

	struct KeyEvent : public Event {
		enum { down, up, repeat } type;
		tb::u32 modifiers;
		tb::u32 charDode;
		tb::u32 keyCode;

		static constexpr tb::u32 shift = 3;
		static constexpr tb::u32 leftShift = 1;
		static constexpr tb::u32 rightShift = 2;
		static constexpr tb::u32 ctrl = 0x0c;
		static constexpr tb::u32 leftCtrl = 4;
		static constexpr tb::u32 rightCtrl = 8;
		static constexpr tb::u32 alt = 0x30;
		static constexpr tb::u32 leftAlt = 0x10;
		static constexpr tb::u32 rightAlt = 0x20;
	};
	virtual bool KeyEvent(const KeyEvent&);

	struct MouseEvent : public Event {
		enum {
			down,
			up,
			move,
			scroll,
			click,
			doubleClick,
			startDrag,
			endDrag
		} type;
		tb::Vector<2, int> where;
		tb::Vector<2, int> starts; // ドラッグ開始位置
	};
	virtual bool MouseEvent(const MouseEvent&);
#endif
};



struct Pane : BasePane, tb::List<Pane>::Node {
protected:
	BasePane& parent;
	Pane(); // 根直下
	Pane(BasePane&);

private:
	// つながってるリストがなくなったら一緒に消滅
	void NotifyListDeleted() final { delete this; };
};
