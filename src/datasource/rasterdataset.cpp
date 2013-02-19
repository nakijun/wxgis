/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  RasterDataset class.
 * Author:   Baryshnikov Dmitriy (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2009-2011,2013 Bishop
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
#include "wxgis/datasource/rasterdataset.h"
#include "wxgis/datasource/sysop.h"
#include "wxgis/datasource/rasterop.h"

#include <wx/filename.h>
#include "gdal_rat.h"
#include "gdalwarper.h"

//---------------------------------------
// wxGISFeatureDataset
//---------------------------------------

IMPLEMENT_CLASS(wxGISRasterDataset, wxGISDataset)

wxGISRasterDataset::wxGISRasterDataset(const CPLString &sPath, wxGISEnumRasterDatasetType nType) : wxGISDataset(sPath)
{
    m_nSubType = (int)nType;
	m_nType = enumGISRasterDataset;
	m_nDataType = GDT_Unknown;
    m_paNoData = NULL;
    m_bHasOverviews = false;
    m_bHasStats = false;
    m_poMainDataset = NULL;
    m_poDataset = NULL;
    m_nBandCount = 0;
}

wxGISRasterDataset::~wxGISRasterDataset(void)
{
	Close();
    if(m_paNoData)
        delete [] m_paNoData;
}

void wxGISRasterDataset::Close(void)
{
	if(IsOpened())
    {
		m_stExtent.MinX = m_stExtent.MaxX = m_stExtent.MinY = m_stExtent.MaxY = 0;
		m_nDataType = GDT_Unknown;
        m_bHasOverviews = false;
        m_bHasStats = false;
        m_nBandCount = 0;
        if(m_poDataset)
		{
            if(GDALDereferenceDataset(m_poDataset) < 1)
                GDALClose(m_poDataset);
			m_poDataset = NULL;
            m_poMainDataset = NULL;
		}
        if(m_poMainDataset)
		{
            if(GDALDereferenceDataset(m_poMainDataset) < 1)
                GDALClose(m_poMainDataset);
			m_poMainDataset = NULL;
		}
		m_bIsOpened = false;
        m_bIsReadOnly = true;
    }
    wxGISDataset::Close();
}

bool wxGISRasterDataset::Delete(int iLayer, ITrackCancel* const pTrackCancel)
{
    wxCHECK_MSG(iLayer == 0, false, wxT("Deleting sublayer of raster without layers") );
	
    return wxGISDataset::Delete(iLayer, pTrackCancel);
}

char **wxGISRasterDataset::GetFileList()
{
    char **papszFileList = NULL;
    CPLString szPath;
    //papszFileList = CSLAddString( papszFileList, osIMDFile );
    switch(m_nSubType)
    {
	case enumRasterSAGA:
		szPath = (char*)CPLResetExtension(m_sPath, "sgrd");
		if(CPLCheckForFile((char*)szPath.c_str(), NULL))
			papszFileList = CSLAddString( papszFileList, szPath );
		szPath = (char*)CPLResetExtension(m_sPath, "mgrd");
		if(CPLCheckForFile((char*)szPath.c_str(), NULL))
			papszFileList = CSLAddString( papszFileList, szPath );
		break;
    case enumRasterTiff:
    case enumRasterTil:
		szPath = (char*)CPLResetExtension(m_sPath, "imd");
		if(CPLCheckForFile((char*)szPath.c_str(), NULL))
			papszFileList = CSLAddString( papszFileList, szPath );
		szPath = (char*)CPLResetExtension(m_sPath, "pvl");
		if(CPLCheckForFile((char*)szPath.c_str(), NULL))
			papszFileList = CSLAddString( papszFileList, szPath );
		szPath = (char*)CPLResetExtension(m_sPath, "att");
		if(CPLCheckForFile((char*)szPath.c_str(), NULL))
			papszFileList = CSLAddString( papszFileList, szPath );
		szPath = (char*)CPLResetExtension(m_sPath, "eph");
		if(CPLCheckForFile((char*)szPath.c_str(), NULL))
			papszFileList = CSLAddString( papszFileList, szPath );
        //RPC specific
        szPath = (char*)CPLResetExtension(m_sPath, "rpb");
        if(CPLCheckForFile((char*)szPath.c_str(), NULL))
            papszFileList = CSLAddString( papszFileList, szPath );
        szPath = (char*)CPLResetExtension(m_sPath, "rpc");
        if(CPLCheckForFile((char*)szPath.c_str(), NULL))
            papszFileList = CSLAddString( papszFileList, szPath );
        {
        CPLString szRpcName = CPLString(CPLGetBasename(m_sPath)) + CPLString("_rpc.txt");
        szPath = (char*)CPLFormFilename(CPLGetPath(m_sPath), (const char*)szRpcName.c_str(), NULL);
        if(CPLCheckForFile((char*)szPath.c_str(), NULL))
            papszFileList = CSLAddString( papszFileList, szPath );
        }
        break;
	case enumRasterImg:
        szPath = (char*)CPLResetExtension(m_sPath, "ige");
		if(CPLCheckForFile((char*)szPath.c_str(), NULL))
			papszFileList = CSLAddString( papszFileList, szPath );
        break;
    case enumRasterBmp:
	case enumRasterPng:
	case enumRasterGif:
    case enumRasterUnknown:
    default:
        break;
    }
    //check for world file
	CPLString soWldFilePath = GetWorldFilePath(m_sPath);
	if(!soWldFilePath.empty())
		papszFileList = CSLAddString( papszFileList, soWldFilePath );
    szPath = m_sPath + CPLString(".xml");
    if(CPLCheckForFile((char*)szPath.c_str(), NULL))
        papszFileList = CSLAddString( papszFileList, szPath );
    szPath = m_sPath + CPLString(".aux");
    if(CPLCheckForFile((char*)szPath.c_str(), NULL))
        papszFileList = CSLAddString( papszFileList, szPath );
    szPath = m_sPath + CPLString(".aux.xml");
    if(CPLCheckForFile((char*)szPath.c_str(), NULL))
        papszFileList = CSLAddString( papszFileList, szPath );
    szPath = m_sPath + CPLString(".ovr");
    if(CPLCheckForFile((char*)szPath.c_str(), NULL))
        papszFileList = CSLAddString( papszFileList, szPath );
    szPath = m_sPath + CPLString(".ovr.aux.xml");
    if(CPLCheckForFile((char*)szPath.c_str(), NULL))
        papszFileList = CSLAddString( papszFileList, szPath );
    szPath = m_sPath + CPLString(".rrd");
    if(CPLCheckForFile((char*)szPath.c_str(), NULL))
        papszFileList = CSLAddString( papszFileList, szPath );
    szPath = (char*)CPLResetExtension(m_sPath, "xml");
    if(CPLCheckForFile((char*)szPath.c_str(), NULL))
        papszFileList = CSLAddString( papszFileList, szPath );
    szPath = (char*)CPLResetExtension(m_sPath, "lgo");
    if(CPLCheckForFile((char*)szPath.c_str(), NULL))
        papszFileList = CSLAddString( papszFileList, szPath );
    szPath = (char*)CPLResetExtension(m_sPath, "aux");
    if(CPLCheckForFile((char*)szPath.c_str(), NULL))
        papszFileList = CSLAddString( papszFileList, szPath );
    szPath = (char*)CPLResetExtension(m_sPath, "aux.xml");
    if(CPLCheckForFile((char*)szPath.c_str(), NULL))
        papszFileList = CSLAddString( papszFileList, szPath );
    szPath = (char*)CPLResetExtension(m_sPath, "ovr");
    if(CPLCheckForFile((char*)szPath.c_str(), NULL))
        papszFileList = CSLAddString( papszFileList, szPath );
    szPath = (char*)CPLResetExtension(m_sPath, "rrd");
    if(CPLCheckForFile((char*)szPath.c_str(), NULL))
        papszFileList = CSLAddString( papszFileList, szPath );
    szPath = (char*)CPLResetExtension(m_sPath, "prj");
    if(CPLCheckForFile((char*)szPath.c_str(), NULL))
        papszFileList = CSLAddString( papszFileList, szPath );

    return papszFileList;
}

bool wxGISRasterDataset::Rename(const wxString &sNewName, ITrackCancel* const pTrackCancel)
{
    if(wxGISDataset::Rename(sNewName, pTrackCancel))
    {
        bool bRet = true;
        switch(m_nSubType)
        {
	    case enumRasterSAGA://write some internal info to files (e.g. sgrd mgrd -> new file name            
            {
                CPLString szDirPath = CPLGetPath(m_sPath);
                //CPLString szName = CPLGetBasename(m_sPath);
	            CPLString szNewName(ClearExt(sNewName).mb_str(wxConvUTF8));
                bRet = FixSAGARaster(szDirPath, szNewName);
            }
		    break;
        default:
		    break;
        };    
	    return bRet;
    }
    else
        return false;
}

bool wxGISRasterDataset::Open(bool bReadOnly)
{
    if(bReadOnly != m_bIsReadOnly)
        Close();

	if(IsOpened())
		return true;

	wxCriticalSectionLocker locker(m_CritSect);


    m_poDataset = (GDALDataset *) GDALOpenShared( m_sPath, bReadOnly == true ? GA_ReadOnly : GA_Update );
    //bug in FindFileInZip() [gdal-1.6.3\port\cpl_vsil_gzip.cpp]

	if( m_poDataset == NULL )
        m_poDataset = (GDALDataset *) GDALOpenShared( FixPathSeparator(m_sPath), bReadOnly == true ? GA_ReadOnly : GA_Update );

	if( m_poDataset == NULL )
    {
		const char* err = CPLGetLastErrorMsg();
		wxString sErr = wxString::Format(_("Raster open failed! Path '%s'. GDAL error: %s"), m_sPath.c_str(), wxString(err, wxConvUTF8).c_str());
		wxLogError(sErr);

		return false;
    }
    
    double adfGeoTransform[6] = { 0, 0, 0, 0, 0, 0 };
	CPLErr err = m_poDataset->GetGeoTransform(adfGeoTransform);
    bool bHasGeoTransform = false;
    if(err != CE_Fatal)
    {
        //look for metadata OV-3
        char** papszMetadata = m_poDataset->GetMetadata("IMD");
        //if(EQUAL(CSLFetchNameValue(papszMetadata, "sensorInfo.satelliteName"), "OV-3"))
        if(papszMetadata && wxGISEQUAL(CSLFetchNameValue(papszMetadata, "md_type"), "pvl") && m_poDataset->GetGCPCount() == 0)
        {
            OGRSpatialReference oOGRSpatialReference(SRS_WKT_WGS84);
            double adfX[4], adfY[4], adfZ[4];
            adfX[0] = CPLAtof(CSLFetchNameValueDef(papszMetadata, "productInfo.geodeticCorners.upperLeftCorner.longitude", "0.0"));
            adfY[0] = CPLAtof(CSLFetchNameValueDef(papszMetadata, "productInfo.geodeticCorners.upperLeftCorner.latitude", "0.0"));
            adfZ[0] = CPLAtof(CSLFetchNameValueDef(papszMetadata, "productInfo.geodeticCorners.upperLeftCorner.height", "0.0"));
            adfX[1] = CPLAtof(CSLFetchNameValueDef(papszMetadata, "productInfo.geodeticCorners.upperRightCorner.longitude", "0.0"));
            adfY[1] = CPLAtof(CSLFetchNameValueDef(papszMetadata, "productInfo.geodeticCorners.upperRightCorner.latitude", "0.0"));
            adfZ[1] = CPLAtof(CSLFetchNameValueDef(papszMetadata, "productInfo.geodeticCorners.upperRightCorner.height", "0.0"));
            adfX[2] = CPLAtof(CSLFetchNameValueDef(papszMetadata, "productInfo.geodeticCorners.lowerRightCorner.longitude", "0.0"));
            adfY[2] = CPLAtof(CSLFetchNameValueDef(papszMetadata, "productInfo.geodeticCorners.lowerRightCorner.latitude", "0.0"));
            adfZ[2] = CPLAtof(CSLFetchNameValueDef(papszMetadata, "productInfo.geodeticCorners.lowerRightCorner.height", "0.0"));
            adfX[3] = CPLAtof(CSLFetchNameValueDef(papszMetadata, "productInfo.geodeticCorners.lowerLeftCorner.longitude", "0.0"));
            adfY[3] = CPLAtof(CSLFetchNameValueDef(papszMetadata, "productInfo.geodeticCorners.lowerLeftCorner.latitude", "0.0"));
            adfZ[3] = CPLAtof(CSLFetchNameValueDef(papszMetadata, "productInfo.geodeticCorners.lowerLeftCorner.height", "0.0"));

            //calc UTM Zone
            double dfXCenter = adfX[0] + (adfX[2] - adfX[0]) / 2.0;
            int nZoneNo = ceil( (180.0 + dfXCenter) / 6.0 );
            OGRSpatialReference oDstOGRSpatialReference(SRS_WKT_WGS84);
            oDstOGRSpatialReference.SetUTM(nZoneNo, adfY[0] > 0);

            OGRCoordinateTransformation *poCT = OGRCreateCoordinateTransformation( &oOGRSpatialReference, &oDstOGRSpatialReference);
            if(poCT)
            {
                int nResult = poCT->Transform(4, adfX, adfY, adfZ);
                if(nResult)
                {
                    if(bReadOnly)
                    {
                        GDALClose(m_poDataset);
                        m_poDataset = (GDALDataset *) GDALOpenShared( m_sPath, GA_Update );
                    }

                    GDAL_GCP *pasGCPList = (GDAL_GCP *) CPLCalloc(sizeof(GDAL_GCP), 4);
                    GDALInitGCPs( 4, pasGCPList );
                    char *pszProjection = NULL;

                    if(m_poDataset)
                    {
                        if( oDstOGRSpatialReference.exportToWkt( &pszProjection ) == OGRERR_NONE )
                        {
                            m_poDataset->SetProjection(pszProjection);
                        }

                        m_nXSize = m_poDataset->GetRasterXSize();
                        m_nYSize = m_poDataset->GetRasterYSize();

                        pasGCPList[0].pszId = CPLStrdup( "1" );
                        pasGCPList[0].dfGCPX = adfX[0];
                        pasGCPList[0].dfGCPY = adfY[0];
                        pasGCPList[0].dfGCPZ = adfZ[0];
                        pasGCPList[0].dfGCPLine = 0.5;
                        pasGCPList[0].dfGCPPixel = 0.5;

                        pasGCPList[1].pszId = CPLStrdup( "2" );
                        pasGCPList[1].dfGCPX = adfX[1];
                        pasGCPList[1].dfGCPY = adfY[1];
                        pasGCPList[1].dfGCPZ = adfZ[1];
                        pasGCPList[1].dfGCPLine = 0.5;
                        pasGCPList[1].dfGCPPixel = m_nXSize - 0.5;

                        pasGCPList[2].pszId = CPLStrdup( "3" );
                        pasGCPList[2].dfGCPX = adfX[2];
                        pasGCPList[2].dfGCPY = adfY[2];
                        pasGCPList[2].dfGCPZ = adfZ[2];
                        pasGCPList[2].dfGCPLine = m_nYSize - 0.5;
                        pasGCPList[2].dfGCPPixel = m_nXSize - 0.5;

                        pasGCPList[3].pszId = CPLStrdup( "4" );
                        pasGCPList[3].dfGCPX = adfX[3];
                        pasGCPList[3].dfGCPY = adfY[3];
                        pasGCPList[3].dfGCPZ = adfZ[3];
                        pasGCPList[3].dfGCPLine = m_nYSize - 0.5;
                        pasGCPList[3].dfGCPPixel = 0.5;

                        CPLErr eErr = m_poDataset->SetGCPs(4, pasGCPList, pszProjection);
                        //nResult = GDALGCPsToGeoTransform(4, pasGCPList, adfGeoTransform, 1);
                        //if(nResult)
                        //    CPLErr eErr = m_poDataset->SetGeoTransform(adfGeoTransform);
                    }

                    if(bReadOnly)
                    {
                        GDALClose(m_poDataset);
                        m_poDataset = (GDALDataset *) GDALOpenShared( m_sPath, GA_ReadOnly );
                    }

                    GDALDeinitGCPs( 4, pasGCPList );

                    CPLFree( pasGCPList );
                    CPLFree( pszProjection );
                }
                OCTDestroyCoordinateTransformation(poCT);
            }
         }

        bHasGeoTransform = true;
        if ((adfGeoTransform[2] != 0.0 || adfGeoTransform[4] != 0.0 ) || m_poDataset->GetGCPCount() > 0)// adfGeoTransform[1] < 0.0 || adfGeoTransform[5] > 0.0 ||
        {
            bHasGeoTransform = false;
            m_poMainDataset = m_poDataset;
            m_poDataset = (GDALDataset *) GDALAutoCreateWarpedVRT( m_poMainDataset, NULL, NULL, GRA_NearestNeighbour, 0.3, NULL );//TODO: get GRA_NearestNeighbour, 0.3 from config
            m_poMainDataset->Dereference();
            if(m_poDataset == NULL)
            {
                m_poDataset = m_poMainDataset;
                m_poMainDataset = NULL;
            }
        }
    }

	m_nXSize = m_poDataset->GetRasterXSize();
	m_nYSize = m_poDataset->GetRasterYSize();

    GDALRasterBand* pBand(NULL);
    if(m_poMainDataset)
        pBand = m_poMainDataset->GetRasterBand(1);
    else if(m_poDataset)
        pBand = m_poDataset->GetRasterBand(1);
    if(!pBand)
    {
		wxLogError(_("Raster open failed! Path '%s'. Raster has no bands"), m_sPath.c_str());
		return false;
    }
    if(pBand)
    {
        if(pBand->GetOverviewCount() > 0)
            m_bHasOverviews = true;

        double dfMin, dfMax, dfMean, dfStdDev;
        CPLErr eErr = pBand->GetStatistics(FALSE, FALSE, &dfMin, &dfMax, &dfMean, &dfStdDev );
        if( eErr == CE_None )
            m_bHasStats =  true;

		m_nDataType = pBand->GetRasterDataType(); //Assumed that raster have the same datatype in all other bands
    }

    if(m_nXSize < 2000 && m_nYSize < 2000)//for small rasters there is no need to create pyramids
        m_bHasOverviews = true;

    m_nBandCount = m_poDataset->GetRasterCount();

    if(!bHasGeoTransform)
    {
        CPLErr err = m_poDataset->GetGeoTransform(adfGeoTransform);
        if(err != CE_Fatal)
            bHasGeoTransform = true;
    }

	if(bHasGeoTransform)
	{
		double inX[4];
		double inY[4];

		inX[0] = 0;
		inY[0] = 0;
		inX[1] = m_nXSize;
		inY[1] = 0;
		inX[2] = m_nXSize;
		inY[2] = m_nYSize;
		inX[3] = 0;
		inY[3] = m_nYSize;

		m_stExtent.MaxX = -1000000000;
		m_stExtent.MaxY = -1000000000;
		m_stExtent.MinX = 1000000000;
		m_stExtent.MinY = 1000000000;
		for(int i = 0; i < 4; ++i)
		{
			double rX, rY;
			GDALApplyGeoTransform( adfGeoTransform, inX[i], inY[i], &rX, &rY );
			if(m_stExtent.MaxX < rX)
				m_stExtent.MaxX = rX;
			if(m_stExtent.MinX > rX)
				m_stExtent.MinX = rX;
			if(m_stExtent.MaxY < rY)
				m_stExtent.MaxY = rY;
			if(m_stExtent.MinY > rY)
				m_stExtent.MinY = rY;
		}
	}
	else
	{
		m_stExtent.MaxX = m_nXSize;
		m_stExtent.MaxY = m_nYSize;
		m_stExtent.MinX = 0;
		m_stExtent.MinY = 0;
	}

    //nodata check
    m_paNoData = new double[m_nBandCount];
    for(size_t nBand = 0; nBand < m_nBandCount; ++nBand)
    {
        GDALRasterBand* pBand;
        if(m_poMainDataset)
            pBand = m_poMainDataset->GetRasterBand(nBand + 1);
        else
            pBand = m_poDataset->GetRasterBand(nBand + 1);
        if(!pBand)
            continue;

        int bGotNodata;
	    m_paNoData[nBand] = pBand->GetNoDataValue(&bGotNodata );
        if(bGotNodata)
            continue;
        else
            m_paNoData[nBand] = NOTNODATA;

        switch((wxGISEnumRasterDatasetType)m_nSubType)
        {
        case enumRasterGif:
            {
		        char** papszMetadata = pBand->GetMetadata();
                const char* pszBk = CSLFetchNameValue(papszMetadata, "GIF_BACKGROUND");
                if(pszBk)
                    m_paNoData[nBand] = atof(pszBk);
            }
            break;
        };
    }

	m_bIsOpened = true;
    m_bIsReadOnly = bReadOnly;
	return true;
}

const wxGISSpatialReference wxGISRasterDataset::GetSpatialReference(void)
{
	if(m_SpatialReference.IsOk() || !IsOpened())
		return m_SpatialReference;

	if(	m_poDataset )
	{
        const char* pszProjection = m_poDataset->GetProjectionRef();
        if(CPLStrnlen(pszProjection, 10) > 0)
            m_SpatialReference = wxGISSpatialReference(new OGRSpatialReference(pszProjection));
	}

    return m_SpatialReference;
}

OGREnvelope wxGISRasterDataset::GetEnvelope(void)
{
	return m_stExtent;
}

GDALDataType wxGISRasterDataset::GetDataType(void)
{
	return m_nDataType;
}

wxString wxGISRasterDataset::GetName(void)
{
    if(!IsOpened())
        return wxEmptyString;

    wxString sOut = GetConvName(CPLGetFilename(m_sPath), false);
	if(sOut.IsEmpty())//CPLGetBasename
        sOut = GetConvName(CPLGetBasename(m_sPath), false);
	return sOut;
}

bool wxGISRasterDataset::Copy(const CPLString &szDestPath, ITrackCancel* const pTrackCancel)
{
	wxCriticalSectionLocker locker(m_CritSect);

    Close();

    char** papszFileList = GetFileList();
    papszFileList = CSLAddString( papszFileList, m_sPath );
    if(!papszFileList)    
    {
        if(pTrackCancel)
            pTrackCancel->PutMessage(_("No files to copy"), wxNOT_FOUND, enumGISMessageErr);
        return false;
    }

    CPLString szCopyFileName;	
    CPLString szFileName = GetUniqPath(m_sPath, szDestPath, CPLGetBasename(m_sPath));//CPLString szFileName = CPLGetBasename(m_sPath);

    char** papszFileCopiedList = NULL;

    for(int i = 0; papszFileList[i] != NULL; ++i )
    {
        CPLString szNewDestFileName(CPLFormFilename(szDestPath, szFileName, GetExtension(papszFileList[i], szFileName)));	//CPLString szNewDestFileName = GetUniqPath(papszFileList[i], szDestPath, szFileName);
        papszFileCopiedList = CSLAddString(papszFileCopiedList, szNewDestFileName);
        szCopyFileName = szNewDestFileName;
        if(!CopyFile(szNewDestFileName, papszFileList[i], pTrackCancel))
		{
            // Try to put the ones we moved back.
            for( --i; i >= 0; i-- )
                DeleteFile( papszFileCopiedList[i] );

			CSLDestroy( papszFileList );
			CSLDestroy( papszFileCopiedList );
            return false;
		}
    }

    bool bRet = true;
    switch(m_nSubType)
    {
	case enumRasterSAGA://write some internal info to files (e.g. sgrd mgrd -> new file name
        bRet = FixSAGARaster(szDestPath, szCopyFileName);
		break;
    default:
		break;
    };

	CSLDestroy( papszFileList );
	CSLDestroy( papszFileCopiedList );
	return bRet;
}

bool wxGISRasterDataset::FixSAGARaster(const CPLString &szDestPath, const CPLString &szDestName)
{
    bool bRet = true;
    CPLString sMGRDFileName(CPLFormFilename(szDestPath, CPLGetFilename(szDestName), "mgrd"));
    CPLXMLNode *psXMLRoot = CPLParseXMLFile(sMGRDFileName);
    if(psXMLRoot)
    {
        CPLXMLNode *pNode = psXMLRoot->psNext;//SAGA_METADATA
        bRet = CPLSetXMLValue(pNode, "SOURCE.FILE", CPLFormFilename(szDestPath, CPLGetFilename(szDestName), "sgrd"));
        if(bRet)
            bRet = CPLSerializeXMLTreeToFile(psXMLRoot, sMGRDFileName);
        CPLDestroyXMLNode( psXMLRoot );
    }

    CPLString sSGRDFileName(CPLFormFilename(szDestPath, szDestName, "sgrd"));
    char** papszNameValues = CSLLoad(sSGRDFileName);
    papszNameValues[0] = (char*)CPLSPrintf("NAME\t= %s", szDestName.c_str() );
    bRet = CSLSave(papszNameValues, sSGRDFileName);

    CSLDestroy(papszNameValues);

    return bRet;
}

bool wxGISRasterDataset::Move(const CPLString &szDestPath, ITrackCancel* const pTrackCancel)
{
    if(wxGISDataset::Move(szDestPath, pTrackCancel))
    {
        bool bRet = true;
        switch(m_nSubType)
        {
	    case enumRasterSAGA://write some internal info to files (e.g. sgrd mgrd -> new file name
            bRet = FixSAGARaster(szDestPath, m_sPath);
		    break;
        default:
		    break;
        };
        return bRet;
    }
    else
        return false;
}

bool wxGISRasterDataset::GetPixelData(void *data, int nXOff, int nYOff, int nXSize, int nYSize, int nBufXSize, int nBufYSize, GDALDataType eDT, int nBandCount, int *panBandList)
{
    CPLErrorReset();

    CPLErr err = m_poDataset->AdviseRead(nXOff, nYOff, nXSize, nYSize, nBufXSize, nBufYSize, eDT, nBandCount, panBandList, NULL);
    if(err != CE_None)
    {
        const char* pszerr = CPLGetLastErrorMsg();
		wxLogError(_("AdviseRead failed! GDAL error: %s"), wxString(pszerr, wxConvUTF8).c_str());
        return false;
    }

    int nPixelSpace(0);
    int nLineSpace(0);
	int nBandSpace(0);
	if(nBandCount > 1)
	{
		int nDataSize = GDALGetDataTypeSize(eDT) * 0.125;//same as /8
		nPixelSpace = nDataSize * nBandCount;
		nLineSpace = nBufXSize * nPixelSpace;
		nBandSpace = nDataSize;
	}
    err = m_poDataset->RasterIO(GF_Read, nXOff, nYOff, nXSize, nYSize, data, nBufXSize, nBufYSize, eDT, nBandCount, panBandList, nPixelSpace, nLineSpace, nBandSpace);
    if(err != CE_None)
    {
        const char* pszerr = CPLGetLastErrorMsg();
        wxLogError(_("RasterIO failed! GDAL error: %s"), wxString(pszerr, wxConvUTF8).c_str());
        return false;
    }
	return true;
}

bool wxGISRasterDataset::WriteWorldFile(wxGISEnumWldExtType eType)
{
    CPLString sExt = CPLGetExtension(m_sPath);
    CPLString sNewExt;

	switch(eType)
	{
	case enumGISWldExt_FirstLastW:
		sNewExt += sExt[0];
		sNewExt += sExt[sExt.size() - 1];
		sNewExt += 'w';
		break;
	case enumGISWldExt_ExtPlusWX:
		sNewExt += sExt[0];
		sNewExt += sExt[sExt.size() - 1];
		sNewExt += 'w';
		sNewExt += 'x';
		break;
	case enumGISWldExt_Wld:
		sNewExt.append("wld");
		break;
	case enumGISWldExt_ExtPlusW:
		sNewExt = sExt + CPLString("w");
		break;
	};
    double adfGeoTransform[6] = { 0, 0, 0, 0, 0, 0 };
	if(m_poDataset->GetGeoTransform(adfGeoTransform) != CE_None)
    {
        const char* pszerr = CPLGetLastErrorMsg();
        wxLogError(_("GetGeoTransform failed! GDAL error: %s"), wxString(pszerr, wxConvUTF8).c_str());
		return false;
    }
	return GDALWriteWorldFile(m_sPath, sNewExt, adfGeoTransform);
}
