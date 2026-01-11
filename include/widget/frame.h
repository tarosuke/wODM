/*****************************************************************************
 * Copyright (C) 2025, 2026 tarosuke<webmaster@tarosuke.net>
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
#include <tb/geometry/rect.h>
#include <tb/geometry/spread.h>
#include <tb/geometry/vector.h>
#include <tb/list.h>
#include <tb/matrix.h>
#include <tb/prefs.h>
#include <tb/time.h>



namespace widget {

	/***** 位置と大きさのみを持つ
	 */
	struct Frame : tb::List<Frame>::Node {
		using P = tb::geometry::Vector<2, float>;
		using S = tb::geometry::Spread<2, float>;
		using R = tb::geometry::Rect<2, float>;
		using P3 = tb::geometry::Vector<3, float>;
		using S3 = tb::geometry::Spread<3, float>;
		using R3 = tb::geometry::Rect<3, float>;
		using M = tb::Matrix<4, 4, float>;

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
		virtual void ReDepth(float depth, float thick);

		P GetCenter() const;
		const P3& Origin3() const { return rect.Origin(); };
		const S3& Spread3() const { return rect.Spread(); };
		const P Origin() const { return ToP(rect.Origin()); };
		const S Spread() const { return ToS(rect.Spread()); };

	protected:
		static tb::Prefs<float> movingRatio;
		tb::List<Frame> children;
		virtual void Add(Frame& child) {
			children.Add(child);
		}; // 継承先で配置など

		/***** 可視判定
		 */
		const R& GetMask() const { return mask; };
		bool IsShown() const { return shown; };

		/***** 位置、範囲
		 */
		R3 rect;					 // 現在の範囲
		P3 target;					 // 目標点(左上)
		static R3 ToR3(const R& o) { // RからR3(奥行きは0)
			return R3(P3({o.Origin()[0], o.Origin()[1], 0.0f}),
				S3{{o.Spread()[0], o.Spread()[1], 0.0f}});
		};
		static R ToR(const R3& o) { // R3の平面分
			return R(P{{o.Origin()[0], o.Origin()[1]}},
				S{{o.Spread()[0], o.Spread()[1]}});
		};
		static P3 ToP3(const P& o) { // PからP3へ(奥行きは0)
			return P3({o[0], o[1], 0.0f});
		};
		static P ToP(const P3& o) { // P3の平面分
			return P({o[0], o[1]});
		};
		static S3 ToS3(const S& o) { // PからP3へ(奥行きは0)
			return S3({o[0], o[1], 0.0f});
		};
		static S ToS(const P3& o) { // P3の平面分
			return S({o[0], o[1]});
		};
		static tb::geometry::Spread<2, unsigned> ToUS(const R3& o) {
			return tb::geometry::Spread<2, unsigned>(
				{o.Spread(0), o.Spread(1)});
		};
		void AccualMove(); // 実際の移動

		/***** 構築、破壊
		 */
		Frame(Frame& parent, const R3&); // 親ありフル指定
		Frame(const R3&);				 // フル指定Window用
		Frame(const S&);				 // Windowの中身用


		Frame(Frame& parent, const R& rect) : Frame(parent, ToR3(rect)) {};
		Frame(Frame& parent, const P& origin, const S& spread) :
			Frame(parent, R(origin, spread)) {};
		virtual ~Frame() {
			if (ptOn == this) {
				// TODO:カーソルをデフォルトに戻す
				ptOn = 0;
			}
			if (focused == this) {
				focused = 0;
			}
		};



		/***** イベントハンドラ
		 */
		static Frame* ptOn;
		static Frame* focused;
		struct {
			tb::Timestamp time; // 最後のボタン操作時刻
			P pt;			  // 最後にdown / upした場所(動いていなければclick)
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
		// 下の二つは都度生成されるので初期化不要
		R mask;		// 親要素との論理積
		bool shown; // UpdateにてmaskがEmptyでないなら真に設定される
	};



	// 横方向リスト
	struct HorizontalList : Frame {
		HorizontalList(Frame& parent, const R& rect, unsigned spacing = 4);
		HorizontalList(const R& rect, unsigned spacing = 4);

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

		R3 Assign(float width);
	};

	// 縦方向リスト
	struct VerticalList : Frame {
		VerticalList(Frame& parent, const R& rect, unsigned spacing = 4);
		VerticalList(const S&, unsigned spacing = 4);

		template <class T> struct Item : T {
			template <typename... ARGS>
			Item(VerticalList& parent, float height, ARGS... args) :
				T(parent, parent.Assign(height), args...),
				height(height) {
				const float h(parent.rect.Spread(2) * 0.5f);
				(*this).ReDepth(h, h);
			};

		private:
			const float height; // 再配置のために指定された高さ情報を保存
		};

	private:
		const unsigned spacing; // 子要素の上下左右に確保する隙間
		float head;
		float tail;

		R3 Assign(float height);
	};
}
