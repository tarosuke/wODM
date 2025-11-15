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
#include "widget/prefs.h"



namespace widget {

	tb::Prefs<unsigned> Prefs::foreColor(
		"widget/foreColor", 0xffffff, "前景色");
	tb::Prefs<unsigned> Prefs::backColor(
		"widget/backColor", 0x403030, "背景色");
	tb::Prefs<unsigned> Prefs::backColor2(
		"widget/backColor2", 0x304030, "背景色2");

	tb::Prefs<unsigned> Prefs::navRingColor(
		"widget/navRingColor", 0x80c080, "ナビゲーションリングの色(透過率)");
	tb::Prefs<float> Prefs::pDistance(
		"widget/paneDistance", 1.0f, "一番手前のWidgetが見える奥行き[m]");
	tb::Prefs<float> Prefs::vDistance("widget/virtualDistance",
		1.0f,
		"視野中心を決めるための視点の投影面までの距離[m]");
	tb::Prefs<float> Prefs::nDistance(
		"widget/navDistance", 0.6f, "ナビゲーションリングの奥行き[m]");
	tb::Prefs<float> Prefs::scale("widget/scale", 0.001f, "1pxのサイズ[m]");
	tb::Prefs<float> Prefs::navThick(
		"widget/navigationAngle", 16.0f, "ナビゲーションリングの太さ[px]");
	tb::Prefs<float> Prefs::windowThick("widget/windowThick",
		10.0f,
		"Windowの奥行き(この値だけ基準面から屋に配置される)");
}
