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
#include "window.h"



namespace widget {
	struct Root {
		Root(const tb::List<Eye>&);
		~Root();

		// 周期処理(navPanelだけでなくWidgetの管理もここが入口)
		void Update();
		void DrawAll(const Eye&);
		void TrawAll(const Eye&);

		// navPanel上に点を描画(Windowから呼ばれる)
		static void Dot(const Frame::P& center);

		static void Register(Window&);
		static void ReDepthAll();

		// イベントの元
		static void PointerNotify(const PtEvent&);

	private:
		static tb::List<Frame> windows;
		Model_C* navPanel;
		static Frame::P lookingPoint;
		static Frame::M viewMat;

		static Model_C* PrepareNavPanel(const tb::List<Eye>&);

		Root() = delete;
		Root(const Root&) = delete;

		Frame::R mask;
		const Frame::R& GetMask() { return mask; };
	};
}
