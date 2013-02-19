/******************************************************************************
 * Project:  wxGIS
 * Purpose:  wxGISApplication class. Base application functionality (commands, menues, etc.)
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
#pragma once

#include "wxgis/framework/applicationbase.h"
#include "wxgis/framework/accelerator.h"

#include "wx/ffile.h"
#include "wx/dir.h"
#include "wx/filename.h"
#include "wx/file.h"
#include "wx/datetime.h"
#include "wx/intl.h"

/** \class wxGISApplication application.h
    \brief A wxGISApplication class.  
*/
class WXDLLIMPEXP_GIS_FRW wxGISApplication :
	public wxFrame,	public wxGISApplicationBase
{
    DECLARE_CLASS(wxGISApplication)
public:
	//constructor
	wxGISApplication(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_FRAME_STYLE | wxSUNKEN_BORDER );
	//destructor
	virtual ~wxGISApplication(void);
	virtual wxStatusBar* OnCreateStatusBar(int number, long style, wxWindowID id, const wxString& name);
	virtual wxGISAcceleratorTable* GetGISAcceleratorTable(void) const;
//wxGISApplicationBase
    wxGISStatusBar* GetStatusBar(void) const;
	virtual void ShowStatusBar(bool bShow);
	virtual bool IsStatusBarShown(void);
	virtual void ShowToolBarMenu(void);
 //IApplication
    virtual bool CreateApp(void);
    virtual void OnAppOptions(void);
    virtual bool SetupLog(const wxString &sLogPath, const wxString &sNamePrefix = wxEmptyString);
    virtual bool SetupSys(const wxString &sSysPath);
    virtual bool SetupLoc(const wxString &sLoc, const wxString &sLocPath);
    virtual wxString GetDecimalPoint(void) const{return m_sDecimalPoint;};
protected:
	virtual void SerializeFramePos(bool bSave = false);
    virtual void LoadToolbars(wxXmlNode* pRootNode);
    virtual void SerializeCommandBars(bool bSave = false);
	//events
    virtual void OnEraseBackground(wxEraseEvent& event);
    virtual void OnSize(wxSizeEvent& event);
	virtual void OnCommand(wxCommandEvent& event);
	virtual void OnDropDownCommand(wxCommandEvent& event);
	virtual void OnCommandUI(wxUpdateUIEvent& event);
	virtual void OnRightDown(wxMouseEvent& event);
	virtual void OnAuiRightDown(wxAuiToolBarEvent& event);
	virtual void OnToolDropDown(wxAuiToolBarEvent& event);
	virtual void OnClose(wxCloseEvent & event);
//
protected:
	wxGISAcceleratorTable* m_pGISAcceleratorTable;
    IDropDownCommand* m_pDropDownCommand;
	ITrackCancel* m_pTrackCancel;
    //
    wxFFile m_LogFile;
    wxLocale* m_pLocale; // locale we'll be using
	//char* m_pszOldLocale;
    wxString m_sDecimalPoint;
private:    
    DECLARE_EVENT_TABLE()
};
