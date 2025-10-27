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

#include "content.h"
#include "frame.h"


/***** TODO:子クラスとして以下を作る
 * 整列だけでなく子要素がpickされたときの挙動も異なる
 * 1. 順番変更及び奥行き整列→Window
 * 2. 縦/横整列→リスト、テキストなど
 * 3. 奥行き変更、整列なし、最初の要素のみ描画→タブによる切り替えなど
 */

namespace widget {

	// 範囲と内容の描画を追加(奥行き制御なし)
	template <class CONTENT = Content, bool TRANSLUCENT = false>
	struct Pane : Frame, CONTENT {
	protected:
		/***** 奥行きを含めた範囲を持つ
		 * NOTE:parentを指定すると先頭に追加されて奥行き設定が予約される
		 * 先頭でないなら呼び出し側が追加する必要がある
		 */
		Pane(const P& position, const S& spread, float depth, float thick) :
			Frame(position, spread, depth, thick) {};
		Pane(Base& parent,
			const P& position,
			const S& spread,
			float depth,
			float thick) :
			Frame(position, spread, depth, thick) {};


		void Draw() override { Base::Draw(); };
		void Draw(const R& r) override {
			if ((shown = !(mask = Mask(r)).IsEmpty())) {
				if constexpr (TRANSLUCENT) {
					Base::Draw(mask);
				} else {
					Base::Draw(mask);
					CONTENT::DrawContent(mask);
				}
			}
		};
		void Traw() override {
			if (shown) {
				if constexpr (TRANSLUCENT) {
					CONTENT::TrawContent(mask);
					Base::Traw();
				} else {
					Base::Traw();
				}
			}
		};
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
