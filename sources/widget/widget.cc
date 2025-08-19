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
#include "widget/widget.h"
#include "gl/gl.h"



namespace widget {

	void Widget::ReDepth(float d, float t) {
		target[2] = d;
		thick = t;

		/***** thickを子要素へ割り振る
		 * 子要素の奥行を先頭から n/(n+1) で割り当てる
		 */
		unsigned n(2);
		float p(thick);
		for (tb::List<Base>::I i(children); ++i; ++n) {
			const float q(thick / n);
			ReDepth(q, p - q);
			p = q;
		}
	}

}
