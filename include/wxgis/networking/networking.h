/******************************************************************************
 * Project:  wxGIS (GIS Remote)
 * Purpose:  wxGIS Networking header. Network classes for remote server
 * Author:   Bishop (aka Baryshnikov Dmitriy), polimax@mail.ru
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

#include "wxgis/core/core.h"

#include "wx/socket.h"
#include "wx/xml/xml.h"

#include <queue>

#define WIN 0
#define LIN 1

#ifdef __WXMSW__
	#define CURROS WIN
#else
	#define CURROS LIN
#endif

enum wxGISMessagePriority
{
	enumGISPriorityLowest   = 0, 
	enumGISPriorityLow      = 25,
	enumGISPriorityNormal   = 50,
	enumGISPriorityHigh     = 75,
    enumGISPriorityHighest  = 100
};

enum wxGISMessageDirection
{
    enumGISMsgDirUnk,
	enumGISMsgDirIn, 
	enumGISMsgDirOut
};

enum wxGISMessageState
{
    enumGISMsgStUnk,
	enumGISMsgStOk, 
	enumGISMsgStErr,
    enumGISMsgStRefuse,
    enumGISMsgStNote,
    enumGISMsgStGet,
    enumGISMsgStCmd,
    enumGISMsgStHello,
    enumGISMsgStBye,
	enumGISMsgStAlive,
    enumGISMsgStSnd,
    enumGISMsgStRcv
};

enum wxGISCommandState
{
    enumGISCmdStUnk,
    enumGISCmdStErr,
    enumGISCmdStAdd,
    enumGISCmdStDel,
    enumGISCmdStChng,
    enumGISCmdStStart,
    enumGISCmdStStop
};

enum wxGISUserType
{
    enumGISUserUnk,
    enumGISUserAnon,
    enumGISUserPass
};

#define FILETRANSFERBUFFSIZE 10240
#define MSGBUFFSIZE 15000
#define SIMPLEMSGBUFFSIZE 1024

/** \class INetMessage networking.h
    \brief The network message interface class.
*/
class INetMessage
{
public:
    virtual ~INetMessage(void){};
	//pure virtual
    virtual const short GetPriority(void) = 0;
    virtual void SetPriority(short nPriority) = 0;
    virtual bool IsOk(void) = 0;
    virtual const wxGISMessageDirection GetDirection(void) = 0;
    virtual void SetDirection(wxGISMessageDirection nDirection) = 0;
    virtual const unsigned char* GetData(void) = 0;
    virtual const size_t GetDataLen(void) = 0;
    virtual const wxGISMessageState GetState(void) = 0;
    virtual void SetState(wxGISMessageState nState) = 0;
	virtual void SetDestination(wxString sDst) = 0;
	virtual const wxString GetDestination(void) = 0;
    virtual wxXmlNode* GetRoot(void) = 0;
};

DEFINE_SHARED_PTR(INetMessage);

typedef struct _msg
{
	INetMessageSPtr pMsg;
	long nUserID;
	bool operator< (const struct _msg& x) const { return pMsg->GetPriority() < x.pMsg->GetPriority(); }
} WXGISMSG;

typedef std::priority_queue< WXGISMSG, std::deque<WXGISMSG> > WXGISMSGQUEUE;

/** \class INetConnection networking.h
    \brief The network connection interface class.
*/
class INetConnection
{
public:
    virtual ~INetConnection(void)
    {
        CleanMsgQueueres();
    };
	virtual bool Connect(void) = 0;
	virtual bool Disconnect(void) = 0;
	virtual bool IsConnected(void){return m_bIsConnected;};
	virtual const long GetUserID(void){return m_nUserID;};
	virtual void SetUserID(const long nUserID){m_nUserID = nUserID;};
	//virtual WXGISMSG GetInMessage(void) = 0;
    //{
    //    WXGISMSG Msg = {NULL, -1};
    //    if(m_InMsgQueue.size() > 0)
    //    {
    //        wxCriticalSectionLocker locker(m_CriticalSection);
    //        Msg = m_InMsgQueue.top();
    //        m_InMsgQueue.pop();
    //    }
    //    return Msg;
    //};
	virtual void PutInMessage(WXGISMSG msg) = 0;
    //{
    //    wxCriticalSectionLocker locker(m_CriticalSection);
    //    m_InMsgQueue.push(msg);
    //}
	virtual WXGISMSG GetOutMessage(void)
    {
        WXGISMSG Msg = {INetMessageSPtr(), wxNOT_FOUND};
        wxCriticalSectionLocker locker(m_CriticalSection);
        if(m_OutMsgQueue.size() > 0)
        {
            Msg = m_OutMsgQueue.top();
            m_OutMsgQueue.pop();
        }
        return Msg;
    };
	virtual void PutOutMessage(WXGISMSG msg)
    {
        wxCriticalSectionLocker locker(m_CriticalSection);
        m_OutMsgQueue.push(msg);
    };
    virtual void CleanMsgQueueres(void)
    {
        //clean OutMsgQueue
        wxCriticalSectionLocker locker(m_CriticalSection);
        while( m_OutMsgQueue.size() > 0 )
        {
		    WXGISMSG Msg = m_OutMsgQueue.top();
		    m_OutMsgQueue.pop();  
        }
      //  //clean InMsgQueue
      //  while( m_InMsgQueue.size() > 0 )
      //  {
		    //WXGISMSG Msg = m_InMsgQueue.top();
		    //m_InMsgQueue.pop();  
      //      wxDELETE(Msg.pMsg);
      //  }
    };
protected:
	long m_nUserID;	//user ID for server, and -1 for client	
	WXGISMSGQUEUE m_OutMsgQueue;//, m_InMsgQueuemessages quere
    wxCriticalSection m_CriticalSection;
	bool m_bIsConnected;
};

/** \class INetMessageReceiver networking.h
    \brief A network message class.
*/
class INetMessageReceiver
{
public:
	virtual ~INetMessageReceiver(void){};
    virtual void ProcessMessage(WXGISMSG msg, wxXmlNode* pChildNode) = 0;
};

/** \class INetMessageProcessor networking.h
    \brief A network message class.
*/
class INetMessageProcessor
{
public:
	virtual ~INetMessageProcessor(void){};
    virtual WXGISMSG GetInMessage(void) = 0;
    virtual void ProcessMessage(WXGISMSG msg) = 0;
	virtual void ClearMessageQueue(void) = 0;
	virtual void AddMessageReceiver(wxString sName, INetMessageReceiver* pNetMessageReceiver) = 0;
	virtual void DelMessageReceiver(wxString sName, INetMessageReceiver* pNetMessageReceiver) = 0;
	virtual void ClearMessageReceiver(void) = 0;
};