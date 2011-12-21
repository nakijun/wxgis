/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGISNewMenu class.
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
#include "wxgis/catalogui/gxcatalogui.h"
#include "wxgis/framework/framework.h"
#include "wxgis/framework/commandbar.h"

#define NEWMENUNAME wxT("Application.NewMenu")

//----------------------------------------------------------------------
// wxGISNewMenu
//----------------------------------------------------------------------

class WXDLLIMPEXP_GIS_CLU wxGISNewMenu :
	public wxGISMenu,
	public ICommand
{
public:
	wxGISNewMenu(const wxString& sName = NEWMENUNAME, const wxString& sCaption = _("New"), wxGISEnumCommandBars type = enumGISCBSubMenu, const wxString& title = wxEmptyString, long style = 0);
	~wxGISNewMenu(void);
	//wxGISMenu
	virtual void AddCommand(ICommand* pCmd);
	//virtual void RemoveCommand(size_t nIndex){};
	virtual void MoveCommandLeft(size_t nIndex){};
	virtual void MoveCommandRight(size_t nIndex){};
	//ICommand
	virtual wxIcon GetBitmap(void);
	virtual wxString GetCaption(void);
	virtual wxString GetCategory(void);
	virtual bool GetChecked(void);
	virtual bool GetEnabled(void);
	virtual wxString GetMessage(void);
	virtual wxGISEnumCommandKind GetKind(void);
	virtual void OnClick(void);
	virtual bool OnCreate(IFrameApplication* pApp);
	virtual wxString GetTooltip(void);
	virtual unsigned char GetCount(void);
    //wxGISMenu
	virtual void Update(IGxSelection* Selection);
protected:
	IFrameApplication* m_pApp;
	wxGxCatalogUI* m_pCatalog;
	//wxGISConnectionPointContainer* m_pConnectionPointSelection;
	//long m_ConnectionPointSelectionCookie;
};


