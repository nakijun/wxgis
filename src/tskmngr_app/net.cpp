/******************************************************************************
 * Project:  wxGIS (Task Manager)
 * Purpose:  Network classes.
 * Author:   Bishop (aka Barishnikov Dmitriy), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2012 Bishop
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

#include "wxgis/tskmngr_app/net.h"
#include "wxgis/core/config.h"

//----------------------------------------------------------------------------
// wxGISNetworkService
//----------------------------------------------------------------------------
IMPLEMENT_CLASS(wxGISLocalNetworkService, wxGISNetworkService)

wxGISLocalNetworkService::wxGISLocalNetworkService(INetEventProcessor* pNetEvtProc) : wxGISNetworkService(pNetEvtProc)
{
}

wxGISLocalNetworkService::~wxGISLocalNetworkService()
{
}

bool wxGISLocalNetworkService::Start()
{
    INetworkPlugin *pPlugin = new wxGISLocalNetworkPlugin();
	if(pPlugin && pPlugin->Start(this, NULL))
	{
		m_paNetworkPlugins.push_back( pPlugin );
	}
	else
		wxLogError(_("wxGISLocalNetworkService: Error initializing plugin wxGISLocalNetworkPlugin"));

    wxLogVerbose(_("wxGISLocalNetworkService: Service started"));

    return true;
}

wxString wxGISLocalNetworkService::GetServerName(void) const
{
    return wxString(wxT("wxGIS Task Manager"));
}

bool wxGISLocalNetworkService::CanConnect(const wxString &sName, const wxString &sPass)
{
    return true;
}


//----------------------------------------------------------------------------
// wxGISLocalNetworkPlugin
//----------------------------------------------------------------------------

BEGIN_EVENT_TABLE(wxGISLocalNetworkPlugin, wxEvtHandler)
  EVT_SOCKET(TCP_SERVER_ID,  wxGISLocalNetworkPlugin::OnTCPServerEvent)
END_EVENT_TABLE()

IMPLEMENT_CLASS(wxGISLocalNetworkPlugin, wxEvtHandler)

wxGISLocalNetworkPlugin::wxGISLocalNetworkPlugin(void) : m_nPort(9980)
{
}

wxGISLocalNetworkPlugin::~wxGISLocalNetworkPlugin(void)
{
}

bool wxGISLocalNetworkPlugin::Start(INetworkService* pNetService, const wxXmlNode* pConfig)
{
    m_pNetService = pNetService;
    if(pConfig)
    {
        m_sAddr = pConfig->GetAttribute(wxT("addr"), wxT("127.0.0.1"));
	    m_nPort = wxAtoi(pConfig->GetAttribute(wxT("port"), wxT("9980")));
    }
    else
    {
        wxGISAppConfig oConfig = GetConfig();
        if(oConfig.IsOk())
        {
            m_sAddr = oConfig.Read(enumGISHKCU, wxString(wxT("wxGISCommon/tasks/host")), wxT("127.0.0.1"));
            m_nPort = oConfig.ReadInt(enumGISHKCU, wxString(wxT("wxGISCommon/tasks/port")), 9980);
        }
        else
            m_sAddr = wxString(wxT("127.0.0.1"));
    }

	wxLogMessage(_("%s: Plugin is started..."), wxT("wxGISLocalNetworkPlugin"));

    return CreateListenSocket();
}

bool wxGISLocalNetworkPlugin::Stop(void)
{

	wxLogMessage(_("%s: Plugin is shutdown..."), wxT("wxGISLocalNetworkPlugin"));

    if(m_listeningSocket)
    {
        m_listeningSocket->Destroy();
        m_listeningSocket = NULL;
    }	
    return true;
}

void wxGISLocalNetworkPlugin::OnTCPServerEvent(wxSocketEvent& event)
{
    switch(event.GetSocketEvent())
    {
        case wxSOCKET_INPUT:
            wxLogError(_("wxGISLocalNetworkPlugin: Unexpected wxSOCKET_INPUT in wxSocketServer"));
            break;
        case wxSOCKET_OUTPUT:
            wxLogError(_("wxGISLocalNetworkPlugin: Unexpected wxSOCKET_OUTPUT in wxSocketServer"));
        break;
        case wxSOCKET_CONNECTION:
        {
            // Accept new connection if there is one in the pending
            // connections queue, else exit. We use Accept(false) for
            // non-blocking accept (although if we got here, there
            // should ALWAYS be a pending connection).

            wxSocketBase* sock = m_listeningSocket->Accept(false);
            IPaddress addr;
            if (!sock->GetPeer(addr))
            {
                wxLogError(_("wxGISLocalNetworkPlugin: cannot get peer info"));
            } 
            else 
            {
                wxLogMessage(_("wxGISLocalNetworkPlugin: Got connection from %s:%d"), addr.IPAddress().c_str(), addr.Service());
            }
            
            wxGISLocalServerConnection* pSrvConn = new wxGISLocalServerConnection(sock);
            m_pNetService->AddConnection(pSrvConn);
        }
        break;
        case wxSOCKET_LOST:
            wxLogError(_("wxGISLocalNetworkPlugin: Unexpected wxSOCKET_LOST in wxSocketServer"));
        break;
    }
}

bool wxGISLocalNetworkPlugin::CreateListenSocket(void)
{
	IPaddress LocalAddress; // For the listening 
	LocalAddress.Service(m_nPort); // port on which we listen for clients

	bool bIsAddrSet = false;
	if(m_sAddr.IsEmpty())
		bIsAddrSet = LocalAddress.AnyAddress(); 
	else
		bIsAddrSet = LocalAddress.Hostname(m_sAddr);
	if(!bIsAddrSet)
	{
		wxLogError(_("wxGISLocalNetworkPlugin: Invalid address - %s"), m_sAddr.c_str());
		return false;
	}

	m_listeningSocket = new wxSocketServer(LocalAddress, wxSOCKET_WAITALL|wxSOCKET_REUSEADDR);
    m_listeningSocket->SetEventHandler(*this, TCP_SERVER_ID);
    m_listeningSocket->SetNotify(wxSOCKET_CONNECTION_FLAG);
    m_listeningSocket->Notify(true);
    if (!m_listeningSocket->IsOk())
    {
		wxLogError(_("wxGISLocalNetworkPlugin: Could not listen at the specified port! Port number - %d"), m_nPort);
        //wxLogError(wxString(_("Cannot bind listening socket")));
        return false;
    }

    wxLogMessage(_("Server listening at port %d, waiting for connections"), m_nPort);

    return true;
}


//----------------------------------------------------------------------------
// wxGISNetServerConnection
//----------------------------------------------------------------------------

IMPLEMENT_CLASS(wxGISLocalServerConnection, INetConnection)

wxGISLocalServerConnection::wxGISLocalServerConnection(void) : wxGISNetServerConnection()
{
}

wxGISLocalServerConnection::wxGISLocalServerConnection(wxSocketBase* sock) : wxGISNetServerConnection(sock)
{
}

void wxGISLocalServerConnection::SetSocket(wxSocketBase* sock)
{
    m_bIsConnected = true;
    m_pSock = sock;
    m_pSock->SetEventHandler(*this, SOCKET_ID);
    m_pSock->SetNotify( wxSOCKET_LOST_FLAG );
    m_pSock->Notify(true);
    m_pSock->SetFlags(wxSOCKET_WAITALL | wxSOCKET_BLOCK);

    if (!m_pSock->IsOk())
    {
		wxLogError(_("Cannot bind listening socket"));
    }
    else
    {
        CreateAndRunThreads();
    }
}

wxGISLocalServerConnection::~wxGISLocalServerConnection(void)
{
    DestroyThreads();

    if(m_pNetService)
        m_pNetService->RemoveConnection(this);
}

void wxGISLocalServerConnection::ProcessInputNetMessage(unsigned char * const buff)
{
    if(!m_pSock || m_pSock->IsDisconnected())
        return;
    if(m_pSock->WaitForRead(WAITFOR))
    {
        size_t nSize = m_pSock->ReadMsg(buff, BUFFSIZE).LastCount();
        if ( !nSize )
        {
            wxLogError(wxString::Format(_("Read error (%d): %s"), m_pSock->LastError(), GetSocketErrorMsg(m_pSock->LastError())));
            return;
        }

 		wxNetMessage msg((const char*)buff, nSize);
        if(!msg.IsOk())
        {
            wxLogError(_("Invalid input message"));
            return;
        }

        wxGISNetEvent event(m_nUserId, wxGISNET_MSG, msg);
        PostEvent(event);

    }
}

void wxGISLocalServerConnection::OnSocketEvent(wxSocketEvent& event)
{
    switch(event.GetSocketEvent())
    {
        case wxSOCKET_INPUT:
            //send event to advisers
            wxLogDebug(wxT("wxGISNetServerConnection: INPUT"));
            break;
        case wxSOCKET_OUTPUT:            
            wxLogDebug(wxT("wxGISNetServerConnection: OUTPUT"));
            //ProcessNetMessage();
            break;
        case wxSOCKET_CONNECTION:
            wxLogDebug(wxT("wxGISNetServerConnection: CONNECTION"));
            break;
        case wxSOCKET_LOST:
            wxLogDebug(wxT("wxGISNetServerConnection: LOST"));
            {
                IPaddress addr;
                if (!m_pSock->GetPeer(addr))
                {
                    wxLogMessage(_("User #%d is disconnected"), m_nUserId);
                } 
                else 
                {
                    wxLogMessage(_("User #%d from %s:%d is disconnected"), m_nUserId, addr.IPAddress().c_str(), addr.Service());
                }

                m_bIsConnected = false;
                //send bye to the app
                wxNetMessage msgin(enumGISNetCmdBye, enumGISNetCmdStUnk, enumGISPriorityHighest);
                wxGISNetEvent event(m_nUserId, wxGISNET_MSG, msgin);
                PostEvent(event);

                //as connection is lost we destroy itself
                Destroy();
            }
            break;
        default:
            break;
    }
}

void wxGISLocalServerConnection::OnTimer( wxTimerEvent & event)
{
}