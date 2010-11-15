/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxDiscConnectionsUI class.
 * Author:   Bishop (aka Barishnikov Dmitriy), polimax@mail.ru
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
#include "wxgis/catalogui/gxdiscconnectionsui.h"
#include "wxgis/catalogui/gxdiscconnectionui.h"
#include "wxgis/catalogui/gxcatalogui.h"

#include "../../art/folder_conns_16.xpm"
#include "../../art/folder_conns_48.xpm"

#include "wx/volume.h"
#include "wx/msgdlg.h"
#include "wx/dir.h"

IMPLEMENT_DYNAMIC_CLASS(wxGxDiscConnectionsUI, wxGxDiscConnections)

wxGxDiscConnectionsUI::wxGxDiscConnectionsUI(void) : wxGxDiscConnections()
{
}

wxGxDiscConnectionsUI::~wxGxDiscConnectionsUI(void)
{
}

wxIcon wxGxDiscConnectionsUI::GetLargeImage(void)
{
	return wxIcon(folder_conns_48_xpm);
}

wxIcon wxGxDiscConnectionsUI::GetSmallImage(void)
{
	return wxIcon(folder_conns_16_xpm);
}

void wxGxDiscConnectionsUI::EmptyChildren(void)
{
    m_aConnections.clear();
	for(size_t i = 0; i < m_Children.size(); i++)
	{
        wxGxDiscConnectionUI* pwxGxDiscConnection = dynamic_cast<wxGxDiscConnectionUI*>(m_Children[i]);
        if(pwxGxDiscConnection)
        {
            CONN_DATA data = {pwxGxDiscConnection->GetName(), pwxGxDiscConnection->GetPath()};
            m_aConnections.push_back(data);
        }
        wxGxCatalogUI* pCatalog = dynamic_cast<wxGxCatalogUI*>(m_pCatalog);
        if(pCatalog)
        {
            IGxSelection* pSel = pCatalog->GetSelection();
            if(pSel)
                pSel->Unselect(m_Children[i], IGxSelection::INIT_ALL);
        }
		m_Children[i]->Detach();
		wxDELETE(m_Children[i]);
	}
	m_Children.clear();
	m_bIsChildrenLoaded = false;
}

void wxGxDiscConnectionsUI::LoadChildren(void)
{
	if(m_bIsChildrenLoaded)
		return;	

    for(size_t i = 0; i < m_aConnections.size(); i++)
    {
        wxGxDiscConnectionUI* pwxGxDiscConnection = new wxGxDiscConnectionUI(m_aConnections[i].sPath, m_aConnections[i].sName);
        IGxObject* pGxObject = static_cast<IGxObject*>(pwxGxDiscConnection);
        if(AddChild(pGxObject))
            wxLogMessage(_("wxGxDiscConnections: Add folder connection [%s]"), m_aConnections[i].sName.c_str());
    }

	m_bIsChildrenLoaded = true;
}

IGxObject* wxGxDiscConnectionsUI::ConnectFolder(wxString sPath)
{
    IGxObject* pReturnObj(NULL);

    for(size_t i = 0; i < m_Children.size(); i++)
    {
        wxGxDiscConnectionUI* pConn = dynamic_cast<wxGxDiscConnectionUI*>(m_Children[i]);
        if(pConn)
            if(pConn->GetPath().CmpNoCase(sPath) == 0)
                return dynamic_cast<IGxObject*>(pConn);
    }

    if(wxDir::Exists(sPath)) 	//check if path is valid
	{
	    wxGxDiscConnectionUI* pwxGxDiscConnection = new wxGxDiscConnectionUI(sPath, sPath);
        IGxObject* pGxObject = static_cast<IGxObject*>(pwxGxDiscConnection);
        if(AddChild(pGxObject))
        {
		    m_pCatalog->ObjectAdded(pGxObject);
            return pGxObject;
        }
        else
		    pReturnObj = this;
    }
    else
    {
		pReturnObj = this;
    }
	return pReturnObj;
}