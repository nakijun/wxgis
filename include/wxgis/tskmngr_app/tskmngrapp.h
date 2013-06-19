/******************************************************************************
 * Project:  wxGIS (Task Manager)
 * Purpose:  Task manager application class.
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

#pragma once

#include "wxgis/base.h"
#include "wxgis/core/config.h"
#include "wxgis/core/init.h"
#include "wxgis/tskmngr_app/tskmngr.h"
#include "wxgis/version.h"

#include <wx/app.h>
#include <wx/snglinst.h>
#include <wx/cmdline.h> 

/** \class wxGISTaskManagerApp tskmngrapp.h
    \brief Main task manager application.

    This is an singleton application which manage tasks (geoprocessing ao something else). Each application or different instances of the same application create, change, delete, start, stop, pause their tasks via this application. The application instance should try to start task manager application and connect to it via tcp ethernet protocol. While exiting - send exit command to task manager. If there are no any other connections, task manager should exit. In standalone mode task manager can be run as a service to execute by timer.
*/
class wxGISTaskManagerApp :
	public wxAppConsole,
    public wxThreadHelper,
    public wxGISInitializer
{
public:
	wxGISTaskManagerApp(void);
	virtual ~wxGISTaskManagerApp(void);
    //wxAppConsole
    virtual bool OnInit();
    virtual int OnExit();
    void OnInitCmdLine(wxCmdLineParser& pParser);
    bool OnCmdLineParsed(wxCmdLineParser& pParser);
    //wxGISInitializer
	virtual bool Initialize(const wxString &sAppName, const wxString &sLogFilePrefix);//, wxCmdLineParser& parser
    // IApplication
    virtual bool SetupSys(const wxString &sSysPath);
    virtual wxString GetAppName(void) const {return m_appName;};
	virtual wxString GetUserAppName(void) const{return m_appDisplayName;};
    virtual wxString GetUserAppNameShort(void) const {return wxString(_("Task Manager"));};
    virtual wxString GetAppVersionString(void) const {return wxString(wxGIS_VERSION_NUM_DOT_STRING_T);};
    virtual void OnAppAbout(void);
    virtual void OnAppOptions(void);
protected:
    virtual wxThread::ExitCode Entry();
    bool CreateAndRunExitThread(void);
    void DestroyExitThread(void);
private:
    wxGISAppConfig m_oConfig;
#ifdef wxUSE_SNGLINST_CHECKER
    wxSingleInstanceChecker *m_pChecker;
#endif    
    wxCriticalSection m_ExitLock;
    wxGISTaskManager* m_pTaskManager;
};

DECLARE_APP(wxGISTaskManagerApp)


