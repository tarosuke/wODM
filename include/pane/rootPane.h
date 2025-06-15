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

#include "pane.h"
#include <tb/prefs.h>



struct RootPane : BasePane {
	static void UpdateAll(const A&, const tb::Timestamp&);
	static void DrawAll(const A& eye2Head);
	static void TrawAll(const A& eye2Head);


	RootPane() : BasePane(-baseStep) { UpdateNav(); };

private:
	static tb::Prefs<float> vDistance;
	static tb::Prefs<float> scale;
	static tb::Prefs<float> navigationAngle;
	static tb::Prefs<float> navigationThick;

	static P lookingPoint;

	void UpdateNav();
};
