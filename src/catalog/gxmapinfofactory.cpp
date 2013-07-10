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
#include "wxgis/catalog/gxmapinfofactory.h"
#include "wxgis/catalog/gxdataset.h"
#include "wxgis/datasource/sysop.h"

#include <wx/ffile.h>

//---------------------------------------------------------------------------
// wxGxMapInfoFactory
//---------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxGxMapInfoFactory, wxGxObjectFactory)

wxGxMapInfoFactory::wxGxMapInfoFactory(void)
{
    m_bHasDriver = OGRSFDriverRegistrar::GetRegistrar()->GetDriverByName("MapInfo File");
}

wxGxMapInfoFactory::~wxGxMapInfoFactory(void)
{
}

bool wxGxMapInfoFactory::GetChildren(wxGxObject* pParent, char** &pFileNames, wxArrayLong & pChildrenIds)
{
    for(int i = CSLCount(pFileNames) - 1; i >= 0; i-- )
    {
        wxGxObject* pGxObj = NULL;
        CPLString szExt = CPLGetExtension(pFileNames[i]);
        CPLString szPath;

        if(wxGISEQUAL(szExt, "tab"))
        {
            bool bHasMap(false), bHasInd(false), bHasID(false), bHasDat(false);
            szPath = (char*)CPLResetExtension(pFileNames[i], "dat");
            if(CPLCheckForFile((char*)szPath.c_str(), NULL))
                bHasDat = true;
            if(!bHasDat)
            {
                szPath = (char*)CPLResetExtension(pFileNames[i], "DAT");
                if(CPLCheckForFile((char*)szPath.c_str(), NULL))
                    bHasDat = true;
            }

            szPath = (char*)CPLResetExtension(pFileNames[i], "id");
            if(CPLCheckForFile((char*)szPath.c_str(), NULL))
                bHasID = true;
            if(!bHasID)
            {
                szPath = (char*)CPLResetExtension(pFileNames[i], "ID");
                if(CPLCheckForFile((char*)szPath.c_str(), NULL))
                    bHasID = true;
            }
            szPath = (char*)CPLResetExtension(pFileNames[i], "ind");
            if(CPLCheckForFile((char*)szPath.c_str(), NULL))
                bHasInd = true;
            if(!bHasInd)
            {
                szPath = (char*)CPLResetExtension(pFileNames[i], "IND");
                if(CPLCheckForFile((char*)szPath.c_str(), NULL))
                    bHasInd = true;
            }
            szPath = (char*)CPLResetExtension(pFileNames[i], "map");
            if(CPLCheckForFile((char*)szPath.c_str(), NULL))
                bHasMap = true;
            if(!bHasMap)
            {
                szPath = (char*)CPLResetExtension(pFileNames[i], "MAP");
                if(CPLCheckForFile((char*)szPath.c_str(), NULL))
                    bHasMap = true;
            }

            if(bHasMap && bHasMap && bHasID && bHasDat)
                pGxObj = GetGxObject(pParent, GetConvName(pFileNames[i]), pFileNames[i], enumVecMapinfoTab);
            else if(bHasDat)
                pGxObj = GetGxObject(pParent, GetConvName(pFileNames[i]), pFileNames[i], wxGISEnumVectorDatasetType(emumVecMAX + 1));
            pFileNames = CSLRemoveStrings( pFileNames, i, 1, NULL );
        }
        else if(wxGISEQUAL(szExt, "mif"))
        {
            bool bHasMid(false);
            szPath = (char*)CPLResetExtension(pFileNames[i], "mid");
            if(CPLCheckForFile((char*)szPath.c_str(), NULL))
                bHasMid = true;
            if(bHasMid)
                pGxObj = GetGxObject(pParent, GetConvName(pFileNames[i]), pFileNames[i], enumVecMapinfoMif);
            else
                pGxObj = GetGxObject(pParent, GetConvName(pFileNames[i]), pFileNames[i], wxGISEnumVectorDatasetType(emumVecMAX + 2));
            pFileNames = CSLRemoveStrings( pFileNames, i, 1, NULL );
        }
        else if(wxGISEQUAL(szExt, "map"))
            pFileNames = CSLRemoveStrings( pFileNames, i, 1, NULL );
        else if(wxGISEQUAL(szExt, "ind"))
            pFileNames = CSLRemoveStrings( pFileNames, i, 1, NULL );
        else if(wxGISEQUAL(szExt, "id"))
            pFileNames = CSLRemoveStrings( pFileNames, i, 1, NULL );
        else if(wxGISEQUAL(szExt, "dat"))
        {
            bool bHasTab(false);
            szPath = (char*)CPLResetExtension(pFileNames[i], "tab");
            if(CPLCheckForFile((char*)szPath.c_str(), NULL))
                bHasTab = true;
            if(!bHasTab)
            {
                szPath = (char*)CPLResetExtension(pFileNames[i], "TAB");
                if(CPLCheckForFile((char*)szPath.c_str(), NULL))
                    bHasTab = true;
            }
            if(bHasTab)
                pFileNames = CSLRemoveStrings( pFileNames, i, 1, NULL );
        }
        else if(wxGISEQUAL(szExt, "mid"))
            pFileNames = CSLRemoveStrings( pFileNames, i, 1, NULL );

		if(pGxObj != NULL)
        {
			pChildrenIds.Add(pGxObj->GetId());
            pGxObj = NULL;
        }
    }
	return true;
}

wxGxObject* wxGxMapInfoFactory::GetGxObject(wxGxObject* pParent, const wxString &soName, const CPLString &szPath, wxGISEnumVectorDatasetType type)
{
    if(!m_bHasDriver)
        return NULL;
    if(emumVecMAX + 1)
        return wxStaticCast(new wxGxTableDataset(enumTableMapinfoTab, pParent, soName, szPath), wxGxObject);
	else if(emumVecMAX + 2)
        return wxStaticCast(new wxGxTableDataset(enumTableMapinfoMif, pParent, soName, szPath), wxGxObject);
	wxGxFeatureDataset* pDataset = new wxGxFeatureDataset(type, pParent, soName, szPath);
    return wxStaticCast(pDataset, wxGxObject);
}

