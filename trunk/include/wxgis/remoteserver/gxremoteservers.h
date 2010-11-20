/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxRemoteServers class.
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

/** \class wxGxRemoteServers gxremoteservers.h
    \brief A Remote Servers GxRootObject.
*/
class WXDLLIMPEXP_GIS_RS wxGxRemoteServers :
	public IGxObjectContainer,
    public IGxRootObjectProperties,
    public wxObject
{
   DECLARE_DYNAMIC_CLASS(wxGxRemoteServers)
public:
	wxGxRemoteServers(void);
	virtual ~wxGxRemoteServers(void);
	//IGxObject
	virtual void Detach(void);
	virtual wxString GetName(void){return wxString(_("Remote Servers"));};
    virtual wxString GetBaseName(void){return GetName();};
    virtual wxString GetFullName(void){return wxEmptyString;};
    virtual wxString GetPath(void){return wxEmptyString;};
	virtual wxString GetCategory(void){return wxString(_("Remote Servers"));};
	virtual void Refresh(void);
	//IGxObjectContainer
	virtual bool DeleteChild(IGxObject* pChild);
	virtual bool AreChildrenViewable(void){return true;};
	virtual bool HasChildren(void){LoadChildren(); return m_Children.size() > 0 ? true : false;};
    //IGxRootObjectProperties
    virtual void Init(wxXmlNode* pConfigNode);
    virtual wxXmlNode* GetProperties(void);
	//wxGxRemoteServers
	virtual void LoadChildren(void);
	virtual void EmptyChildren(void);

    //typedef struct _conn_data{
    //    wxString sName, sPath;
    //}CONN_DATA;
protected:
    //std::vector<CONN_DATA> m_aConnections;
	bool m_bIsChildrenLoaded;
};