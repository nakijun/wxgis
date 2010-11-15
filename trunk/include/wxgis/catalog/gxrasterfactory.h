/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxRasterFactory class.
 * Author:   Bishop (aka Barishnikov Dmitriy), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2009-2010  Bishop
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

#include "wxgis/catalog/catalog.h"

/** \class wxGxRasterFactory gxrasterfactory.h
    \brief A raster GxObject factory.
*/
class WXDLLIMPEXP_GIS_CLT wxGxRasterFactory :
	public IGxObjectFactory,
	public wxObject
{
	DECLARE_DYNAMIC_CLASS(wxGxRasterFactory)
public:
	wxGxRasterFactory(void);
	virtual ~wxGxRasterFactory(void);
	//IGxObjectFactory
	virtual bool GetChildren(wxString sParentDir, wxArrayString* pFileNames, GxObjectArray* pObjArray);
    virtual void Serialize(wxXmlNode* pConfig, bool bStore);
    virtual wxString GetClassName(void){return GetClassInfo()->GetClassName();};
    virtual wxString GetName(void){return wxString(_("Raster files"));};
    //wxGxRasterFactory
    virtual IGxObject* GetGxDataset(wxString path, wxString name, wxGISEnumRasterDatasetType type);
};