/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  Catalog Main Commands class.
 * Author:   Baryshnikov Dmitriy (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2009-2013 Bishop
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

#include "wxgis/framework/command.h"
#include "wxgis/catalogui/gxapplication.h"

/** \class wxGISCatalogMainCmd catalogcmd.h
    \brief The main catalog commands.

	The main catalog commands includes: "Up One Level", "Connect Folder", "Disconnect Folder", "Location", "Delete Item", "Back", "Forward", "Create Folder", "Rename", "Refresh", "Properties", "Copy", "Cut" and "Paste".
*/

class wxGISCatalogMainCmd :
    public wxGISCommand,
	public IToolControl,
    public IDropDownCommand
{
    DECLARE_DYNAMIC_CLASS(wxGISCatalogMainCmd)

public:
	wxGISCatalogMainCmd(void);
	virtual ~wxGISCatalogMainCmd(void);
	//wxGISCommand
	virtual wxIcon GetBitmap(void);
	virtual wxString GetCaption(void);
	virtual wxString GetCategory(void);
	virtual bool GetChecked(void);
	virtual bool GetEnabled(void);
	virtual wxString GetMessage(void);
	virtual wxGISEnumCommandKind GetKind(void);
	virtual void OnClick(void);
	virtual bool OnCreate(wxGISApplicationBase* pApp);
	virtual wxString GetTooltip(void);
	virtual unsigned char GetCount(void);
	//IToolControl
	virtual IToolBarControl* GetControl(void);
	virtual wxString GetToolLabel(void);
	virtual bool HasToolLabel(void);
    //IDropDownCommand
   	virtual wxMenu* GetDropDownMenu(void);
    virtual void OnDropDownCommand(int nID);
private:
	wxGISApplicationBase* m_pApp;
	wxGxApplicationBase* m_pGxApp;
	wxIcon m_IconFolderUp, m_IconFolderConn, m_IconFolderConnDel, m_IconDel, m_IconGoPrev, m_IconGoNext;
    wxIcon m_IconFolderNew, m_IconEdit, m_IconViewRefresh, m_IconProps;
    wxIcon m_LargeFolderIcon, m_SmallFolderIcon;
    wxIcon m_CopyIcon, m_CutIcon, m_PasteIcon;
    short m_nPrevNextSelCount;
};
