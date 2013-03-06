/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxFolderFactory class. Create new GxFolder objects
 * Author:   Baryshnikov Dmitriy (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2009-2012 Bishop
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
#include "wxgis/catalog/gxfolderfactory.h"
#include "wxgis/catalog/gxfolder.h"

#include "wx/filename.h"
#include "wx/dir.h"

//------------------------------------------------------------------------------
// wxGxFolderFactory
//------------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxGxFolderFactory, wxGxObjectFactory)

wxGxFolderFactory::wxGxFolderFactory(void)
{
}

wxGxFolderFactory::~wxGxFolderFactory(void)
{
}

bool wxGxFolderFactory::GetChildren(wxGxObject* pParent, char** &pFileNames, wxArrayLong & pChildrenIds)
{
    wxGxCatalogBase* pCatalog = GetGxCatalog();
    for(int i = CSLCount(pFileNames) - 1; i >= 0; i-- )
    {
        wxString path(pFileNames[i], wxConvUTF8);
		if(wxFileName::DirExists(path))
		{
            wxFileName FName(path);
            wxString sName = FName.GetFullName();

			wxGxObject* pObj = GetGxObject(pParent, sName, pFileNames[i]); 
            if(pObj)
                pChildrenIds.Add(pObj->GetId());
            pFileNames = CSLRemoveStrings( pFileNames, i, 1, NULL );
		}
    }
	return true;
}

wxGxObject* wxGxFolderFactory::GetGxObject(wxGxObject* pParent, const wxString &soName, const CPLString &szPath)
{
	wxGxFolder* pFolder = new wxGxFolder(pParent, soName, szPath);
	return static_cast<wxGxObject*>(pFolder);
}
