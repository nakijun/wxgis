/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGISRasterPropertyPage class.
 * Author:   Baryshnikov Dmitriy (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2010-2011,2013 Bishop
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
#include "wxgis/catalog/gxdataset.h"
#include "wxgis/datasource/rasterdataset.h"
#include "wxgis/geoprocessing/geoprocessing.h"
#include "wxgis/geoprocessingui/geoprocessingui.h"
#include "wxgis/core/event.h"

#include "wx/propgrid/propgrid.h"

/** \class wxGISRasterPropertyPage rasterpropertypage.h
    \brief The tab of raster dataset properties.
*/

class WXDLLIMPEXP_GIS_CLU wxGISRasterPropertyPage : 
    public wxPanel
{
    DECLARE_DYNAMIC_CLASS(wxGISRasterPropertyPage)
	enum
	{
		ID_PPCTRL = wxID_HIGHEST + 4504
	};

public:
    wxGISRasterPropertyPage(void);
    wxGISRasterPropertyPage(wxGxRasterDataset* pGxDataset, wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 420,540 ), long style = wxTAB_TRAVERSAL, const wxString& name = wxT("Raster"));
	~wxGISRasterPropertyPage();
    virtual bool Create(wxGxRasterDataset* pGxDataset, wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 420,540 ), long style = wxTAB_TRAVERSAL, const wxString& name = wxT("Raster"));
    virtual wxString GetPageName(void) const {return wxString(_("Raster"));};
    wxPGProperty* AppendProperty(wxPGProperty* pProp);
    wxPGProperty* AppendProperty(wxPGProperty* pid, wxPGProperty* pProp);
    wxPGProperty* AppendMetadataProperty(wxPGProperty* pid, wxString sMeta);
    wxPGProperty* GetSubProperty(wxPGProperty* pid, const wxString &sName, wxString &sResultName);
    void FillGrid(void);
    //events
    void OnPropertyGridButtonClick ( wxCommandEvent& );
    void OnFinish(wxGISProcessEvent& event);
protected:
    wxGISRasterDataset* m_pDataset;
    wxGxRasterDataset* m_pGxDataset;
    wxPropertyGrid* m_pg;
	long m_nCounter;
	//long m_nCookie;
	//IToolManagerUI* m_pToolManagerUI;

    DECLARE_EVENT_TABLE()
};

