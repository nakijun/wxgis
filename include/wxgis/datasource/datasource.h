/******************************************************************************
 * Project:  wxGIS
 * Purpose:  datasource header.
 * Author:   Baryshnikov Dmitriy (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2009-2012 Bishop
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

#define NOTNODATA -9999.0

/** \enum wxGISEnumDatasetType
    \brief The dataset types
*/

enum wxGISEnumDatasetType
{
    enumGISAny = 0,
	enumGISFeatureDataset = 1,
	enumGISTableDataset = 2,
	enumGISRasterDataset = 3,
	enumGISContainer = 4
};

/** \enum wxGISEnumVectorDatasetType
    \brief The vector datasource types
*/
enum wxGISEnumVectorDatasetType
{
	enumVecUnknown = 0,
	enumVecESRIShapefile,
	enumVecMapinfoTab,
    enumVecMapinfoMif,
    enumVecKML,
    enumVecKMZ,
    enumVecDXF,
	emumVecPostGIS,
	enumVecGML,
    enumVecGeoJSON,
    enumVecGeoWFS,
    emumVecMAX
};

/** \enum wxGISEnumRasterDatasetType
    \brief The raster datasource types
*/
enum wxGISEnumRasterDatasetType
{
	enumRasterUnknown = 0,
	enumRasterBmp,
	enumRasterTiff,
	enumRasterTil,
	enumRasterImg,
	enumRasterJpeg,
	enumRasterPng,
    enumRasterGif,
    enumRasterSAGA,
    enumRasterVRT,
    enumRasterWMS,
    enumRasterWMSTMS,
    enumRasterMAX
};

/** \enum wxGISEnumTableDatasetType
    \brief The table datasource types
*/
enum wxGISEnumTableDatasetType
{
	enumTableUnknown = 0,
    enumTableDBF,
	enumTablePostgres,
    enumTableQueryResult,
    enumTableMapinfoTab,
    enumTableMapinfoMif,
    enumTableCSV,
    emumTableMAX
};

/** \enum wxGISEnumPrjFileType
    \brief The projection file types
*/
enum wxGISEnumPrjFileType
{
	enumESRIPrjFile = 1,
	enumSPRfile = 2
};

/** \enum wxGISEnumContainerType
    \brief The container types
*/
enum wxGISEnumContainerType
{
    enumContUnknown = 0,
	enumContFolder,
	enumContGDBFolder,
    enumContDataset,
	enumContGDB,
	enumContRemoteConnection
};

/** \enum wxGISEnumWldExtType
    \brief The world file ext type
*/

enum wxGISEnumWldExtType
{
	enumGISWldExt_FirstLastW = 1,
	enumGISWldExt_Wld = 2,
	enumGISWldExt_ExtPlusW = 3,
	enumGISWldExt_ExtPlusWX = 4
};



