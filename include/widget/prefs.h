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

#include <tb/prefs.h>



namespace widget {

	struct Prefs {
		static tb::Prefs<unsigned> foreColor;
		static tb::Prefs<unsigned> backColor;
		static tb::Prefs<unsigned> backColor2;

		static tb::Prefs<unsigned> navRingColor;
		static tb::Prefs<float> navThick;
		static tb::Prefs<float> nDistance;

		static tb::Prefs<float> pDistance;
		static tb::Prefs<float> vDistance;
		static tb::Prefs<float> scale;
		static tb::Prefs<float> windowThick;
	};
}
