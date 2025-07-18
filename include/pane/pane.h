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

#include "pane/position.h"



struct Pane : tb::List<Pane>::Node {
	using P = tb::Vector<2, float>;
	using S = tb::Spread<2, unsigned>;
	using R = tb::Rect<2, float>;
	using A = tb::Matrix<4, 4, float>;


	virtual void SetDepth(float depth, float thickness) {};
	virtual void ReDepth(); // 再帰的に子要素の奥行を再計算



	/***** コンストラクタ、デストラクタ
	 * NOTE:親要素への追加はコンストラクタから戻ってから
	 */
protected:
	Pane() = default;
	Pane(const Pane&) = delete;
	void operator=(const Pane&) = delete;
	virtual ~Pane() {};


	/***** 子要素管理
	 * 子要素の追加、奥行管理
	 * ※削除はデストラクタによる
	 */
public:
	void AddHead(struct Pane&); // 引数のPaneを自身のchildrennの先頭に追加
	void AddTail(struct Pane&); // 引数のPaneを自身のchildrennの末尾に追加
	void Pick() { notify.bits.pick = 1; }; // Update死にpickUpしてもらう

protected:
	tb::List<struct Pane> children;


	/***** 周期処理
	 * デフォルトの、子要素全てを再帰で呼ぶハンドラ
	 * TODO:Updateの戻り値でpickなどのリクエストを返すことでparentを廃止
	 */
public:
	union Notify {
		// Updateの戻り値で親インスタンスに伝える通知
		unsigned raw;
		struct {
			unsigned pick : 1;	 // 最前面へ
			unsigned notify : 1; // 通知が必要
		} bits;
		void operator|=(const Notify& t) {
			// rawをorする
			raw |= t.raw;
		};
	};
	virtual Notify Update(const tb::Timestamp&);
	virtual void Draw();
	virtual void Traw();
	virtual void Draw(const R&);
	virtual void Traw(const R&);
	virtual void DrawNavigation(const P&);

protected:
	static Pane* lastPicked;
	static Pane* lastNotified;
	Notify notify;

private:
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

private:
	// つながってるリストがなくなったら一緒に消滅
	void NotifyListDeleted() final { delete this; };
};
