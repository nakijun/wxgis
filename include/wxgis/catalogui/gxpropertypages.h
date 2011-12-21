/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  PropertyPages of Catalog.
 * Author:   Bishop (aka Baryshnikov Dmitriy), polimax@mail.ru
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
#pragma once
#include "wxgis/framework/framework.h"
#include "wxgis/catalogui/gxcatalogui.h"
#include "wxgis/framework/checklist.h"

#include <wx/intl.h>

#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include "wx/listctrl.h"
#include <wx/button.h>
#include <wx/sizer.h>
#include <wx/statbox.h>
#include <wx/checkbox.h>
#include <wx/panel.h>

///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// Class wxGISCatalogGeneralPropertyPage
///////////////////////////////////////////////////////////////////////////////
class wxGISCatalogGeneralPropertyPage : 
    public IPropertyPage
{
    DECLARE_DYNAMIC_CLASS(wxGISCatalogGeneralPropertyPage)
	enum
	{
		ID_ROOTLISTCTRL = wxID_HIGHEST + 4505,
		ID_FACTORYLISTCTRL
	};

public:
    wxGISCatalogGeneralPropertyPage(void);
	~wxGISCatalogGeneralPropertyPage();
    virtual bool Create(IFrameApplication* application, wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 420,540 ), long style = wxTAB_TRAVERSAL, const wxString& name = wxT("General_Panel"));
//  IPropertyPage    
    virtual wxString GetPageName(void){return wxString(_("General"));};
    virtual void Apply(void);
protected:
    wxGISCheckList* m_pRootItems;
    wxGISCheckList* m_pFactoryItems;
	wxButton* m_button2;
	wxButton* m_button3;
	wxCheckBox* m_checkBoxHideExt;
	wxCheckBox* m_checkBoxLast;
	wxCheckBox* m_checkBoxHidden;
    wxGxCatalogUI* m_pCatalog;
};