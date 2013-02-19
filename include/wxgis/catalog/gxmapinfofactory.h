/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxMapInfoFactory class.
 * Author:   Baryshnikov Dmitriy (aka Bishop), polimax@mail.ru
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

/** \class wxGxMapInfoFactory gxmapinfofactory.h
    \brief A MapInfo GxObject factory.
*/
/*
class WXDLLIMPEXP_GIS_CLT wxGxMapInfoFactory :
	public IGxObjectFactory,
	public wxObject
{
	DECLARE_DYNAMIC_CLASS(wxGxMapInfoFactory)
public:
	wxGxMapInfoFactory(void);
	virtual ~wxGxMapInfoFactory(void);
	//IGxObjectFactory
	virtual bool GetChildren(CPLString sParentDir, char** &pFileNames, GxObjectArray &ObjArray);
    virtual void Serialize(wxXmlNode* const pConfig, bool bStore);
	virtual wxString GetClassName(void){return GetClassInfo()->GetClassName();};
    virtual wxString GetName(void){return wxString(_("Mapinfo files"));};
    //wxGxMapInfoFactory
    virtual IGxObject* GetGxDataset(CPLString path, wxString name, wxGISEnumVectorDatasetType type);
};
*/