/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxFolderFactory class. Create new GxFolder objects
 * Author:   Bishop (aka Baryshnikov Dmitriy), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2009-2011 Bishop
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


class WXDLLIMPEXP_GIS_CLT wxGxFolderFactory :
	public IGxObjectFactory,
	public wxObject
{
	DECLARE_DYNAMIC_CLASS(wxGxFolderFactory)
public:
	wxGxFolderFactory(void);
	virtual ~wxGxFolderFactory(void);
	//IGxObjectFactory
	virtual bool GetChildren(CPLString sParentDir, char** &pFileNames, GxObjectArray &ObjArray);
    virtual void Serialize(wxXmlNode* const pConfig, bool bStore);
    virtual wxString GetClassName(void){return GetClassInfo()->GetClassName();};
    virtual wxString GetName(void){return wxString(_("Folders"));};
    virtual IGxObject* GetGxObject(CPLString szPath, wxString soName);
};
