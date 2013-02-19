/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxView class.
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
#include "wxgis/catalogui/gxselection.h"
#include "wxgis/catalog/catalog.h"
#include "wxgis/catalog/gxobject.h"

/** \class wxGxView gxview.h
 *  \brief The base class for views.
 */

class WXDLLIMPEXP_GIS_CLU wxGxView :
    public IView
{
public:	
	wxGxView(void);
	virtual ~wxGxView(void);
    //IView
    virtual bool Activate(IApplication* const pApplication, wxXmlNode* const pConf);
	virtual void Deactivate(void);
	virtual wxString GetViewName(void) const;
	virtual wxIcon GetViewIcon(void);
	virtual void SetViewIcon(wxIcon Icon);
    virtual void Refresh(void){};    
    //wxGxView
	virtual bool Applies(wxGxSelection* const Selection);
    virtual void BeginRename(long nObjectID = wxNOT_FOUND){};
protected:
	wxString m_sViewName;
	wxXmlNode* m_pXmlConf;
    wxIcon m_Icon;
};

int GxObjectCompareFunction(wxGxObject* const pObject1, wxGxObject* const pObject2, long sortData);
