/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxRasterFactoryUI class.
 * Author:   Bishop (aka Barishnikov Dmitriy), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2010  Bishop
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

#include "wxgis/catalogui/catalogui.h"
#include "wxgis/catalog/gxrasterfactory.h"

/** \class wxGxRasterFactoryUI gxrasterfactoryui.h
    \brief A raster GxObject factory.
*/
class wxGxRasterFactoryUI :
	public wxGxRasterFactory
{
	DECLARE_DYNAMIC_CLASS(wxGxRasterFactoryUI)
public:
	wxGxRasterFactoryUI(void);
	virtual ~wxGxRasterFactoryUI(void);
    //wxGxRasterFactory
    virtual IGxObject* GetGxDataset(CPLString path, wxString name, wxGISEnumRasterDatasetType type);
protected:
    wxIcon m_LargeIcon, m_SmallIcon;
};