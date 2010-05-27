/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  controls classes.
 * Author:   Bishop (aka Barishnikov Dmitriy), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2009  Bishop
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

#include "wxgis/geoprocessingui/geoprocessingui.h"
#include "wxgis/geoprocessing/geoprocessing.h"

#include <wx/intl.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/statbmp.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/choice.h>
#include <wx/sizer.h>
#include <wx/panel.h>

/////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// Class wxGISDTBase
///////////////////////////////////////////////////////////////////////////////
class wxGISDTBase : public wxPanel 
{
public:
	wxGISDTBase( IGPParameter* pParam, wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 500,300 ), long style = wxTAB_TRAVERSAL );
    virtual ~wxGISDTBase();
    virtual void SetMessage(wxGISEnumGPMessageType nType = wxGISEnumGPMessageUnknown, wxString sMsg = wxEmptyString) = 0;
    virtual bool Validate(void) = 0;
    virtual void Update(void) = 0;
    virtual IGPParameter* GetParameter(void);
protected:
	wxStaticBitmap* m_StateBitmap;
	wxStaticText* m_sParamDisplayName;
	wxStaticBitmap* m_bitmap;
    IGPParameter* m_pParam;
	wxImageList m_ImageList;
};

///////////////////////////////////////////////////////////////////////////////
/// Class wxGISTextCtrl
///////////////////////////////////////////////////////////////////////////////
class wxGISTextCtrl : public wxTextCtrl
{
public:
    wxGISTextCtrl(wxWindow* parent, wxWindowID id, const wxString& value = wxEmptyString, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = 0, const wxValidator& validator = wxDefaultValidator, const wxString& name = wxTextCtrlNameStr);
    virtual ~wxGISTextCtrl(void);
    virtual void OnKillFocus(wxFocusEvent& event);
protected:
    wxGISDTBase* m_pBaseCtrl;

    DECLARE_EVENT_TABLE()
};

///////////////////////////////////////////////////////////////////////////////
/// Class wxGISDTPath
///////////////////////////////////////////////////////////////////////////////
class wxGISDTPath : public wxGISDTBase 
{
public:
	wxGISDTPath( IGPParameter* pParam, IGxCatalog* pCatalog, wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 500,300 ), long style = wxTAB_TRAVERSAL );
	virtual ~wxGISDTPath();	
    virtual void SetMessage(wxGISEnumGPMessageType nType = wxGISEnumGPMessageUnknown, wxString sMsg = wxEmptyString);
//events
    virtual void OnOpen(wxCommandEvent& event);
    virtual bool Validate(void);
    virtual void Update(void);
protected:
    wxGISTextCtrl* m_PathTextCtrl;
	wxBitmapButton* m_bpButton;
    IGxCatalog* m_pCatalog;

    DECLARE_EVENT_TABLE()
};

///////////////////////////////////////////////////////////////////////////////
/// Class wxGISDTChoice
///////////////////////////////////////////////////////////////////////////////
class wxGISDTChoice : public wxPanel 
{
	private:
	
	protected:
		wxStaticBitmap* m_bitmap1;
		wxStaticText* m_staticText1;
		wxStaticBitmap* m_bitmap2;
		wxChoice* m_choice1;
	
	public:
		wxGISDTChoice( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 500,300 ), long style = wxTAB_TRAVERSAL );
		~wxGISDTChoice();
	
};