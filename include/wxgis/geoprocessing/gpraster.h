/******************************************************************************
 * Project:  wxGIS (GIS Toolbox)
 * Purpose:  raster dataset functions.
 * Author:   Bishop (aka Barishnikov Dmitriy), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2011 Bishop
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

#include "wxgis/geoprocessing/geoprocessing.h"
#include "wxgis/datasource/featuredataset.h"
#include "wxgis/datasource/rasterdataset.h"

/** \fn bool SubrasterByVector(wxGISFeatureDatasetSPtr pSrcFeatureDataSet, wxGISRasterDatasetSPtr pSrcRasterDataSet, CPLString &szDstFolderPath)
 *  \brief Get subruster clipped by vector geometry.
 *  \param pSrcFeatureDataSet The clip geometry source dataset
 *  \param pSrcRasterDataSet The clipped raster
 *  \param szDstFolderPath The subraster store path
 *  \return False if any error or true
 */	
bool WXDLLIMPEXP_GIS_DS SubrasterByVector(wxGISFeatureDatasetSPtr pSrcFeatureDataSet, wxGISRasterDatasetSPtr pSrcRasterDataSet, CPLString &szDstFolderPath, ITrackCancel* pTrackCancel = NULL);

GDALDataset* CreateOutputDataset();