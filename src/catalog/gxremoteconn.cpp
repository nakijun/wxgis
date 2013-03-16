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

#include "wxgis/catalog/gxremoteconn.h"

#ifdef wxGIS_USE_POSTGRES

#include "wxgis/datasource/sysop.h"
#include "wxgis/catalog/gxpostgisdataset.h"
#include "wxgis/datasource/postgisdataset.h"
#include "wxgis/catalog/gxcatalog.h"
#include "wxgis/catalog/gxdbconnfactory.h"

//--------------------------------------------------------------
//class wxGxRemoteConnection
//--------------------------------------------------------------
IMPLEMENT_CLASS(wxGxRemoteConnection, wxGxObjectContainer)

wxGxRemoteConnection::wxGxRemoteConnection(wxGxObject *oParent, const wxString &soName, const CPLString &soPath) : wxGxObjectContainer(oParent, soName, soPath)
{
    m_pwxGISDataset = NULL;
}

wxGxRemoteConnection::~wxGxRemoteConnection(void)
{
    Disconnect();
    wsDELETE(m_pwxGISDataset);
}

bool wxGxRemoteConnection::HasChildren()
{
    if(!Connect())
        return false;
    else
        return wxGxObjectContainer::HasChildren();
}

wxGISDataset* const wxGxRemoteConnection::GetDatasetFast(void)
{
 	if(m_pwxGISDataset == NULL)
    {
        wxGISPostgresDataSource* pDSet = new wxGISPostgresDataSource(m_sPath);
        m_pwxGISDataset = wxStaticCast(pDSet, wxGISDataset);
        m_pwxGISDataset->Reference();
    }
    m_pwxGISDataset->Reference();
    return m_pwxGISDataset;
}

bool wxGxRemoteConnection::Delete(void)
{
	wxGISDataset* pDSet = GetDatasetFast();
    
    if(!pDSet)
        return false;

    bool bRet = pDSet->Delete();
    wsDELETE(pDSet);

    if( !bRet )
    {
        const char* err = CPLGetLastErrorMsg();
		wxLogError(_("Operation '%s' failed! GDAL error: %s, %s '%s'"), _("Delete"), wxString(err, wxConvUTF8).c_str(), GetCategory().c_str(), wxString(m_sPath, wxConvUTF8).c_str());
		return false;	
    }
    return true;
}

bool wxGxRemoteConnection::Rename(const wxString &sNewName)
{
	wxGISDataset* pDSet = GetDatasetFast();
    
    if(!pDSet)
        return false;

	if(pDSet->IsOpened())
		pDSet->Close();

    bool bRet = pDSet->Rename(sNewName);
    wsDELETE(pDSet);

	if( !bRet )
	{
		const char* err = CPLGetLastErrorMsg();
		wxLogError(_("Operation '%s' failed! GDAL error: %s, %s '%s'"), _("Rename"), wxString(err, wxConvUTF8).c_str(), GetCategory().c_str(), wxString(m_sPath, wxConvUTF8).c_str());
		return false;
	}	
    return true;
}


bool wxGxRemoteConnection::Copy(const CPLString &szDestPath, ITrackCancel* const pTrackCancel)
{
    if(pTrackCancel)
        pTrackCancel->PutMessage(wxString::Format(_("%s %s %s"), _("Copy"), GetCategory().c_str(), m_sName.c_str()), wxNOT_FOUND, enumGISMessageInfo);

	wxGISDataset* pDSet = GetDatasetFast();
    
    if(!pDSet)
    {
        if(pTrackCancel)
            pTrackCancel->PutMessage(wxString::Format(_("%s %s %s failed!"), _("Copy"), GetCategory().c_str(), m_sName.c_str()), wxNOT_FOUND, enumGISMessageErr);
        return false;
    }

    bool bRet = pDSet->Copy(szDestPath, pTrackCancel);
    wsDELETE(pDSet);

    if(!bRet)
    {
        const char* err = CPLGetLastErrorMsg();
        wxString sErr = wxString::Format(_("Operation '%s' failed! GDAL error: %s, %s '%s'"), _("Copy"), wxString(err, wxConvUTF8).c_str(), GetCategory().c_str(), wxString(m_sPath, wxConvUTF8).c_str());
		wxLogError(sErr);
        if(pTrackCancel)
            pTrackCancel->PutMessage(sErr, wxNOT_FOUND, enumGISMessageErr);
		return false;	
    }
	
    return true;
}

bool wxGxRemoteConnection::Move(const CPLString &szDestPath, ITrackCancel* const pTrackCancel)
{
    if(pTrackCancel)
		pTrackCancel->PutMessage(wxString::Format(_("%s %s %s"), _("Move"), GetCategory().c_str(), m_sName.c_str()), wxNOT_FOUND, enumGISMessageInfo);

	wxGISDataset* pDSet = GetDatasetFast();
    
    if(!pDSet)
    {
        if(pTrackCancel)
            pTrackCancel->PutMessage(wxString::Format(_("%s %s %s failed!"), _("Move"), GetCategory().c_str(), m_sName.c_str()), wxNOT_FOUND, enumGISMessageErr);
        return false;
    }

    bool bRet = pDSet->Move(szDestPath, pTrackCancel);
    wsDELETE(pDSet);

    if(!bRet)
    {
        const char* err = CPLGetLastErrorMsg();
        wxString sErr = wxString::Format(_("Operation '%s' failed! GDAL error: %s, %s '%s'"), _("Move"), GetCategory().c_str(), wxString(err, wxConvUTF8).c_str(), wxString(m_sPath, wxConvUTF8).c_str());
		wxLogError(sErr);
        if(pTrackCancel)
            pTrackCancel->PutMessage(sErr, wxNOT_FOUND, enumGISMessageErr);
		return false;	
    } 

    return true;
}

bool wxGxRemoteConnection::Connect(void)
{
    if(IsConnected())
        return true;
    bool bRes = true;
    wxGISPostgresDataSource* pDSet = wxDynamicCast(GetDatasetFast(), wxGISPostgresDataSource);
    if(pDSet)
    {
        bRes = pDSet->Open();
        if(!bRes)
            return bRes;

        LoadChildren();

        wxGIS_GXCATALOG_EVENT(ObjectChanged);
    }
    wsDELETE(pDSet);
    return bRes;
}

bool wxGxRemoteConnection::Disconnect(void)
{
    if(!IsConnected())
        return true;
    wxGISDataset* pDSet = GetDatasetFast();
    if(pDSet)
    {
        pDSet->Close();
        DestroyChildren();
        wxGIS_GXCATALOG_EVENT(ObjectChanged);
    }
    wsDELETE(pDSet);
    return true;
}

bool wxGxRemoteConnection::IsConnected()
{
    wxGISDataset* pDSet = GetDatasetFast();
    return pDSet && pDSet->IsOpened();
}

void wxGxRemoteConnection::Refresh(void)
{
    DestroyChildren();
    LoadChildren();
    wxGxObject::Refresh();
}

void wxGxRemoteConnection::LoadChildren(void)
{
    wxGISPostgresDataSource* pDSet = wxDynamicCast(GetDatasetFast(), wxGISPostgresDataSource);
    if(NULL == pDSet)
        return;

    //list all tables include ones which we don't have access
    //SELECT * FROM pg_catalog.pg_tables WHERE schemaname NOT LIKE 'pg_%' AND schemaname NOT LIKE 'information_schema' AND schemaname NOT LIKE 'layer'
    
    //previous sql statement
    //SELECT table_schema, table_name FROM information_schema.tables WHERE table_schema NOT LIKE 'pg_%' AND table_schema NOT LIKE 'information_schema'
    wxGISTable* pInfoScheme = wxDynamicCast(pDSet->ExecuteSQL(wxT("SELECT table_schema, table_name FROM information_schema.tables WHERE table_schema NOT IN ('pg_catalog', 'information_schema')"), wxT("PG")), wxGISTable);

    if(NULL == pInfoScheme)
    {
        wsDELETE(pDSet);
        return;
    }

    bool bLoadSystemTablesAndSchemes = false;
    wxGxCatalog* pGxCatalog = wxDynamicCast(GetGxCatalog(), wxGxCatalog);
    if(pGxCatalog)
    {
        wxGxDBConnectionFactory* const pDBConnectionFactory = wxDynamicCast(pGxCatalog->GetObjectFactoryByName(_("DataBase connections")), wxGxDBConnectionFactory);
        if(pDBConnectionFactory)
        {
            bLoadSystemTablesAndSchemes = pDBConnectionFactory->GetLoadSystemTablesAndShemes();
        }
    }

    bool bHasGeom(false), bHasGeog(false), bHasRaster(false);
    std::map<wxString, wxArrayString> smSchema;

    wxFeatureCursor Cursor = pInfoScheme->Search();
    wxGISFeature Feature;
    while( (Feature = Cursor.Next()).IsOk() )
    {
        wxString sScheme = Feature.GetFieldAsString(0);
        if(!bLoadSystemTablesAndSchemes)
        {
            if(sScheme.IsSameAs(wxT("topology")))//TODO: add more schemes
                continue;
        }
        wxString sName = Feature.GetFieldAsString(1);
        if(!bLoadSystemTablesAndSchemes && (sName.IsSameAs("raster_overviews") || sName.IsSameAs("spatial_ref_sys")))
            continue;

        //looj for postgis special tables oly in public
        if(sScheme.IsSameAs(wxT("public")))
        {
            if(sName.IsSameAs(wxT("geometry_columns")))
                bHasGeom = true;
            else if(sName.IsSameAs(wxT("geography_columns")))
                bHasGeog = true;
            else if(sName.IsSameAs(wxT("raster_columns")))
                bHasRaster = true;
            else
                smSchema[sScheme].Add(sName);
        }
        else
            smSchema[sScheme].Add(sName);
    }

    wxDELETE(pInfoScheme);

    for(std::map<wxString, wxArrayString>::const_iterator IT = smSchema.begin(); IT != smSchema.end(); ++IT)
    {
        pDSet->Reference();
        GetNewRemoteDBSchema(IT->first, IT->second, bHasGeom, bHasGeog, bHasRaster, pDSet);
    }

    wsDELETE(pDSet);
}


wxGxRemoteDBSchema* wxGxRemoteConnection::GetNewRemoteDBSchema(const wxString &sName, const wxArrayString &saTables, bool bHasGeom, bool bHasGeog, bool bHasRaster, wxGISPostgresDataSource *pwxGISRemoteConn)
{
    return new wxGxRemoteDBSchema(saTables, bHasGeom, bHasGeog, bHasRaster, pwxGISRemoteConn, this, sName, "");
}

//--------------------------------------------------------------
//class wxGxRemoteDBSchema
//--------------------------------------------------------------

IMPLEMENT_CLASS(wxGxRemoteDBSchema, wxGxObjectContainer)

wxGxRemoteDBSchema::wxGxRemoteDBSchema(const wxArrayString &saTables, bool bHasGeom, bool bHasGeog, bool bHasRaster, wxGISPostgresDataSource* pwxGISRemoteConn, wxGxObject *oParent, const wxString &soName, const CPLString &soPath) : wxGxObjectContainer(oParent, soName, soPath)
{
    m_pwxGISRemoteConn = pwxGISRemoteConn;
    m_bChildrenLoaded = false;
    m_saTables = saTables;
    m_bHasGeom = bHasGeom;
    m_bHasGeog = bHasGeog;
    m_bHasRaster = bHasRaster;
}

wxGxRemoteDBSchema::~wxGxRemoteDBSchema(void)
{
    wsDELETE(m_pwxGISRemoteConn);
}

bool wxGxRemoteDBSchema::HasChildren(void)
{
    LoadChildren();
    return wxGxObjectContainer::HasChildren(); 
}

void wxGxRemoteDBSchema::Refresh(void)
{
    DestroyChildren();
    m_bChildrenLoaded = false;
    LoadChildren();
    wxGxObject::Refresh();
}


bool wxGxRemoteDBSchema::Delete(void)
{
    return false;
}

bool wxGxRemoteDBSchema::Rename(const wxString &sNewName)
{
    return false;
}

void wxGxRemoteDBSchema::LoadChildren(void)
{
    wxCriticalSectionLocker locker(m_CritSect);
    if(m_bChildrenLoaded)
        return;
    wxCHECK_RET(m_pwxGISRemoteConn, wxT("wxGISRemoteConnection pointer is NULL"));

    bool bLoadSystemTablesAndSchemes = false;
    wxGxCatalog* pGxCatalog = wxDynamicCast(GetGxCatalog(), wxGxCatalog);
    if(pGxCatalog)
    {
        wxGxDBConnectionFactory* const pDBConnectionFactory = wxDynamicCast(pGxCatalog->GetObjectFactoryByName(_("DataBase connections")), wxGxDBConnectionFactory);
        if(pDBConnectionFactory)
        {
            bLoadSystemTablesAndSchemes = pDBConnectionFactory->GetLoadSystemTablesAndShemes();
        }
    }

    //get geometry and geography
    if(m_bHasGeom)
    {
        //remove table name from tables list
        wxGISTable* pTableList = wxDynamicCast(m_pwxGISRemoteConn->ExecuteSQL(wxString::Format(wxT("SELECT f_table_name FROM public.geometry_columns WHERE f_table_schema LIKE '%s'"), GetName().c_str()), wxT("PG")), wxGISTable);

        if(NULL != pTableList)
        {
            wxFeatureCursor Cursor = pTableList->Search();
            wxGISFeature Feature;
            while( (Feature = Cursor.Next()).IsOk() )
            {
                wxString sTable = Feature.GetFieldAsString(0);
                int nIndex = m_saTables.Index(sTable);
                if(nIndex != wxNOT_FOUND)
                {
                    AddTable(sTable, enumGISFeatureDataset);
                    m_saTables.RemoveAt(nIndex);
                }
            }
            wxDELETE( pTableList );
        }
    }

    m_bChildrenLoaded = true;
    if(m_saTables.IsEmpty())
        return;
    
    if(m_bHasGeog)
    {
        //remove table name from tables list
        wxGISTable* pTableList = wxDynamicCast(m_pwxGISRemoteConn->ExecuteSQL(wxString::Format(wxT("SELECT f_table_name FROM public.geography_columns WHERE f_table_schema LIKE '%s'"), GetName().c_str()), wxT("PG")), wxGISTable);

        if(NULL != pTableList)
        {
            wxFeatureCursor Cursor = pTableList->Search();
            wxGISFeature Feature;
            while( (Feature = Cursor.Next()).IsOk() )
            {
                wxString sTable = Feature.GetFieldAsString(0);
                int nIndex = m_saTables.Index(sTable);
                if(nIndex != wxNOT_FOUND)
                {
                    AddTable(sTable, enumGISFeatureDataset);
                    m_saTables.RemoveAt(nIndex);
                }
            }
            wxDELETE( pTableList );
        }
    }
        
    if(m_saTables.IsEmpty())
        return;

    if(m_bHasRaster)
    {
        //remove table name from tables list
        wxGISTable* pTableList = wxDynamicCast(m_pwxGISRemoteConn->ExecuteSQL(wxString::Format(wxT("SELECT r_table_name FROM puyblic.raster_columns WHERE r_table_schema LIKE '%s'"), GetName().c_str()), wxT("PG")), wxGISTable);

        if(NULL != pTableList)
        {
            wxFeatureCursor Cursor = pTableList->Search();
            wxGISFeature Feature;
            while( (Feature = Cursor.Next()).IsOk() )
            {
                wxString sTable = Feature.GetFieldAsString(0);
                int nIndex = m_saTables.Index(sTable);
                if(nIndex != wxNOT_FOUND)
                {
                    AddTable(sTable, enumGISRasterDataset);
                    m_saTables.RemoveAt(nIndex);
                }
            }
            wxDELETE( pTableList );
        }
    }

    
    for(size_t i = 0; i < m_saTables.GetCount(); ++i)
    {
        AddTable(m_saTables[i], enumGISTableDataset);
    }
}


void wxGxRemoteDBSchema::AddTable(const wxString &sTableName, const wxGISEnumDatasetType eType)
{
    switch(eType)
    {
    case enumGISFeatureDataset:
        m_pwxGISRemoteConn->Reference();
        new wxGxPostGISFeatureDataset(GetName(), m_pwxGISRemoteConn, this, sTableName, "");
        break;
    case enumGISRasterDataset:
        break;
    case enumGISTableDataset:
    default:
        m_pwxGISRemoteConn->Reference();
        new wxGxPostGISTableDataset(GetName(), m_pwxGISRemoteConn, this, sTableName, "");
        break;
    };

    //IGxObject* pGxObject(NULL);
    //if(bHasGeometry)
    //{
    //    wxGxPostGISFeatureDataset* pGxPostGISFeatureDataset = new wxGxPostGISFeatureDataset(szName, szSchema, m_pwxGISRemoteConn);
    //    pGxObject = static_cast<IGxObject*>(pGxPostGISFeatureDataset);
    //}
    //else
    //{
    //    wxGxPostGISTableDataset* pGxPostGISTableDataset = new wxGxPostGISTableDataset(szName, szSchema, m_pwxGISRemoteConn);
    //    pGxObject = static_cast<IGxObject*>(pGxPostGISTableDataset);
    //}
    //    wxGISEnumDatasetType eType = pGISDataset->GetType();
    //    IGxObject* pGxObject(NULL);
    //    switch(eType)
    //    {
    //    case enumGISFeatureDataset:
    //        {
    //            wxGxPostGISFeatureDataset* pGxPostGISFeatureDataset = new wxGxPostGISFeatureDataset(pGISDataset->GetPath(), pGISDataset);
    //            pGxObject = static_cast<IGxObject*>(pGxPostGISFeatureDataset);
    //        }
    //        break;
    //    case enumGISTableDataset:
    //        {
    //            wxGxPostGISTableDataset* pGxPostGISTableDataset = new wxGxPostGISTableDataset(pGISDataset->GetPath(), pGISDataset);
    //            pGxObject = static_cast<IGxObject*>(pGxPostGISTableDataset);
    //        }
    //        break;
    //    case enumGISRasterDataset:
    //        break;
    //    default:
    //    case enumGISContainer:
    //        break;
    //    };

    //if(pGxObject)
    //{
	   // bool ret_code = AddChild(pGxObject);
	   // if(!ret_code)
		  //  wxDELETE(pGxObject);
    //}

    //    pDSet->Reference();
    //    GetNewRemoteDBSchema(sScheme, pDSet);


    //wxGISTable* pInfoScheme = wxDynamicCast(pDSet->ExecuteSQL(wxT("SELECT table_schema, table_name from information_schema.tables WHERE table_schema NOT LIKE 'pg_%' AND table_schema NOT LIKE 'information_schema'")), wxGISTable);//maybe more columns

    ////create arraystring of table names for each scheme
    //std::map<wxString, wxArrayString> DBSchema;

    //wxFeatureCursor Cursor = pInfoScheme->Search();
    //wxGISFeature Feature;
    ////wxArrayString saGeomCol, saGeogCol; 
    //while( (Feature = Cursor.Next()).IsOk() )
    //{
    //    wxString sScheme = Feature.GetFieldAsString(0);
    //    pDSet->Reference();
    //    new wxGxRemoteDBSchema(this, sScheme, "", pDSet);
    ////    wxString sTable = Feature.GetFieldAsString(0);
    ////    if(sTable.IsSameAs("geometry_columns", false))
    ////    {
    ////        saGeomCol.Add(sScheme + wxT(".") + sTable);
    ////    }
    ////    else if(sTable.IsSameAs("geography_columns", false))
    ////    {
    ////        saGeogCol.Add(sScheme + wxT(".") + sTable);
    ////    }
    ////    else
    ////    {
    ////        DBSchema[sScheme].Add(sTable);
    ////    }
    //}

    //wxDELETE(pInfoScheme);

    //wsDELETE(pDSet);


        ////get all schemes
        //SELECT * from information_schema.tables WHERE table_schema NOT LIKE 'pg_%' AND table_schema NOT LIKE 'information_schema';

        ////get geometry tables names and schemes
        //select * from geography_columns;
        //select * from public.geometry_columns;
        ////get inherits
        //
        //wxGISDataset* pGeoMetaData = pDSet->ExecuteSQL(wxT("SELECT f_table_schema,f_table_name from geography_columns"));
 
        //SELECT * FROM pg_class WHERE relname LIKE '%_columns';

/*
    OGRPGDataSource* pDS = dynamic_cast<OGRPGDataSource*>(m_pwxGISRemoteConn->GetDataSource());
    Oid nGeogOID = pDS->GetGeographyOID();
    Oid nGeomOID = pDS->GetGeometryOID();
    wxGISTableSPtr pGeostruct;
    if(nGeogOID != 0)
        pGeostruct = boost::dynamic_pointer_cast<wxGISTable>(m_pwxGISRemoteConn->ExecuteSQL(wxString(wxT("SELECT f_table_schema,f_table_name from geography_columns"))));//,f_geography_column,type
    else if(nGeomOID != 0)
        pGeostruct = boost::dynamic_pointer_cast<wxGISTable>(m_pwxGISRemoteConn->ExecuteSQL(wxString(wxT("SELECT f_table_schema,f_table_name from geometry_columns"))));//,f_geography_column,type

    std::vector<PGTABLEDATA> aDBStruct;
    if(pGeostruct)
    {
        OGRFeatureSPtr pFeature;
        while((pFeature = pGeostruct->Next()) != NULL)
        {
            PGTABLEDATA data = {true, pFeature->GetFieldAsString(1), pFeature->GetFieldAsString(0)};
            aDBStruct.push_back(data);
        }
    }

    std::vector<PGTABLEDATA> aDBStructOut;
    OGRDataSource* poDS = m_pwxGISRemoteConn->GetDataSource();
    for(size_t nIndex = 0; nIndex < m_pwxGISDataset->GetSubsetsCount(); ++nIndex)
    {
        OGRPGTableLayer* pPGTableLayer = dynamic_cast<OGRPGTableLayer*>(poDS->GetLayer(nIndex));
        CPLString sTableName (pPGTableLayer->GetTableName());
        CPLString sTableSchema (pPGTableLayer->GetSchemaName());
        bool bAdd(false);
        for(size_t j = 0; j < aDBStruct.size(); ++j)
        {
            if(aDBStruct[j].sTableName == sTableName && aDBStruct[j].sTableSchema == sTableSchema)
            {
                aDBStructOut.push_back( aDBStruct[j] );
                bAdd = true;
                break;
            }
        }

        if(!bAdd)
        {
            //check inherits
            CPLString osCommand;
            osCommand.Printf("SELECT pg_class.relname FROM pg_class WHERE oid = "
                                "(SELECT pg_inherits.inhparent FROM pg_inherits WHERE inhrelid = "
                                "(SELECT c.oid FROM pg_class c, pg_namespace n WHERE c.relname = '%s' AND c.relnamespace=n.oid AND n.nspname = '%s'))",
                                sTableName.c_str(), sTableSchema.c_str() );
            wxString sSQLExpression(osCommand, wxConvUTF8);
            wxGISTableSPtr pInherits = boost::dynamic_pointer_cast<wxGISTable>(m_pwxGISRemoteConn->ExecuteSQL(sSQLExpression));
            if(pInherits)
            {
                OGRFeatureSPtr pFeature;
                while((pFeature = pInherits->Next()) != NULL)
                {
                    CPLString soParentName = pFeature->GetFieldAsString(0);
                    for(size_t j = 0; j < aDBStruct.size(); ++j)
                    {
                        if(aDBStruct[j].sTableName == soParentName && aDBStruct[j].sTableSchema == sTableSchema)
                        {
                            PGTABLEDATA data = {true, sTableName, sTableSchema};
                            aDBStructOut.push_back(data);
                            bAdd = true;
                            break;
                        }
                    }
                }
            }
        }
        if(!bAdd)
        {
            PGTABLEDATA data = {false, sTableName, sTableSchema};
            aDBStructOut.push_back( data );
        }
    }

    std::map<CPLString, wxGxRemoteDBSchema*> DBSchema;

    for(size_t i = 0; i < aDBStructOut.size(); ++i)
    {
        wxGxRemoteDBSchema* pGxRemoteDBSchema = DBSchema[aDBStructOut[i].sTableSchema];
        if(pGxRemoteDBSchema)
            pGxRemoteDBSchema->AddTable(aDBStructOut[i].sTableName, aDBStructOut[i].sTableSchema, aDBStructOut[i].bHasGeometry);
        else
        {
            pGxRemoteDBSchema = GetNewRemoteDBSchema(aDBStructOut[i].sTableSchema, m_pwxGISRemoteConn);
            bool ret_code = AddChild(static_cast<IGxObject*>(pGxRemoteDBSchema));
		    if(!ret_code)
                wxDELETE(pGxRemoteDBSchema);
            else
            {
                pGxRemoteDBSchema->AddTable(aDBStructOut[i].sTableName, aDBStructOut[i].sTableSchema, aDBStructOut[i].bHasGeometry);
                DBSchema[aDBStructOut[i].sTableSchema] = pGxRemoteDBSchema;
            }
        }
    }

  //  for(auto itr = DBSchema.begin(); itr != DBSchema.end(); ++itr)
  //  {
  //      bool ret_code = AddChild(static_cast<IGxObject*>(itr->second));
		//if(!ret_code)
  //          wxDELETE(itr->second);
  //  }*/
}



#endif //wxGIS_USE_POSTGRES