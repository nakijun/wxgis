/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGISCartoUI main header.
 * Author:   Bishop (aka Barishnikov Dmitriy), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2009  Bishop
*
*    This program is free software: you can redistribute it and/or modify
*    it under the terms of the GNU General Public License as published by
*    the Free Software Foundation, either version 3 of the License, or
*    (at your option) any later version.
*
*    This program is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 ****************************************************************************/

#pragma once

#include "wxgis/carto/carto.h"

enum wxGISEnumMapToolState
{
	enumGISMapNone		= 0x0000, 
	enumGISMapPanning	= 0x0001,
	enumGISMapZooming	= 0x0002,
	enumGISMapRotating	= 0x0004,
	enumGISMapDrawing	= 0x0008,
	enumGISMapWheeling	= 0x0010,
	enumGISMapWheelingStop	= 0x0020
};
