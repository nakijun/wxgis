/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  Main frame class.
 * Author:   Bishop (aka Barishnikov Dmitriy), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2009-2010 Bishop
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

#include "wxgis/framework/framework.h"
#include "wxgis/catalogui/gxapplication.h"

#include "wx/aui/aui.h"
#include "wx/generic/logg.h"
#include "wx/artprov.h"
#include <wx/event.h>
#include <wx/menu.h>

//#include <wx/scrolwin.h>


//-----------------------------------------------------------
// wxGISCatalogFrame
//-----------------------------------------------------------

class wxGISCatalogFrame :
	public wxGxApplication
{
    DECLARE_CLASS(wxGISCatalogFrame)
public:
	wxGISCatalogFrame(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_FRAME_STYLE | wxSUNKEN_BORDER);//| wxWS_EX_VALIDATE_RECURSIVELY
	~wxGISCatalogFrame(void);
//IFrameApplication
	virtual void OnAppAbout(void);
};

