/******************************************************************************
 * Project:  wxGIS (GIS Toolbox)
 * Purpose:  ortho correct geoprocessing tools.
 * Author:   Bishop (aka Barishnikov Dmitriy), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2010-2011 Bishop
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

#include "wxgis/geoprocessing/gporthocorrecttool.h"
#include "wxgis/geoprocessing/gptoolmngr.h"
#include "wxgis/geoprocessing/gpdomain.h"
#include "wxgis/geoprocessing/gpparam.h"
#include "wxgis/catalog/gxfilters.h"
#include "wxgis/catalog/catop.h"
#include "wxgis/datasource/rasterdataset.h"
#include "wxgis/framework/application.h"

/////////////////////////////////////////////////////////////////////////
// wxGISGPOrthoCorrectTool
/////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNAMIC_CLASS(wxGISGPOrthoCorrectTool, wxGISGPTool)

wxGISGPOrthoCorrectTool::wxGISGPOrthoCorrectTool(void) : wxGISGPTool()
{
}

wxGISGPOrthoCorrectTool::~wxGISGPOrthoCorrectTool(void)
{
}

const wxString wxGISGPOrthoCorrectTool::GetDisplayName(void)
{
    return wxString(_("Create Ortho-Corrected Raster"));
}

const wxString wxGISGPOrthoCorrectTool::GetName(void)
{
    return wxString(wxT("create_ortho"));
}

const wxString wxGISGPOrthoCorrectTool::GetCategory(void)
{
    return wxString(_("Data Management Tools/Raster"));
}

GPParameters wxGISGPOrthoCorrectTool::GetParameterInfo(void)
{
    if(m_paParam.IsEmpty())
    {
        //src path
        wxGISGPParameter* pParam1 = new wxGISGPParameter();
        pParam1->SetName(wxT("src_path"));
        pParam1->SetDisplayName(_("Source raster"));
        pParam1->SetParameterType(enumGISGPParameterTypeRequired);
        pParam1->SetDataType(enumGISGPParamDTPath);
        pParam1->SetDirection(enumGISGPParameterDirectionInput);

        wxGISGPGxObjectDomain* pDomain1 = new wxGISGPGxObjectDomain();
        pDomain1->AddFilter(new wxGxRasterFilter(enumRasterTiff));
        pDomain1->AddFilter(new wxGxRasterFilter(enumRasterTil));
        pParam1->SetDomain(pDomain1);

        m_paParam.Add(static_cast<IGPParameter*>(pParam1));

        //dst path
        wxGISGPParameter* pParam2 = new wxGISGPParameter();
        pParam2->SetName(wxT("dst_path"));
        pParam2->SetDisplayName(_("Destination raster"));
        pParam2->SetParameterType(enumGISGPParameterTypeRequired);
        pParam2->SetDataType(enumGISGPParamDTPath);
        pParam2->SetDirection(enumGISGPParameterDirectionOutput);

        wxGISGPGxObjectDomain* pDomain2 = new wxGISGPGxObjectDomain();
        AddAllRasterFilters(pDomain2);
        pParam2->SetDomain(pDomain2);

        //pParam2->AddParameterDependency(wxT("src_path"));

        m_paParam.Add(static_cast<IGPParameter*>(pParam2));

        //DEM_raster
        wxGISGPParameter* pParam3 = new wxGISGPParameter();
        pParam3->SetName(wxT("dem_path"));
        pParam3->SetDisplayName(_("DEM raster"));
        pParam3->SetParameterType(enumGISGPParameterTypeOptional);
        pParam3->SetDataType(enumGISGPParamDTPath);
        pParam3->SetDirection(enumGISGPParameterDirectionInput);

        wxGISGPGxObjectDomain* pDomain3 = new wxGISGPGxObjectDomain();
        pDomain3->AddFilter(new wxGxDatasetFilter(enumGISRasterDataset));
        pParam3->SetDomain(pDomain3);

        m_paParam.Add(static_cast<IGPParameter*>(pParam3));

        //constant_ elevation double
        wxGISGPParameter* pParam4 = new wxGISGPParameter();
        pParam4->SetName(wxT("cons_elev"));
        pParam4->SetDisplayName(_("Constant elevation value or base value to the DEM"));
        pParam4->SetParameterType(enumGISGPParameterTypeOptional);
        pParam4->SetDataType(enumGISGPParamDTDouble);
        pParam4->SetDirection(enumGISGPParameterDirectionInput);
        pParam4->SetValue(0.0);

        m_paParam.Add(static_cast<IGPParameter*>(pParam4));

        //constant_ elevation double
        wxGISGPParameter* pParam5 = new wxGISGPParameter();
        pParam5->SetName(wxT("elev_scale"));
        pParam5->SetDisplayName(_("Scaling factor to convert elevation values"));
        pParam5->SetParameterType(enumGISGPParameterTypeOptional);
        pParam5->SetDataType(enumGISGPParamDTDouble);
        pParam5->SetDirection(enumGISGPParameterDirectionInput);
        pParam5->SetValue(1.0);

        m_paParam.Add(static_cast<IGPParameter*>(pParam5));

        //elevation interpolation type
        wxGISGPParameter* pParam6 = new wxGISGPParameter();
        pParam6->SetName(wxT("elev_interpol"));
        pParam6->SetDisplayName(_("Elevation values interpolation"));
        pParam6->SetParameterType(enumGISGPParameterTypeOptional);
        pParam6->SetDataType(enumGISGPParamDTStringChoice);
        pParam6->SetDirection(enumGISGPParameterDirectionInput);

        wxGISGPStringDomain* pDomain6 = new wxGISGPStringDomain();
        pDomain6->AddString(_("Bilinear"));
        pDomain6->AddString(_("Cubic"));
        pParam6->SetDomain(pDomain6);

        pParam6->SetValue(_("Bilinear"));

        m_paParam.Add(static_cast<IGPParameter*>(pParam6));
    }
    return m_paParam;
}

bool wxGISGPOrthoCorrectTool::Validate(void)
{
    if(!m_paParam[1]->GetAltered())
    {
        if(m_paParam[0]->GetIsValid())
        {
            //generate temp name
            wxString sPath = m_paParam[0]->GetValue();
            wxFileName Name(sPath);
            Name.SetName(Name.GetName() + wxT("_ortho"));
            m_paParam[1]->SetValue(wxVariant(Name.GetFullPath(), wxT("path")));
            m_paParam[1]->SetAltered(true);//??
        }
    }
    return true;
}

bool wxGISGPOrthoCorrectTool::Execute(ITrackCancel* pTrackCancel)
{
    if(!Validate())
    {
        //add messages to pTrackCancel
        if(pTrackCancel)
            pTrackCancel->PutMessage(_("Unexpected error occurred"), -1, enumGISMessageErr);
        return false;
    }

    IGxObjectContainer* pGxObjectContainer = dynamic_cast<IGxObjectContainer*>(m_pCatalog);
    if(!pGxObjectContainer)
    {
        //add messages to pTrackCancel
        if(pTrackCancel)
            pTrackCancel->PutMessage(_("Error getting catalog object"), -1, enumGISMessageErr);
        return false;
    }

    wxString sSrcPath = m_paParam[0]->GetValue();
    IGxObject* pGxObject = pGxObjectContainer->SearchChild(sSrcPath);
    if(!pGxObject)
    {
        //add messages to pTrackCancel
        if(pTrackCancel)
            pTrackCancel->PutMessage(_("Error getting source object"), -1, enumGISMessageErr);
        return false;
    }
    IGxDataset* pGxDataset = dynamic_cast<IGxDataset*>(pGxObject);
    if(!pGxDataset)
    {
        //add messages to pTrackCancel
        if(pTrackCancel)
            pTrackCancel->PutMessage(_("Source object is of incompatible type"), -1, enumGISMessageErr);
        return false;
    }
    wxGISRasterDatasetSPtr pSrcDataSet = boost::dynamic_pointer_cast<wxGISRasterDataset>(pGxDataset->GetDataset());
    if(!pSrcDataSet)
    {
        //add messages to pTrackCancel
        if(pTrackCancel)
            pTrackCancel->PutMessage(_("Source dataset is of incompatible type"), -1, enumGISMessageErr);
        return false;
    }
	if(!pSrcDataSet->IsOpened())
		if(!pSrcDataSet->Open(true))
			return false;

    wxString sDstPath = m_paParam[1]->GetValue();

	//check overwrite & do it!
	if(!OverWriteGxObject(pGxObjectContainer->SearchChild(sDstPath), pTrackCancel))
		return false;

    wxFileName sDstFileName(sDstPath);
    wxString sPath = sDstFileName.GetPath();
    IGxObject* pGxDstObject = pGxObjectContainer->SearchChild(sPath);
    if(!pGxDstObject)
    {
        //add messages to pTrackCancel
        if(pTrackCancel)
            pTrackCancel->PutMessage(_("Error get destination object"), -1, enumGISMessageErr);
        return false;
    }

    CPLString szPath = pGxDstObject->GetInternalName();
    wxString sName = sDstFileName.GetName();

    wxGISGPGxObjectDomain* pDomain = dynamic_cast<wxGISGPGxObjectDomain*>(m_paParam[1]->GetDomain());
	IGxObjectFilter* pFilter = pDomain->GetFilter(m_paParam[1]->GetSelDomainValue());
    if(!pFilter)
    {
        //add messages to pTrackCancel
        if(pTrackCancel)
            pTrackCancel->PutMessage(_("Error getting selected destination filter"), -1, enumGISMessageErr);
        return false;
    }

    wxString sDriver = pFilter->GetDriver();
    wxString sExt = pFilter->GetExt();
    int nNewSubType = pFilter->GetSubType();

    GDALDataset* poGDALDataset = pSrcDataSet->GetRaster();
    if(!poGDALDataset)
    {
        //add messages to pTrackCancel
        if(pTrackCancel)
            pTrackCancel->PutMessage(_("Error getting raster"), -1, enumGISMessageErr);
        return false;
    }

	GDALDriver* poDriver = (GDALDriver*)GDALGetDriverByName( sDriver.mb_str() );
    GDALRasterBand * poGDALRasterBand = poGDALDataset->GetRasterBand(1);

    if(!poGDALRasterBand)
    {
        //add messages to pTrackCancel
        if(pTrackCancel)
            pTrackCancel->PutMessage(_("The raster has no bands"), -1, enumGISMessageErr);
        return false;
    }
    GDALDataType eDT = poGDALRasterBand->GetRasterDataType();

    wxString soChoice = m_paParam[5]->GetValue();
    if(soChoice == wxString(_("Cubic")))
        CPLSetConfigOption( "GDAL_RPCDEMINTERPOLATION", "CUBIC" ); //BILINEAR


    //CPLString osSRCSRSOpt = "SRC_SRS=";
    //osSRCSRSOpt += poGDALDataset->GetProjectionRef();
    CPLString osDSTSRSOpt = "DST_SRS=";
    osDSTSRSOpt += poGDALDataset->GetProjectionRef();

    const char *apszOptions[6] = { osDSTSRSOpt.c_str(), "METHOD=RPC", NULL, NULL, NULL, NULL};//, NULL  osSRCSRSOpt.c_str(),
    wxString soDEMPath = m_paParam[2]->GetValue();

    CPLString soCPLDemPath;
    if(pGxObjectContainer)
    {
        IGxObject* pGxDemObj = pGxObjectContainer->SearchChild(soDEMPath);
        if(pGxDemObj)
        {
            soCPLDemPath = pGxDemObj->GetInternalName();
        }
    }

    CPLString osDEMFileOpt = "RPC_DEM=";
    osDEMFileOpt += soCPLDemPath;
    apszOptions[2] = osDEMFileOpt.c_str();

    wxString soHeight = m_paParam[3]->GetValue();
    CPLString osHeightOpt = "RPC_HEIGHT=";
    osHeightOpt += soHeight.mb_str();
    apszOptions[3] = osHeightOpt.c_str();

    wxString soHeightScale = m_paParam[4]->GetValue();
    CPLString osHeightScaleOpt = "RPC_HEIGHT_SCALE=";
	osHeightScaleOpt += soHeightScale.mb_str();
    apszOptions[4] = osHeightScaleOpt.c_str();

    //double dfPixErrThreshold = MIN(adfDstGeoTransform[1], adfDstGeoTransform[5]);

    void *hTransformArg = GDALCreateGenImgProjTransformer2( poGDALDataset, NULL, (char **)apszOptions );
    if(!hTransformArg)
    {
        const char* pszErr = CPLGetLastErrorMsg();
        if(pTrackCancel)
            pTrackCancel->PutMessage(wxString::Format(_("Error CreateGenImgProjTransformer. GDAL Error: %s"), wxString(pszErr, wxConvUTF8).c_str()), -1, enumGISMessageErr);
        return false;
    }

    double adfDstGeoTransform[6] = {0,0,0,0,0,0};
    int nPixels=0, nLines=0;

    CPLErr eErr = GDALSuggestedWarpOutput( poGDALDataset, GDALGenImgProjTransform, hTransformArg, adfDstGeoTransform, &nPixels, &nLines );
    if(eErr != CE_None)
    {
        const char* pszErr = CPLGetLastErrorMsg();
        if(pTrackCancel)
            pTrackCancel->PutMessage(wxString::Format(_("Error determining output raster size. GDAL Error: %s"), wxString(pszErr, wxConvUTF8).c_str()), -1, enumGISMessageErr);
        return false;
    }


    GDALDestroyGenImgProjTransformer( hTransformArg );

    // Create the output file.
	CPLString sFullPath = CPLFormFilename(szPath, sName.mb_str(wxConvUTF8), sExt.mb_str(wxConvUTF8));
    GDALDataset * poOutputGDALDataset = poDriver->Create( sFullPath, nPixels, nLines, poGDALDataset->GetRasterCount(), eDT, NULL );
    if(poOutputGDALDataset == NULL)
    {
        const char* pszErr = CPLGetLastErrorMsg();
        if(pTrackCancel)
            pTrackCancel->PutMessage(wxString::Format(_("Error creating output raster. GDAL Error: %s"), wxString(pszErr, wxConvUTF8).c_str()), -1, enumGISMessageErr);
        return false;
    }

    poOutputGDALDataset->SetProjection(poGDALDataset->GetProjectionRef());
    poOutputGDALDataset->SetGeoTransform( adfDstGeoTransform );

    // Copy the color table, if required.
    GDALColorTableH hCT;

    hCT = GDALGetRasterColorTable( GDALGetRasterBand(poGDALDataset,1) );
    if( hCT != NULL )
        GDALSetRasterColorTable( GDALGetRasterBand(poOutputGDALDataset,1), hCT );


    // Setup warp options.

    GDALWarpOptions *psWarpOptions = GDALCreateWarpOptions();

    psWarpOptions->hSrcDS = poGDALDataset;
    psWarpOptions->hDstDS = poOutputGDALDataset;

    //psWarpOptions->nBandCount = 1;
    //psWarpOptions->panSrcBands =
    //    (int *) CPLMalloc(sizeof(int) * psWarpOptions->nBandCount );
    //psWarpOptions->panSrcBands[0] = 1;
    //psWarpOptions->panDstBands =
    //    (int *) CPLMalloc(sizeof(int) * psWarpOptions->nBandCount );
    //psWarpOptions->panDstBands[0] = 1;

    psWarpOptions->pfnProgress = ExecToolProgress;
    psWarpOptions->pProgressArg = (void*)pTrackCancel;

    // Establish reprojection transformer.

    psWarpOptions->pTransformerArg = GDALCreateGenImgProjTransformer2( poGDALDataset, poOutputGDALDataset, (char **)apszOptions );
    psWarpOptions->pfnTransformer = GDALGenImgProjTransform;
    
    //TODO: Add to config memory limit in % of free memory
    double dfMemLim = wxMemorySize(wxGetFreeMemory() / wxThread::GetCPUCount()).ToDouble();
    if(dfMemLim > 135000000) //128Mb in bytes
    {
        psWarpOptions->dfWarpMemoryLimit = dfMemLim;
        wxLogDebug(wxT("wxGISGPOrthoCorrectTool: The dfWarpMemoryLimit set to %f Mb"), dfMemLim / 1048576);
    }

    psWarpOptions->papszWarpOptions = CSLSetNameValue(psWarpOptions->papszWarpOptions, "SOURCE_EXTRA", "5" );
    psWarpOptions->papszWarpOptions = CSLSetNameValue(psWarpOptions->papszWarpOptions, "SAMPLE_STEPS", "101" );
    psWarpOptions->eResampleAlg = GRA_Bilinear;

    // Initialize and execute the warp operation.

    GDALWarpOperation oOperation;

    oOperation.Initialize( psWarpOptions );
    eErr = oOperation.ChunkAndWarpImage( 0, 0, nPixels, nLines );//ChunkAndWarpMulti( 0, 0, nPixels, nLines );//
    if(eErr != CE_None)
    {
        const char* pszErr = CPLGetLastErrorMsg();
        if(pTrackCancel)
        {
            pTrackCancel->PutMessage(wxString::Format(_("OrthoCorrect failed! GDAL error: %s"), wxString(pszErr, wxConvUTF8).c_str()), -1, enumGISMessageErr);
        }
        GDALClose(poOutputGDALDataset);
        //wsDELETE(pSrcDataSet);
        return false;
    }

    GDALDestroyGenImgProjTransformer( psWarpOptions->pTransformerArg );
    GDALDestroyWarpOptions( psWarpOptions );

    GDALClose(poOutputGDALDataset);
    //wsDELETE(pSrcDataSet);

    if(pGxObjectContainer)
    {
        IGxObject* pParentLoc = pGxObjectContainer->SearchChild(sPath);
        if(pParentLoc)
            pParentLoc->Refresh();
    }

    return true;
}

