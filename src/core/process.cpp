/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  stream common classes.
 * Author:   Baryshnikov Dmitriy (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2010-2012 Bishop
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

#include "wxgis/core/process.h"

#include <wx/txtstrm.h>

#define READ_LINE_DELAY 50
//------------------------------------------------------------------------------
// Class wxGISProcess
//------------------------------------------------------------------------------

wxGISProcess::wxGISProcess(IGISProcessParent* pParent) : wxProcess(wxPROCESS_REDIRECT), wxThreadHelper()
{
    m_pParent = pParent;
}

wxGISProcess::~wxGISProcess(void)
{
}

bool wxGISProcess::Start()
{
	m_pid = Execute();
    if(m_pid == 0)
        return false;

	m_nState = enumGISTaskWork;
	m_dtBeg = wxDateTime::Now();
    m_dfDone = 0;
	//create and start read thread stdin
	if(IsInputOpened())
	{
        return CreateAndRunReadThread();
	}
    
    //start err thread ?
    return true; //or false?
}

void wxGISProcess::OnTerminate(int pid, int status)
{
    DestroyReadThread();
    if(m_nState == enumGISTaskPaused)//process paused
    {
        m_dfDone = 0;
        if(m_pParent)
            m_pParent->OnFinish(this, false);
        return;
    }

    if(m_pParent && m_nState == enumGISTaskWork)
    {
        m_dfDone = 100.0;
        if(m_pParent)
            m_pParent->OnFinish(this, status != 0);
    }

	m_nState = status == 0 ? enumGISTaskDone : enumGISTaskError;
    m_dtEstEnd = wxDateTime::Now();    
}

void wxGISProcess::Stop(void)
{
    if(m_nState == enumGISTaskDone)
        return;

	if(m_nState == enumGISTaskWork)
	{
		////send cancel code
		//wxTextOutputStream OutStr(*GetOutputStream());
		//wxString sStopCmd(wxT("STOP"));
		//OutStr.WriteString(sStopCmd);

	    m_nState = enumGISTaskPaused;

		wxKillError eErr = Kill(m_pid, wxSIGKILL);// wxSIGINT wxSIGTERM
 
        DestroyReadThread();
	   //and detach
		Detach();
	}
	m_nState = enumGISTaskPaused;
    m_dtEstEnd = wxDateTime::Now();
}

void wxGISProcess::UpdatePercent(const wxString &sPercentData)
{
	m_dfDone = wxAtof(sPercentData);
	wxTimeSpan Elapsed = wxDateTime::Now() - m_dtBeg;//time left
    wxString sTxt;

    double nPercentR = 100.0 - m_dfDone;
	if(nPercentR >= 0.0 && m_dfDone > 0.0)
	{
		long dMSec = double(Elapsed.GetMilliseconds().ToDouble() * nPercentR) / m_dfDone;
		wxTimeSpan Remains = wxTimeSpan(0,0,0,dMSec);
		m_dtEstEnd = wxDateTime::Now() + Remains;
	}
}

void wxGISProcess::ProcessInput(wxString &sInputData)
{
	if(m_nState != enumGISTaskWork)
        return;
    sInputData = sInputData.Trim(true).Trim(false);

	wxString sRest;
	if( sInputData.StartsWith(wxT("DONE: "), &sRest) )
	{
        UpdatePercent(sRest.Trim(true).Trim(false).Truncate(sRest.Len() - 1));
		return;
	}
	if( sInputData.StartsWith(wxT("INFO: "), &sRest) )
	{
        AddInfo(enumGISMessageNorm, sRest);
		return;
	}
	if( sInputData.StartsWith(wxT("ERR: "), &sRest) )
	{
        AddInfo(enumGISMessageErr, sRest);
		return;
	}
	if( sInputData.StartsWith(wxT("WARN: "), &sRest) )
	{
        AddInfo(enumGISMessageWarning, sRest);
		return;
	}
	if( sInputData.StartsWith(wxT("SND: "), &sRest) )
	{
        AddInfo(enumGISMessageSend, sRest);
		return;
	}
	if( sInputData.StartsWith(wxT("RCV: "), &sRest) )
	{
        AddInfo(enumGISMessageReceive, sRest);
		return;
	}    
        
    AddInfo(enumGISMessageUnk, sInputData);
}

wxThread::ExitCode wxGISProcess::Entry()
{
    // IMPORTANT:
    // this function gets executed in the secondary thread context!

    // here we do our long task, periodically calling TestDestroy():

    //wxTextInputStream (wxInputStream &stream, const wxString &sep=" \t", const wxMBConv &conv=wxConvAuto())
    wxInputStream &InStream = *GetInputStream();
	wxTextInputStream InputStr(InStream, wxT(" \t"), *wxConvCurrent);
	while(!GetThread()->TestDestroy())
    {
        if(InStream.Eof())
            break;
        if(InStream.IsOk() && InStream.CanRead())
        {
		    wxString line = InputStr.ReadLine();
		    ProcessInput(line);
        }
		wxThread::Sleep(READ_LINE_DELAY);
    }

    // TestDestroy() returned true (which means the main thread asked us
    // to terminate as soon as possible) or we ended the long task...
    return (wxThread::ExitCode)wxTHREAD_NO_ERROR;
}

bool wxGISProcess::CreateAndRunReadThread(void)
{
    if (CreateThread(wxTHREAD_JOINABLE) != wxTHREAD_NO_ERROR)//wxTHREAD_DETACHED//
    {
        wxLogError(_("Could not create the thread!"));
        return false;
    }

    // go!
    if (GetThread()->Run() != wxTHREAD_NO_ERROR)
    {
        wxLogError(_("Could not run the thread!"));
        return false;
    }
    return true;
}

void wxGISProcess::DestroyReadThread(void)
{
    wxCriticalSectionLocker lock(m_ExitLock);
    if (GetThread() && GetThread()->IsRunning())
        GetThread()->Wait();//Delete();//
}

