/******************************************************************************
 * Project:  wxGIS (GIS Toolbox)
 * Purpose:  remote server header.
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

#include "wxgis/catalog/catalog.h"
#include "wxgis/networking/networking.h"

/** \class INetSearchCallback networking.h
    \brief The callback class to add found servers.
*/
class INetSearchCallback
{
public:
    virtual ~INetSearchCallback(void){};
	//pure virtual
	virtual void AddServer(wxXmlNode* pServerData) = 0;
};

/** \class INetCallback networking.h
    \brief The network callback class.
*/
class INetCallback
{
public:
    virtual ~INetCallback(void){};
	//pure virtual
    virtual void OnConnect(void) = 0;
    virtual void OnDisconnect(void) = 0;
    virtual void PutInMessage(WXGISMSG msg) = 0;
};

/** \class INetClientConnection networking.h
    \brief The network connection interface class.

	This class describes the network connection. Used in server connection list.
*/
class INetClientConnection : public INetConnection
{
public:
    virtual ~INetClientConnection(void){};
	//pure virtual
    /** \fn wxXmlNode* GetAttributes(void)
     *  \brief Get Properties of plugin.
     *  \return The properties of the plugin
	 *
	 *  It should be the new wxXmlNode (not a copy of setted properties)
     */	 	
	virtual wxXmlNode* GetAttributes(void) = 0;
    /** \fn void SetAttributes(wxXmlNode* pProp, INetCallback* pNetCallback)
     *  \brief Set Properties of plugin.
     *  \param pProp The properties of the plugin
     *  \param pNetCallback The callback pointer
	 *
	 *  Executed while LoadChildren (after connection class created). 
     */	  
	virtual bool SetAttributes(const wxXmlNode* pProp) = 0;
	virtual void SetCallback(INetCallback* pNetCallback) = 0;
	virtual wxString GetName(void) = 0;
    virtual wxString GetUser(void) = 0;
    virtual wxString GetCryptPasswd(void) = 0;
};

//typedef std::vector<INetConnection*> NETCONNARRAY;

/** \class INetConnFactory remoteserver.h
    \brief The network connection factory interface class.
*/
class INetConnFactory
{
public:
    virtual ~INetConnFactory(void){};
	//pure virtual
	virtual bool StartServerSearch() = 0;
	virtual bool StopServerSearch() = 0;
	virtual bool CanStopServerSearch() = 0;
	virtual bool IsServerSearching() = 0;
	virtual void SetCallback(INetSearchCallback* pCallback) = 0;
	virtual INetSearchCallback* GetCallback(void) = 0;
	virtual wxString GetName(void) = 0;
    virtual INetClientConnection* GetConnection(wxXmlNode* pProp) = 0;
    /** \fn void Serialize(wxXmlNode* pConfigNode, bool bSave)
     *  \brief Store Properties of Factory.
     *  \param pConfigNode wxXmlNode to store params.
     *  \param bSave Set or Get indicator.
     */	 	
    virtual void Serialize(wxXmlNode* pConfigNode, bool bSave = true) = 0;

	//virtual char GetID(void) = 0;
	//virtual void SetID(char nID) = 0;
};

typedef std::vector<INetConnFactory*> NETCONNFACTORYARRAY;
