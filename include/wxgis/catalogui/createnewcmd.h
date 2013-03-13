/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  Catalog Create New Commands class.
 * Author:   Baryshnikov Dmitriy (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2011-2012 Bishop
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
#include "wxgis/framework/applicationbase.h"
#include "wxgis/catalogui/gxapplication.h"

/** \class wxGISCreateNewCmd createnewcmd.h
    \brief The new commands for create various data types.
*/

class wxGISCreateNewCmd :
    public wxGISCommand
{
    DECLARE_DYNAMIC_CLASS(wxGISCatalogMainCmd)

public:
	wxGISCreateNewCmd(void);
	virtual ~wxGISCreateNewCmd(void);
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
private:
	wxGISApplicationBase* m_pApp;
	wxGxApplicationBase* m_pGxApp;
	wxIcon m_IconCreateRemoteConn, m_IconCreateWebConn;
	wxIcon m_LargeConnIcon, m_SmallConnIcon;
	wxIcon m_LargeDisconnIcon, m_SmallDisconnIcon;
    wxIcon m_LargeIconFeatureClass, m_SmallIconFeatureClass;
    wxIcon m_LargeIconTable, m_SmallIconTable;
};
