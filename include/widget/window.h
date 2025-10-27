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
#include <tb/list.h>



namespace widget {

	// ルート直下(NavRingに表示される)
	struct Window : Frame {

		Window(
			const P& center, const S& spread, float depth = 0, float thick = 0);

		static void PrepareNavPanel();

		void Dot() override;
		void Draw(const R&) override;
		void Traw() override;

	private:
		// 上下左右のウインドウコントロールのサイズ
		static P GetLeftTop(const P& center, const S& contentSpread);
		static S GetWindowSpread(const S& contentSpread);
		static P leftTopMergin; // content左上からWindow左上の差
		static S spreadMergin;	// contentの右下分＋leftTopMerginの負値

		tb::List<Frame> controls;
		R contentRect;

		Window() = delete;
		Window(const Window&) = delete;
	};
}
