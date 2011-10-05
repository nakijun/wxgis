/******************************************************************************
 * Project:  wxGIS
 * Purpose:  wxGISRasterRGBARenderer classes.
 * Author:   Bishop (aka Barishnikov Dmitriy), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2009,2011 Bishop
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

#include "wxgis/carto/carto.h"
#include "wxgis/carto/stretch.h"

/** \class wxGISRasterRGBARenderer rasterrenderer.h
    \brief The raster layer renderer for RGB data and Alpha channel
*/
class wxRasterDrawThread : public wxThread
{
public:
	wxRasterDrawThread(RAWPIXELDATA &stPixelData, GDALDataType eSrcType, int nBandCount, unsigned char *pTransformData, wxGISEnumDrawQuality nQuality, int nOutXSize, int nOutYSize, int nBegY, int nEndY, IRasterRenderer *pRasterRenderer, ITrackCancel *pTrackCancel = NULL);
    virtual void *Entry();
    virtual void OnExit();
private:
    ITrackCancel* m_pTrackCancel;
	IRasterRenderer *m_pRasterRenderer;
	RAWPIXELDATA &m_stPixelData;
	GDALDataType m_eSrcType;
	unsigned char *m_pTransformData;
	wxGISEnumDrawQuality m_nQuality;
	int m_nOutXSize;
	int m_nOutYSize;
	int m_nBegY;
	int m_nEndY;
	int m_nBandCount;
};

/** \class wxGISRasterRGBARenderer rasterrenderer.h
    \brief The raster layer renderer for RGB data and Alpha channel
*/
class wxGISRasterRGBARenderer :
	public IRasterRenderer
{
public:
	wxGISRasterRGBARenderer(void);
	~wxGISRasterRGBARenderer(void);
//IRasterRenderer
	virtual bool CanRender(wxGISDatasetSPtr pDataset);
	virtual void PutRaster(wxGISRasterDatasetSPtr pRaster);
	virtual int *GetBandsCombination(int *pnBandCount);
	virtual void Draw(RAWPIXELDATA &stPixelData, wxGISEnumDrawPhase DrawPhase, wxGISDisplay *pDisplay, ITrackCancel *pTrackCancel = NULL);
	virtual void FillPixel(unsigned char* pOutputData, void *pSrcValR, void *pSrcValG, void *pSrcValB, void *pSrcValA);
/**
    \brief Proceed two dim array multithreaded.
*/
protected:
	virtual bool OnPixelProceed(RAWPIXELDATA &stPixelData, GDALDataType eSrcType, unsigned char *pTransformData, ITrackCancel *pTrackCancel );
	virtual void OnFillStats(void);
protected:
	int m_nRedBand, m_nGreenBand, m_nBlueBand, m_nAlphaBand;
	wxColour m_oNoDataColor; //m_oBkColorGet, m_oBkColorSet, 
	//statistics - current display extent, each raster dataset, custom settings
	wxGISRasterDatasetSPtr m_pwxGISRasterDataset;
	wxGISEnumDrawQuality m_nQuality;
    wxGISStretch *m_paStretch[4];
	bool m_bNodataNewBehaviour;
};
