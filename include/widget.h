/*****************************************************************************
 * Copyright (C) 2024,2025 tarosuke<webmaster@tarosuke.net>
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

#include "pane/pane.h"


/***** Widget
 * 奥行き自動制御機構のあるPane
 */
struct Widget : Pane {
private:
	static tb::Prefs<float> baseThickness; // 根直下の要素間の奥行きの差
	static tb::Prefs<float> thickRatio;	   // 奥行差の親要素との比
	float thickness;					   // 奥行占有範囲

	void ReDepth();
	Notify Update(const tb::Timestamp&) override;
};
