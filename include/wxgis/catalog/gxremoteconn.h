/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxRemoteConnection class.
 * Author:   Baryshnikov Dmitriy (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2011,2013 Bishop
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

#include "wxgis/defs.h"

#ifdef wxGIS_USE_POSTGRES

#include "wxgis/catalog/gxobject.h"
#include "wxgis/catalog/gxdataset.h"
#include "wxgis/datasource/postgisdataset.h"

class wxGxRemoteDBSchema;

/** \class wxGxRemoteConnection gxremoteconn.h
    \brief A Remote Connection GxObject.
*/

class WXDLLIMPEXP_GIS_CLT wxGxRemoteConnection :
	public wxGxObjectContainer,
    public IGxObjectEdit,
    public IGxRemoteConnection
{
    DECLARE_CLASS(wxGxRemoteConnection)
    typedef struct _pgtabledata{
        bool bHasGeometry;
        CPLString sTableName;
        CPLString sTableSchema;
    }PGTABLEDATA;
public:
	wxGxRemoteConnection(wxGxObject *oParent, const wxString &soName = wxEmptyString, const CPLString &soPath = "");
	virtual ~wxGxRemoteConnection(void);
	//wxGxObject
	virtual wxString GetCategory(void) const {return wxString(_("Database Connection"));};
    virtual void Refresh(void);
    //IGxRemoteConnection
	virtual bool Connect(void);
	virtual bool Disconnect(void);
    virtual bool IsConnected(void);
	//IGxObjectEdit
	virtual bool Delete(void);
	virtual bool CanDelete(void){return true;};
	virtual bool Rename(const wxString& NewName);
	virtual bool CanRename(void){return true;};
	virtual bool Copy(const CPLString &szDestPath, ITrackCancel* const pTrackCancel);
	virtual bool CanCopy(const CPLString &szDestPath){return true;};
	virtual bool Move(const CPLString &szDestPath, ITrackCancel* const pTrackCancel);
	virtual bool CanMove(const CPLString &szDestPath){return CanCopy(szDestPath) & CanDelete();};
	//wxGxObjectContainer
	virtual bool AreChildrenViewable(void) const {return true;};
	virtual bool HasChildren(void);
protected:
	//wxGxRemoteConnection
	virtual void LoadChildren(void);
    virtual wxGxRemoteDBSchema* GetNewRemoteDBSchema(const wxString &sName, const wxArrayString &saTables, bool bHasGeom, bool bHasGeog, bool bHasRaster, wxGISPostgresDataSource *pwxGISRemoteConn);
    //create wxGISDataset without openning it
    virtual wxGISDataset* const GetDatasetFast(void);
protected:
    wxGISDataset* m_pwxGISDataset;
    bool m_bIsConnected;
};

/** \class wxGxRemoteDBSchema gxremoteconn.h
    \brief A Remote Database schema GxObject.
*/

class WXDLLIMPEXP_GIS_CLT wxGxRemoteDBSchema :	
	public wxGxObjectContainer,
    public IGxObjectEdit
{
    DECLARE_CLASS(wxGxRemoteDBSchema)
public:
	wxGxRemoteDBSchema(const wxArrayString &saTables, bool bHasGeom, bool bHasGeog, bool bHasRaster, wxGISPostgresDataSource* pwxGISRemoteConn, wxGxObject *oParent, const wxString &soName = wxEmptyString, const CPLString &soPath = "");
	virtual ~wxGxRemoteDBSchema(void);
	//wxGxObject
	virtual wxString GetCategory(void){return wxString(_("Remote Database Schema"));};
    virtual void Refresh(void);
	//wxGxObjectContainer
	virtual bool AreChildrenViewable(void) const {return true;};
	virtual bool HasChildren(void);
    //IGxObjectEdit
	virtual bool Delete(void);
	virtual bool CanDelete(void){return false;};//TODO: depend on user permissions
	virtual bool Rename(const wxString& NewName);
	virtual bool CanRename(void){return false;};//TODO: depend on user permissions
	virtual bool Copy(const CPLString &szDestPath, ITrackCancel* const pTrackCancel){return false;};
    //TODO: check posibility to release copy and move db schema
	virtual bool CanCopy(const CPLString &szDestPath){return false;};//The table schema cannot be copied 
	virtual bool Move(const CPLString &szDestPath, ITrackCancel* const pTrackCancel){return false;};
	virtual bool CanMove(const CPLString &szDestPath){return false;};//The table schema cannot be moved

protected:
    //wxGxRemoteDBSchema
    virtual void LoadChildren(void);
    virtual void AddTable(const wxString &sTableName, const wxGISEnumDatasetType eType);
protected:
    wxGISPostgresDataSource* m_pwxGISRemoteConn;
    bool m_bChildrenLoaded;
    wxCriticalSection m_CritSect;
    wxArrayString m_saTables;
    bool m_bHasGeom, m_bHasGeog, m_bHasRaster;
};

#endif //wxGIS_USE_POSTGRES