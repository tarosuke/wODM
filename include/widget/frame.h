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
		using P = tb::Vector<2, float>;
		using S = tb::Spread<2, unsigned>;
		using R = tb::Rect<2, float>;
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
		void ReSize(const S&); // リサイズ(即時)
		void SetDepth(float);  // 奥行だけ設定

		P GetCenter() const;
		R GetRect() const { return R(position, spread); };
		const S GetSpread() const { return spread; };
		float GetDepth() const { return depth; };

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

		float depth;
		float depthTarget;
		float thick; // 使える奥行:ReDepthではこれを子要素に分配する

		Frame(const P& position, const S& spread, float depth, float thick) :
			target(position),
			position(position),
			spread(spread),
			depth(depth),
			thick(thick) {};
		Frame(Frame* const parent,
			const P& position,
			const S& spread,
			float depth,
			float thick) :
			target(position),
			position(position),
			spread(spread),
			depth(depth),
			thick(thick) {
			if (parent) {
				parent->children.Insert(*this);
			} else {
				throw -1;
			}
		};
		void AccualMove(); // 実際の移動

	private:
		R mask;
		bool shown; // maskがEmptyでないなら真、またContent更新を優先
	};


	struct HorizontalList : Frame {
		void Sort() override; // 子要素を横に整列して自身のサイズを更新
		Notify Update() override;
	};

	struct VerticalList : Frame {
		void Sort() override; // 子要素を縦に整列して自身のサイズを更新
		Notify Update() override;
	};

	struct SelectedList : Frame {
		Notify Update() override;

		// 先頭の子だけを描画
		void Draw(const R&) override;
		void Traw() override;
	};
}
