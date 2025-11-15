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
#include "frame.h"
#include "model.h"
#include "prefs.h"
#include <tb/color.h>
#include <tb/list.h>



namespace widget {

	// ルート直下(NavRingに表示される)
	struct Window : Frame {

		// childを子要素として生成
		Window(Frame& child,
			float depth,
			float thick,
			const tb::Color& leftTopColor = tb::Color(Prefs::foreColor),
			const tb::Color& rightBottomColor = tb::Color(Prefs::backColor),
			unsigned leftMergin = 2,
			unsigned topMergin = 10,
			unsigned rightMergin = 2,
			unsigned bottomMergin = 2);

		static void PrepareNavPanel();

		void Dot() override;
		void Draw(const R&) override;
		Notify Update() override;

	protected:
		struct {
			float left;
			float top;
			float right;
			float bottom;
		} mergin, in, out;
		tb::Color leftTopColor;
		tb::Color rightBottomColor;

	private:
		// 上下左右のウインドウコントロールのサイズ
		static P defaultLeftTopMergin; // content左上からWindow左上の差
		static S defaultSpreadMergin;  // contentの右下分＋leftTopMerginの負値
		void UpdateBorder();

		Window() = delete;
		Window(const Window&) = delete;

		static P MakeLeftTop(
			const Frame&, unsigned leftMergin, unsigned topMergin);
		static S MakeSpread(
			const Frame&, unsigned rightMergin, unsigned bottomMergin);
	};

	struct ResizeableWindow : Window {
		ResizeableWindow(Frame& child,
			float depth = 0,
			float thick = 0,
			unsigned leftMergin = 4,
			unsigned topMergin = 12,
			unsigned rightMergin = 4,
			unsigned bottomMergin = 4) :
			Window(child,
				depth,
				thick,
				defaultLeftTopColor,
				defaultRightBottomColor,
				leftMergin,
				topMergin,
				rightMergin,
				bottomMergin) {};

	private:
		static const tb::Color defaultLeftTopColor;
		static const tb::Color defaultRightBottomColor;
	};
}
