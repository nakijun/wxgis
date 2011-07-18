/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  RasterLayer header.
 * Author:   Bishop (aka Barishnikov Dmitriy), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2009  Bishop
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
#include "wxgis/carto/rasterlayer.h"
/*
#include "wxgis/carto/rasterrenderer.h"

wxGISRasterLayer::wxGISRasterLayer(wxGISDatasetSPtr pwxGISDataset) : wxGISLayer(), m_pRasterRenderer(NULL)
{
    m_pwxGISRasterDataset = boost::dynamic_pointer_cast<wxGISRasterDataset>(pwxGISDataset);
	if(m_pwxGISRasterDataset)
	{
		//m_pwxGISRasterDataset->Reference();
		//check number of bands
		m_pRasterRenderer = new wxGISRasterRGBRenderer();

		SetName(m_pwxGISRasterDataset->GetName());
	}
}

wxGISRasterLayer::~wxGISRasterLayer(void)
{
	wxDELETE(m_pRasterRenderer);
	//wsDELETE(m_pwxGISRasterDataset);
}

bool wxGISRasterLayer::Draw(wxGISEnumDrawPhase DrawPhase, ICachedDisplay* pDisplay, ITrackCancel* pTrackCancel)
{
	IDisplayTransformation* pDisplayTransformation = pDisplay->GetDisplayTransformation();
	if(!pDisplayTransformation)
		return;
	//1. get envelope
    OGREnvelope Env = pDisplayTransformation->GetVisibleBounds();
    const OGREnvelope* LayerEnv = m_pwxGISRasterDataset->GetEnvelope();
    OGRSpatialReference* pEnvSpaRef = pDisplayTransformation->GetSpatialReference();
    const OGRSpatialReferenceSPtr pLayerSpaRef = m_pwxGISRasterDataset->GetSpatialReference();

    if(pLayerSpaRef && pEnvSpaRef)
    {
        if(!pLayerSpaRef->IsSame(pEnvSpaRef))
        {
            OGRCoordinateTransformation *poCT = OGRCreateCoordinateTransformation( pEnvSpaRef, pLayerSpaRef.get() );
            poCT->Transform(1, &Env.MaxX, &Env.MaxY);
            poCT->Transform(1, &Env.MinX, &Env.MinY);
            OCTDestroyCoordinateTransformation(poCT);
        }
    }

	//2. set spatial filter
	pDisplay->StartDrawing(GetCacheID());
	if(m_pRasterRenderer && m_pRasterRenderer->CanRender(m_pwxGISRasterDataset))
	{
		m_pRasterRenderer->Draw(m_pwxGISRasterDataset, DrawPhase, pDisplay, pTrackCancel);
	////	wxGISFeatureSet* pGISFeatureSet(NULL);
	//	//3. get raster set
	//	wxGISSpatialFilter pFilter;
	//	pFilter.SetEnvelope(Env);
	////	pGISFeatureSet = m_pwxGISFeatureDataset->GetFeatureSet(&pFilter, pTrackCancel);
	//	wxImage Image = m_pwxGISRasterDataset->GetSubimage(pDisplayTransformation, &pFilter, pTrackCancel);
	//	if(Image.IsOk())
	//	{
	//		pDisplay->DrawBitmap(Image, 0, 0);
	//		//4. send it to renderer
	////	m_pFeatureRenderer->Draw(pGISFeatureSet, DrawPhase, pDisplay, pTrackCancel);
		m_pPreviousDisplayEnv = Env;
	//	}
	}
	//5. clear a spatial filter		
	pDisplay->FinishDrawing();
}

OGRSpatialReference* wxGISRasterLayer::GetSpatialReference(void)
{
	if(IsValid())
		return m_pwxGISRasterDataset->GetSpatialReference()->Clone();
	return NULL;
}

const OGREnvelope* wxGISRasterLayer::GetEnvelope(void)
{
	if(IsValid())
		return m_pwxGISRasterDataset->GetEnvelope();
	return NULL;
}

bool wxGISRasterLayer::IsValid(void)
{
	return m_pwxGISRasterDataset == NULL ? false : true;
}
*/
	//����������������� ������ ������ �������� ��� ��� �������� ������ �������. ����� ����� ������ ��������� ���������� � ������� - �������� �� ������������� in memory raster ��� ���� �����.
