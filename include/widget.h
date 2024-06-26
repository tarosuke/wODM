/*****************************************************************************
 * Copyright (C) 2024 tarosuke<webmaster@tarosuke.net>
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

#include <tb/list.h>
#include <tb/matrix.h>
#include <tb/rect.h>
#include <tb/time.h>



struct Widget {
	static void UpdateAll(const tb::Matrix<4, 4, float>&);
	static void DrawAll();
	static void TrawAll();

	static const tb::Vector<2, float>& LookingPoint() { return lookingPoint; };

protected:
	virtual void Update(){};
	virtual void Draw(){}; // TODO:クリッピング領域を引数に渡す
	virtual void Traw(){};

private:
	static tb::List<Widget> root;
	static tb::Vector<2, float> lookingPoint;
	tb::List<Widget> children;
};
