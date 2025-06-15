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

#include "model.h"
#include "pane/pane.h"
#include <tb/canvas.h>
#include <tb/image.h>
#include <tb/prefs.h>
#include <tb/vector.h>



/***** Widget
 * 画面直下のPane兼制御のエントリ
 * 位置のみを持つ
 * Windowの各要素を子に持つ
 * その関係でstepは0でNextStepは親のNextStepを転送
 */
struct Widget : Pane {

protected:
	/***** 位置、奥行
	 */
	P center;
	P target;
	float depth;
	float targetDepth;



	Widget() = default;



private:
	void Update(const tb::Timestamp&) final;
	void DrawNavigation(const P& lookingPoint) final;


	float NextStep() final {
		return parent.NextStep();
	}; // Widgetのstepは0なので代わりにrootのNextStepを返す



	Widget(const Widget&) = delete;
	void operator=(const Widget&) = delete;
};
