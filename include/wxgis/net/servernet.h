/******************************************************************************
 * Project:  wxGIS
 * Purpose:  network server class.
 * Author:   Bishop (aka Barishnikov Dmitriy), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2008-2010  Bishop
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

#include "wxgis/net/netconn.h"
#include "wxgis/net/netevent.h"

/** \class INetworkPlugin servernet.h
    \brief A Server side INetworkPlugin interface.
*/
class WXDLLIMPEXP_GIS_NET INetworkPlugin : public wxEvtHandler
{
    DECLARE_ABSTRACT_CLASS(INetworkPlugin)
public:
    virtual ~INetworkPlugin(void);
    virtual bool Start(INetworkService* pNetService, const wxXmlNode* pConfig) = 0;
    virtual bool Stop(void) = 0;
};

WX_DECLARE_LIST_2(wxGISNetServerConnection, wxGISNetServerConnectionList, wxGISNetServerConnectionNode, class WXDLLIMPEXP_GIS_NET);

/** \class wxGISNetworkService servernet.h
    \brief A Server side Network Service.
*/
class WXDLLIMPEXP_GIS_NET wxGISNetworkService : 
    public wxEvtHandler,
    public INetworkService
{
    DECLARE_CLASS(wxGISNetworkService)
public:
    wxGISNetworkService(INetEventProcessor* pNetEvtProc);
    virtual ~wxGISNetworkService(void);
    ////events
    //void OnSocketEvent(wxSocketEvent& pEvent);
    //INetworkService
    virtual bool Start(void);
    virtual bool Stop(void);    
    virtual wxString GetServerName(void) const;
    virtual bool CanConnect(const wxString &sName, const wxString &sPass);
    virtual void AddConnection(wxGISNetServerConnection* pConn);
    virtual bool DestroyConnection( wxGISNetServerConnection* pConn );
    virtual void RemoveConnection( wxGISNetServerConnection* pConn );
    //wxGISNetworkService
    bool DestroyConnections();
    void SendNetMessage(const wxNetMessage & msg, int nId = wxNOT_FOUND);
    virtual void AddNetworkPlugin(const wxString &sClassName, const wxXmlNode *pConfigNode);    
protected:
    wxGISNetServerConnectionList& GetConnections() { return m_plNetworkConnections; }
    //events
    virtual void OnGisNetEvent(wxGISNetEvent& event);
protected:
    short m_nMaxConnectoinCount;
    int m_nConnectionCounter;
    wxVector<INetworkPlugin*> m_paNetworkPlugins;
    wxString m_sServerName;

    wxGISNetServerConnectionList m_plNetworkConnections;
    int m_nConnectionIdCounter;
    INetEventProcessor* m_pNetEvtProc;
private:
    DECLARE_EVENT_TABLE()
};

/** \fn wxGISNetworkService* const GetNetworkService(void)
 *  \brief Global NetworkService getter.
 */	

WXDLLIMPEXP_GIS_NET wxGISNetworkService* const GetNetworkService(void);

/** \fn void SetNetworkService(wxGISNetworkService* pService)
    \brief Global NetworkService setter.
	\param pService The NetworkService pointer.
 */	

WXDLLIMPEXP_GIS_NET void SetNetworkService(wxGISNetworkService* pService);


