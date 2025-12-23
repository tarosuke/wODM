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

#include "event.h"
#include <tb/list.h>
#include <tb/matrix.h>
#include <tb/prefs.h>
#include <tb/rect.h>
#include <tb/spread.h>
#include <tb/time.h>
#include <tb/vector.h>



namespace widget {

	/***** 位置と大きさのみを持つ
	 */
	struct Frame : tb::List<Frame>::Node {
		using P = tb::Vector<3, float>;
		using S = tb::Spread<3, unsigned>;
		using R = tb::Rect<2, float>;
		using M = tb::Matrix<4, 4, float>;
		using P2 = tb::Vector<2, float>;
		using S2 = tb::Spread<2, unsigned>;
		struct RR {
			P position;
			S spread;
		};

		/***** 周期処理、奥行き再計算のインターフェイス
		 */
		virtual Notify Update();
		virtual void Focus() {}; // フォーカスを設定する
		virtual void Sort() {};	 // 子要素を整列

		/***** 描画
		 * マスクを計算して移動、描画ハンドラ呼び出しと子要素の描画
		 */
		virtual void Dot() {};
		void DrawEntity(const R&);
		void TrawEntity();
		virtual void Draw(const R&) {};
		virtual void Traw() {};

		// 移動、リサイズ
		void Move(const P&);   // 引数を差分として移動
		void MoveTo(const P&); // 引数位置へ移動
		void JumpTo(const P&); // 引数位置へ即時移動
		void ReSize(const S&); // リサイズ(即時)
		void SetDepth(float);  // 奥行だけ設定

		P GetCenter() const;
		R GetRect() const;
		const S GetSpread() const { return spread; };
		float GetDepth() const { return position[2]; };

	protected:
		static tb::Prefs<float> movingRatio;
		tb::List<Frame> children;

		/***** 可視判定
		 */
		const R& GetMask() const { return mask; };
		bool IsShown() const { return shown; };

		/***** 位置、範囲
		 * Rで持つと加算誤差が複数要素で蓄積するためPとSで持つ
		 */
		P target;
		P position;
		S spread;

		Frame(Frame& parent, const P& position, const S& spread) :
			target(position),
			position(position),
			spread(spread) {
			parent.children.Insert(*this);
		};
		Frame(Frame& parent, const RR& rect) :
			Frame(parent, rect.position, rect.spread) {};
		// parent非指定の場合はWindowをnewしてその子にする
		Frame(const P& position, const S& spread);
		Frame(const RR& rect) : Frame(rect.position, rect.spread) {};
		// 位置指定がない場合はWindow
		Frame(const S& spread);
		virtual ~Frame() {
			if (ptOn == this) {
				// TODO:カーソルをデフォルトに戻す
				ptOn = 0;
			}
			if (focused == this) {
				focused = 0;
			}
		};
		void AccualMove(); // 実際の移動

		/***** イベントハンドラ
		 */
		static Frame* ptOn;
		static Frame* focused;
		struct {
			tb::Timestamp time; // 最後のボタン操作時刻
			P2 pt;			  // 最後にdown / upした場所(動いていなければclick)
			unsigned buttons; // クリック計測中のボタン
			unsigned n;		  // クリック数(移動したりup & maskが0ならリセット)
		} click;
		bool OnEvent(const PtEvent&);
		virtual void OnEnter(const PtEvent&) {};
		virtual void OnMove(const PtEvent&) {};
		virtual void OnLeave(const PtEvent&) {};
		virtual void OnDown(const PtEvent&) {};
		virtual void OnUp(const PtEvent&) {};
		virtual void OnClick(const PtEvent&) {};
		bool OnEvent(const KeyEvent&);
		virtual void OnKeyDown(const KeyEvent&) {};
		virtual void OnKeyUp(const KeyEvent&) {};
		virtual void OnKeyRepeat(const KeyEvent&) {};

	private:
		R mask;		// 親要素との論理積
		bool shown; // UpdateにてmaskがEmptyでないなら真に設定される
	};



	// 横方向リスト
	struct HorizontalList : Frame {
		HorizontalList(Frame& parent,
			const P& position,
			const S& spread,
			unsigned spacing = 4);
		HorizontalList(
			const P& position, const S& spread, unsigned spacing = 4);

		template <class T> struct Item : T {
			template <typename... ARGS>
			Item(HorizontalList& parent, float width, ARGS... args) :
				T(parent, parent.Assign(width), args...),
				width(width) {}

		private:
			const float width; // 指定された幅を保存
		};

	private:
		const unsigned spacing; // 子要素の上下左右に確保する隙間
		float head;
		float tail;

		RR Assign(float width) { // 配置
			RR r{.position = {(float)spacing, 0.0f, 0.0f},
				.spread = {spread[1] - spacing * 2U, 0U, 0U}};
			if (0.0 <= width) {
				// 左寄せ
				const float w(1.0 < width ? width : (tail - head) * width);
				r.position[1] = head + spacing;
				r.spread[1] = w;
				head += spacing + w;
			} else {
				// 右寄せ
				const float w(width < -1.0 ? width : (tail - head) * width);
				r.position[1] = tail + w - spacing;
				r.spread[1] = -w;
				tail += spacing + w;
			}
			return r;
		};
	};

	// 縦方向リスト
	struct VerticalList : Frame {
		VerticalList(Frame& parent,
			const P& position,
			const S& spread,
			unsigned spacing = 4);
		VerticalList(const P& position, const S& spread, unsigned spacing = 4);

		template <class T> struct Item : T {
			template <typename... ARGS>
			Item(VerticalList& parent, float height, ARGS... args) :
				T(parent, parent.Assign(height), args...),
				height(height) {}

		private:
			const float height; // 指定された高さを保存
		};

	private:
		const unsigned spacing; // 子要素の上下左右に確保する隙間
		float head;
		float tail;

		RR Assign(float height) { // 配置
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
				tail += spacing + h;
			}
			return r;
		};
	};

}
