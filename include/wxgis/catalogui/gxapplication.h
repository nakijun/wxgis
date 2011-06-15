/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxApplication main header.
 * Author:   Bishop (aka Barishnikov Dmitriy), polimax@mail.ru
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

#include "wxgis/framework/applicationex.h"
#include "wxgis/catalogui/gxcatalogui.h"
#include "wxgis/catalogui/gxtreeview.h"
#include "wxgis/catalogui/gxtabview.h"
#include "wxgis/catalogui/newmenu.h"

#include "wx/aui/aui.h"
#include "wx/artprov.h"

/** \class wxGxApplication gxapplication.h
 *   \brief A catalog application framework class.
 */
class WXDLLIMPEXP_GIS_CLU wxGxApplication :
	public wxGISApplicationEx,
    public IGxApplication
{
    DECLARE_CLASS(wxGxApplication)
public:	
	wxGxApplication(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_FRAME_STYLE | wxSUNKEN_BORDER);
	virtual ~wxGxApplication(void);
    //IGxApplication
	virtual IGxCatalog* const GetCatalog(void);
    //events 
    virtual void OnClose(wxCloseEvent& event);
	//IApplication
    virtual wxString GetAppName(void){return wxString(wxT("wxGISCatalog"));};
    virtual bool Create(IGISConfig* pConfig);
	virtual wxIcon GetAppIcon(void){return m_pAppIcon;};
protected:
	wxGxTreeView* m_pTreeView;
	wxGxTabView* m_pTabView;
	wxGxCatalogUI* m_pCatalog;
    wxGISNewMenu* m_pNewMenu;
	wxIcon m_pAppIcon;
};
