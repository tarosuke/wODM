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

#include "eye.h"
#include "model.h"
#include "pane.h"
#include <tb/color.h>
#include <tb/list.h>



namespace widget {

	// ルート直下(NavRingに表示される)
	struct Window : Pane {

		// TODO:Paramsは廃止、Prefsから値を直接取得
		struct Mergin {
			float left;
			float top;
			float right;
			float bottom;
		};
		struct Params {
			tb::Color leftTopColor;
			tb::Color rightBottomColor;
			Mergin mergin;
		};


		/***** 構築
		 */

		/* Frame&を子要素としてその大きさに合わせて作る
		 * 合わせるのは大きさだけで、位置はは無視される
		 * また子要素の位置は0,0に変更される
		 */
		Window(Frame&, const Params& params = defaultParams);


		static void PrepareNavPanel();

		void Dot() override;
		void Draw(const R&) override;
		Notify Update() override;

	protected:
		// 規定の設定
		static const Params defaultParams;

		Mergin mergin, in, out;
		tb::Color leftTopColor;
		tb::Color rightBottomColor;

		void Sort() override;

	private:
		void UpdateBorder();

		Window() = delete;
		Window(const Window&) = delete;

		static R3 MakeRect(const Frame&, const Params&);
	};

	struct ResizeableWindow : Window {
		ResizeableWindow(Frame& child,
			float depth = 0,
			float thick = 0,
			const Params& params = defaultParams) :
			Window(child, defaultParams) {};

	private:
		static const tb::Color defaultLeftTopColor;
		static const tb::Color defaultRightBottomColor;
	};
}
