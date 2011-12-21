/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGISTableView class.
 * Author:   Bishop (aka Baryshnikov Dmitriy), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2009,2011 Bishop
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
#include "wxgis/catalogui/gxview.h"
#include "wxgis/cartoui/tableview.h"
#include "wxgis/catalogui/gxcatalogui.h"
#include "wxgis/catalogui/gxeventui.h"

class WXDLLIMPEXP_GIS_CLU wxGxTableView :
	public wxGISTableView, 
	public wxGxView
{
    DECLARE_DYNAMIC_CLASS(wxGxTableView)
public:
    wxGxTableView(void);
	wxGxTableView(wxWindow* parent, wxWindowID id = TABLECTRLID, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize);
	virtual ~wxGxTableView(void);
//IGxView
    virtual bool Create(wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxTAB_TRAVERSAL, const wxString& name = wxT("TableView"));
	virtual bool Activate(IFrameApplication* application, wxXmlNode* pConf);
	virtual void Deactivate(void);
	virtual bool Applies(IGxSelection* Selection);
//events
	virtual void OnSelectionChanged(wxGxSelectionEvent& event);
private:
	IGxSelection* m_pSelection;
	long m_nParentGxObjectID;
	wxGxCatalogUI* m_pCatalog;

	DECLARE_EVENT_TABLE()
};
