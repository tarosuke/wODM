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
#include "widget/event.h"



namespace widget {

	PtEvent::PtEvent(const P& position, unsigned button, unsigned modifiers) :
		origin{{0.0f, 0.0f}},
		dir(position),
		state(button),
		down(0),
		up(0),
		modifiers(modifiers) {}
	PtEvent::PtEvent(const PtEvent& origin, const P& position) :
		origin(origin.origin - position),
		dir(origin.dir),
		state(origin.state),
		down(origin.down),
		up(origin.up),
		modifiers(origin.modifiers) {}



}
