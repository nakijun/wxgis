/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  framework header.
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

#include "wxgis/core/app.h"

#include <wx/dnd.h>
#include <wx/xml/xml.h>

/** \class IView framework.h
    \brief A View interface class.

    Any additional view mast derived from this class. 
*/
class IView
{
public:
	virtual ~IView(void){};
    virtual bool Create(wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxTAB_TRAVERSAL, const wxString& name = wxT("view")) = 0;
	virtual bool Activate(IApplication* const pApplication, wxXmlNode* const pConf) = 0;
	virtual void Deactivate(void) = 0;
	virtual void Refresh(void) = 0;
	virtual wxString GetViewName(void) const = 0;
	virtual wxIcon GetViewIcon(void) = 0;
	virtual void SetViewIcon(wxIcon Icon) = 0;
};

/** \class IViewDropTarget framework.h
    \brief A DropTarget interface class.

    The Views which should support drag'n'drop capability mast derived from this class.
*/
class IViewDropTarget
{
public:
	virtual ~IViewDropTarget(void){};
    virtual wxDragResult OnDragOver(wxCoord x, wxCoord y, wxDragResult def) = 0;
    virtual bool OnDropObjects(wxCoord x, wxCoord y, const wxArrayString& GxObjects) = 0;
    virtual void OnLeave() = 0;
    virtual bool CanPaste(void) {return false;};
};

