/******************************************************************************
 * Project:  wxGIS (GIS Remote)
 * Purpose:  wxGxRemoteServer class.
 * Author:   Bishop (aka Barishnikov Dmitriy), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2010 Bishop
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
#include "wxgis/remoteserver/remoteserver.h"

/** \class wxGxRemoteServer gxremoteserver.h
    \brief A Remote Server GxObject.
*/
class WXDLLIMPEXP_GIS_RS wxGxRemoteServer :
	public IGxObjectContainer
{
public:
	wxGxRemoteServer(INetClientConnection* pNetConn);
	virtual ~wxGxRemoteServer(void);
	//IGxObject
	virtual void Detach(void);
	virtual wxString GetName(void);
    virtual wxString GetBaseName(void){return GetName();};
	virtual wxString GetCategory(void){return wxString(_("Remote Server"));};
	virtual void Refresh(void);
	//IGxObjectContainer
	virtual bool DeleteChild(IGxObject* pChild);
	virtual bool AreChildrenViewable(void){return true;};
	virtual bool HasChildren(void){/*LoadChildren();*/ return m_Children.size() > 0 ? true : false;};
    //wxGxRemoteServer
    virtual wxXmlNode* GetProperties(void);
    virtual bool Connect(void);
    virtual bool Disconnect(void);
protected:
    INetClientConnection* m_pNetConn;
	bool m_bIsChildrenLoaded;
};
