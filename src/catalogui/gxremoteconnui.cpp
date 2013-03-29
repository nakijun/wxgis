/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  Remote Connection UI classes.
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

#include "wxgis/catalogui/gxremoteconnui.h"

#ifdef wxGIS_USE_POSTGRES

#include "wxgis/catalogui/remoteconndlg.h"
#include "wxgis/catalogui/gxpostgisdatasetui.h"
#include "wxgis/catalogui/gxcatalogui.h"

#include "../../art/pg_vec_16.xpm"
#include "../../art/pg_vec_48.xpm"
#include "../../art/table_pg_16.xpm"
#include "../../art/table_pg_48.xpm"
#include "../../art/dbschema_16.xpm"
#include "../../art/dbschema_48.xpm"

//propertypages
#include "wxgis/catalogui/spatrefpropertypage.h"
#include "wxgis/catalogui/rasterpropertypage.h"
#include "wxgis/catalogui/vectorpropertypage.h"
#include "wxgis/catalogui/tablepropertypage.h"

#include "../../art/properties.xpm"

#include "wx/busyinfo.h"
#include "wx/utils.h"
#include "wx/propdlg.h"
#include "wx/bookctrl.h"

//--------------------------------------------------------------
//class wxGxRemoteConnectionUI
//--------------------------------------------------------------

IMPLEMENT_CLASS(wxGxRemoteConnectionUI, wxGxRemoteConnection)

BEGIN_EVENT_TABLE(wxGxRemoteConnectionUI, wxGxRemoteConnection)
    EVT_THREAD(EXIT_EVENT, wxGxRemoteConnectionUI::OnThreadFinished)
END_EVENT_TABLE()

wxGxRemoteConnectionUI::wxGxRemoteConnectionUI(wxGxObject *oParent, const wxString &soName, const CPLString &soPath, const wxIcon &LargeIconConn, const wxIcon &SmallIconConn, const wxIcon &LargeIconDisconn, const wxIcon &SmallIconDisconn) : wxGxRemoteConnection(oParent, soName, soPath)
{
    m_oLargeIconConn = LargeIconConn;
    m_oSmallIconConn = SmallIconConn;
    m_oLargeIconDisconn = LargeIconDisconn;
    m_oSmallIconDisconn = SmallIconDisconn;

    m_PendingId = wxNOT_FOUND;
}

wxGxRemoteConnectionUI::~wxGxRemoteConnectionUI(void)
{
}

wxIcon wxGxRemoteConnectionUI::GetLargeImage(void)
{
    if(m_pwxGISDataset && m_pwxGISDataset->IsOpened())
        return m_oLargeIconConn;
    else
        return m_oLargeIconDisconn;
}

wxIcon wxGxRemoteConnectionUI::GetSmallImage(void)
{
    if(m_pwxGISDataset && m_pwxGISDataset->IsOpened())
        return m_oSmallIconConn;
    else
        return m_oSmallIconDisconn;
}

void wxGxRemoteConnectionUI::EditProperties(wxWindow *parent)
{
	wxGISRemoteConnDlg dlg(m_sPath, parent);
	if(dlg.ShowModal() == wxID_OK)
	{
        Disconnect();
	}
}

bool wxGxRemoteConnectionUI::Invoke(wxWindow* pParentWnd)
{
    wxBusyCursor wait;
    //connect
	if(!Connect())
	{
		wxMessageBox(_("Connect failed!"), _("Error"), wxICON_ERROR | wxOK);
		return false;
	}

    return true;
}

bool wxGxRemoteConnectionUI::Connect(void)
{
    if(IsConnected())
        return true;
    bool bRes = true;
    //add pending item
    wxGxCatalogUI* pCat = wxDynamicCast(GetGxCatalog(), wxGxCatalogUI);
    if(pCat && m_PendingId == wxNOT_FOUND)
    {
        m_PendingId = pCat->AddPending(GetId());
        //pCat->ObjectRefreshed(GetId());
    }
    //start thread to load schemes
    if(!CreateAndRunCheckThread())
        return false;
    return bRes;
}

bool wxGxRemoteConnectionUI::CreateAndRunCheckThread(void)
{
    if(!GetThread())
    {
        if (CreateThread(wxTHREAD_JOINABLE) != wxTHREAD_NO_ERROR)
        {
            wxLogError(_("Could not create the thread!"));
            return false;
        }
    }

    if(GetThread()->IsRunning())
        return true;

    if (GetThread()->Run() != wxTHREAD_NO_ERROR)
    {
        wxLogError(_("Could not run the thread!"));
        return false;
    }
    return true;
}

//thread to load remote DB tables
//before exit we assume that no tables exist
wxThread::ExitCode wxGxRemoteConnectionUI::Entry()
{
    wxGISPostgresDataSource* pDSet = wxDynamicCast(GetDatasetFast(), wxGISPostgresDataSource);
    if(pDSet)
    {        
        if(!pDSet->Open())
        {
            wxThreadEvent event( wxEVT_THREAD, EXIT_EVENT );
            wxQueueEvent( this, event.Clone() );
            return (wxThread::ExitCode)1;
        }
    }
    wsDELETE(pDSet);

    LoadChildren();

    wxThreadEvent event( wxEVT_THREAD, EXIT_EVENT );
    wxQueueEvent( this, event.Clone() );

    return (wxThread::ExitCode)0;
}

void wxGxRemoteConnectionUI::OnThreadFinished(wxThreadEvent& event)
{
    wxGxCatalogUI* pCat = wxDynamicCast(GetGxCatalog(), wxGxCatalogUI);
    if(pCat)
    {
        pCat->RemovePending(m_PendingId);
        m_PendingId = wxNOT_FOUND;
        pCat->ObjectRefreshed(GetId());
        pCat->ObjectChanged(GetId());
    }
}

wxGxRemoteDBSchema* wxGxRemoteConnectionUI::GetNewRemoteDBSchema(const wxString &sName, const wxArrayString &saTables, bool bHasGeom, bool bHasGeog, bool bHasRaster, wxGISPostgresDataSource *pwxGISRemoteConn)
{
    if(!m_oLargeIconFeatureClass.IsOk())
        m_oLargeIconFeatureClass = wxIcon(pg_vec_48_xpm);
    if(!m_oSmallIconFeatureClass.IsOk())
        m_oSmallIconFeatureClass = wxIcon(pg_vec_16_xpm);
    if(!m_oLargeIconTable.IsOk())
        m_oLargeIconTable = wxIcon(table_pg_48_xpm);
    if(!m_oSmallIconTable.IsOk())
        m_oSmallIconTable = wxIcon(table_pg_16_xpm);
    if(!m_oLargeIconSchema.IsOk())
        m_oLargeIconSchema = wxIcon(dbschema_48_xpm);
    if(!m_oSmallIconSchema.IsOk())
        m_oSmallIconSchema = wxIcon(dbschema_16_xpm);
    return wxStaticCast(new wxGxRemoteDBSchemaUI(saTables, bHasGeom, bHasGeog, bHasRaster, pwxGISRemoteConn, this, sName, "", m_oLargeIconSchema, m_oSmallIconSchema, m_oLargeIconFeatureClass, m_oSmallIconFeatureClass, m_oLargeIconTable, m_oSmallIconTable), wxGxRemoteDBSchema);
}

//--------------------------------------------------------------
//class wxGxRemoteDBSchemaUI
//--------------------------------------------------------------

IMPLEMENT_CLASS(wxGxRemoteDBSchemaUI, wxGxRemoteDBSchema)

BEGIN_EVENT_TABLE(wxGxRemoteDBSchemaUI, wxGxRemoteDBSchema)
    EVT_THREAD(EXIT_EVENT, wxGxRemoteDBSchemaUI::OnThreadFinished)
END_EVENT_TABLE()

wxGxRemoteDBSchemaUI::wxGxRemoteDBSchemaUI(const wxArrayString &saTables, bool bHasGeom, bool bHasGeog, bool bHasRaster, wxGISPostgresDataSource* pwxGISRemoteConn, wxGxObject *oParent, const wxString &soName, const CPLString &soPath, const wxIcon &LargeIcon, const wxIcon &SmallIcon, const wxIcon &LargeIconFeatureClass, const wxIcon &SmallIconFeatureClass, const wxIcon &LargeIconTable, const wxIcon &SmallIconTable) : wxGxRemoteDBSchema(saTables, bHasGeom, bHasGeog, bHasRaster, pwxGISRemoteConn, oParent, soName, soPath)
{
    m_oLargeIcon = LargeIcon;
    m_oSmallIcon = SmallIcon;
    m_oLargeIconFeatureClass = LargeIconFeatureClass;
    m_oSmallIconFeatureClass = SmallIconFeatureClass;
    m_oLargeIconTable = LargeIconTable;
    m_oSmallIconTable = SmallIconTable;
    
    m_PendingId = wxNOT_FOUND;
}

wxGxRemoteDBSchemaUI::~wxGxRemoteDBSchemaUI(void)
{
}

void wxGxRemoteDBSchemaUI::EditProperties(wxWindow *parent)
{
    //TODO: change permissions and properties
}

wxIcon wxGxRemoteDBSchemaUI::GetLargeImage(void)
{
    return m_oLargeIcon;
}

wxIcon wxGxRemoteDBSchemaUI::GetSmallImage(void)
{
    return m_oSmallIcon;
}

bool wxGxRemoteDBSchemaUI::HasChildren(void)
{
    if(m_bChildrenLoaded)
        return wxGxObjectContainer::HasChildren(); 
    if(GetThread() && GetThread()->IsRunning())
        return wxGxObjectContainer::HasChildren(); 

    wxGxCatalogUI* pCat = wxDynamicCast(GetGxCatalog(), wxGxCatalogUI);
    if(pCat && m_PendingId == wxNOT_FOUND)
    {
        m_PendingId = pCat->AddPending(GetId());
        //pCat->ObjectRefreshed(GetId());
    }
    //start thread to load schemes
    if(!CreateAndRunLoadChildrenThread())
        return false;

    return wxGxObjectContainer::HasChildren(); 
}

bool wxGxRemoteDBSchemaUI::CreateAndRunLoadChildrenThread(void)
{
    if(!GetThread())
    {
        if (CreateThread(wxTHREAD_JOINABLE) != wxTHREAD_NO_ERROR)
        {
            wxLogError(_("Could not create the thread!"));
            return false;
        }
    }

    if(GetThread()->IsRunning())
        return true;

    if (GetThread()->Run() != wxTHREAD_NO_ERROR)
    {
        wxLogError(_("Could not run the thread!"));
        return false;
    }
    return true;
}

//thread to load remote DB tables
//before exit we assume that no tables exist
wxThread::ExitCode wxGxRemoteDBSchemaUI::Entry()
{
    LoadChildren();

    wxThreadEvent event( wxEVT_THREAD, EXIT_EVENT );
    wxQueueEvent( this, event.Clone() );

    return (wxThread::ExitCode)0;
}

void wxGxRemoteDBSchemaUI::OnThreadFinished(wxThreadEvent& event)
{
    wxGxCatalogUI* pCat = wxDynamicCast(GetGxCatalog(), wxGxCatalogUI);
    if(pCat)
    {
        pCat->RemovePending(m_PendingId);
        m_PendingId = wxNOT_FOUND;
        pCat->ObjectRefreshed(GetId());
        pCat->ObjectChanged(GetId());
    }
}

void wxGxRemoteDBSchemaUI::AddTable(const wxString &sTableName, const wxGISEnumDatasetType eType)
{
    switch(eType)
    {
    case enumGISFeatureDataset:
        m_pwxGISRemoteConn->Reference();
        new wxGxPostGISFeatureDatasetUI(GetName(), m_pwxGISRemoteConn, this, sTableName, "", m_oLargeIconFeatureClass, m_oSmallIconFeatureClass);
        break;
    case enumGISRasterDataset:
        break;
    case enumGISTableDataset:
    default:
        m_pwxGISRemoteConn->Reference();
        new wxGxPostGISTableDatasetUI(GetName(), m_pwxGISRemoteConn, this, sTableName, "", m_oLargeIconTable, m_oSmallIconTable);
        break;
    };
}

#endif //wxGIS_USE_POSTGRES

//--------------------------------------------------------------
//class wxGxTMSWebServiceUI
//--------------------------------------------------------------

IMPLEMENT_CLASS(wxGxTMSWebServiceUI, wxGxTMSWebService)

wxGxTMSWebServiceUI::wxGxTMSWebServiceUI(wxGxObject *oParent, const wxString &soName, const CPLString &soPath, const wxIcon &icLargeIcon, const wxIcon &icSmallIcon, const wxIcon &icLargeIconDsbl, const wxIcon &icSmallIconDsbl) : wxGxTMSWebService(oParent, soName, soPath)
{
    m_icLargeIcon = icLargeIcon;
    m_icSmallIcon = icSmallIcon;
    m_icLargeIconDsbl = icLargeIconDsbl;
    m_icSmallIconDsbl = icSmallIconDsbl;
}

wxGxTMSWebServiceUI::~wxGxTMSWebServiceUI(void)
{
}

wxIcon wxGxTMSWebServiceUI::GetLargeImage(void)
{
    if(m_pwxGISDataset && m_pwxGISDataset->IsOpened())
    {
        return m_icLargeIcon;
    }
    else
    {
        return m_icLargeIconDsbl;
    }
}

wxIcon wxGxTMSWebServiceUI::GetSmallImage(void)
{
    if(m_pwxGISDataset && m_pwxGISDataset->IsOpened())
    {
        return m_icSmallIcon;
    }
    else
    {
        return m_icSmallIconDsbl;
    }
}

void wxGxTMSWebServiceUI::EditProperties(wxWindow *parent)
{
	//wxGISRemoteConnDlg dlg(m_sPath, parent);
	//if(dlg.ShowModal() == wxID_OK)
	//{
 //       if(!m_pwxGISDataset)
 //           return;
 //       if(m_pwxGISDataset->IsOpened())
 //           m_pwxGISDataset->Close();
 //       wxGIS_GXCATALOG_EVENT(ObjectChanged);
	//}

    wxPropertySheetDialog PropertySheetDialog;
    if (!PropertySheetDialog.Create(parent, wxID_ANY, _("Properties"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER))
        return;
    PropertySheetDialog.SetIcon(properties_xpm);
    PropertySheetDialog.CreateButtons(wxOK);
    wxWindow* pParentWnd = static_cast<wxWindow*>(PropertySheetDialog.GetBookCtrl());

    wxGISRasterPropertyPage* RasterPropertyPage = new wxGISRasterPropertyPage(this, pParentWnd);
    PropertySheetDialog.GetBookCtrl()->AddPage(RasterPropertyPage, RasterPropertyPage->GetPageName());
	wxGISRasterDataset* pDset = wxDynamicCast(GetDataset(), wxGISRasterDataset);
	if(pDset)
	{
        if(!pDset->IsOpened())
            pDset->Open(true);
		wxGISSpatialReferencePropertyPage* SpatialReferencePropertyPage = new wxGISSpatialReferencePropertyPage(pDset->GetSpatialReference(), pParentWnd);
		PropertySheetDialog.GetBookCtrl()->AddPage(SpatialReferencePropertyPage, SpatialReferencePropertyPage->GetPageName());
	}

    //TODO: Additional page for virtual raster VRTSourcedDataset with sources files

    //PropertySheetDialog.LayoutDialog();
    PropertySheetDialog.SetSize(480,640);
    PropertySheetDialog.Center();

    PropertySheetDialog.ShowModal();
}