/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxDiscConnection class.
 * Author:   Baryshnikov Dmitriy (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2009,2010,2012,2013  Bishop
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

#include "wxgis/catalog/gxfolder.h"
#include "wxgis/catalog/gxevent.h"
#include "wxgis/catalog/gxcatalog.h"

#include <wx/event.h>
#include <wx/fswatcher.h>

#if wxVERSION_NUMBER <= 2903// && !defined EVT_FSWATCHER(winid, func)
#define EVT_FSWATCHER(winid, func) \
    wx__DECLARE_EVT1(wxEVT_FSWATCHER, winid, wxFileSystemWatcherEventHandler(func))
#endif

/** \class wxGxDiscConnection gxdiscconnection.h
    \brief A Disc Connection GxObject.
*/

class WXDLLIMPEXP_GIS_CLT wxGxDiscConnection :
	public wxGxFolder
{
    DECLARE_DYNAMIC_CLASS(wxGxDiscConnection)
public:
    wxGxDiscConnection(void);
	wxGxDiscConnection(wxGxObject *oParent, const wxString &soXmlConfPath, int nXmlId, const wxString &soName = wxEmptyString, const CPLString &soPath = "");
	virtual ~wxGxDiscConnection(void);
	//wxGxObject
	virtual wxString GetCategory(void){return wxString(_("Folder connection"));};
    virtual bool Destroy(void);
    virtual void Refresh(void);
	//IGxObjectEdit
	virtual bool Delete(void);
	virtual bool CanDelete(void){return false;};
	virtual bool Rename(const wxString& NewName);
    virtual int GetXmlId(void) const {return m_nXmlId;};
//events
    virtual void OnFileSystemEvent(wxFileSystemWatcherEvent& event);
#ifdef __WXGTK__
    virtual void OnObjectAdded(wxGxCatalogEvent& event);
#endif
protected:
    virtual void StartWatcher(void);
	virtual void LoadChildren(void);
    virtual bool IsPathWatched(const wxString& sPath);
protected:
    int m_nXmlId;
    wxString m_soXmlConfPath;
    wxFileSystemWatcher *m_pWatcher;
    wxGxCatalog* m_pCatalog;
#ifdef __WXGTK__
    long m_ConnectionPointCatalogCookie;
#endif
private:
    DECLARE_EVENT_TABLE()
};
