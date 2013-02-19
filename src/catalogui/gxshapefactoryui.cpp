/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxShapeFactoryUI class.
 * Author:   Baryshnikov Dmitriy (aka Bishop), polimax@mail.ru
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
#include "wxgis/catalogui/gxshapefactoryui.h"

#include "wxgis/catalogui/gxdatasetui.h"

#include "../../art/shp_dset_16.xpm"
#include "../../art/shp_dset_48.xpm"
#include "../../art/table_dbf_16.xpm"
#include "../../art/table_dbf_48.xpm"


IMPLEMENT_DYNAMIC_CLASS(wxGxShapeFactoryUI, wxGxShapeFactory)

wxGxShapeFactoryUI::wxGxShapeFactoryUI(void) : wxGxShapeFactory()
{
}

wxGxShapeFactoryUI::~wxGxShapeFactoryUI(void)
{
}

wxGxObject* wxGxShapeFactoryUI::GetGxObject(wxGxObject* pParent, const wxString &soName, const CPLString &szPath, wxGISEnumDatasetType type)
{
    switch(type)
    {
    case enumGISFeatureDataset:
        {
            if(!m_SmallSHPIcon.IsOk())
                m_SmallSHPIcon = wxIcon(shp_dset_16_xpm);
            if(!m_LargeSHPIcon.IsOk())
                m_LargeSHPIcon = wxIcon(shp_dset_48_xpm);
	        wxGxFeatureDatasetUI* pDataset = new wxGxFeatureDatasetUI(enumVecESRIShapefile, pParent, soName, szPath, m_LargeSHPIcon, m_SmallSHPIcon);
            return wxStaticCast(pDataset, wxGxObject);
        }
    case enumGISTableDataset:
        {
            if(!m_SmallDBFIcon.IsOk())
                m_SmallDBFIcon = wxIcon(table_dbf_16_xpm);
            if(!m_LargeDBFIcon.IsOk())
                m_LargeDBFIcon = wxIcon(table_dbf_48_xpm);
            wxGxTableDatasetUI* pDataset = new wxGxTableDatasetUI(enumTableDBF, pParent, soName, szPath, m_LargeDBFIcon, m_SmallDBFIcon);
            return wxStaticCast(pDataset, wxGxObject);
        }
    }
    return NULL;
}
