/******************************************************************************
 * Project:  wxGIS
 * Purpose:  RasterLayer header.
 * Author:   Baryshnikov Dmitriy (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2013 Bishop
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

#include "wxgis/carto/renderer.h"

/** \class wxGISLayer layer.h
    \brief The base class for map layers.
*/

class WXDLLIMPEXP_GIS_CRT wxGISLayer :
	public wxObject
{
    DECLARE_ABSTRACT_CLASS(wxGISLayer)
public:
	wxGISLayer(const wxString &sName = _("new layer"), wxGISDataset* pwxGISDataset = NULL);
	virtual ~wxGISLayer(void);
	virtual wxGISSpatialReference GetSpatialReference(void) const;
	virtual void SetSpatialReference(const wxGISSpatialReference &SpatialReference);
	virtual OGREnvelope GetEnvelope(void) const;
	virtual void SetMaximumScale(double dMaxScale){m_dMaxScale = dMaxScale;};
	virtual double GetMaximumScale(void) const {return m_dMaxScale;};
	virtual void SetMinimumScale(double dMinScale){m_dMinScale = dMinScale;};
	virtual double GetMinimumScale(void) const {return m_dMinScale;};
	virtual bool GetVisible(void) const {return m_bVisible;};
	virtual void SetVisible(bool bVisible){m_bVisible = bVisible;};
	virtual void SetName(const wxString &sName){m_sName = sName;};
	virtual wxString GetName(void) const {return m_sName;};
	virtual size_t GetCacheID(void) const {return m_nCacheID;};
	virtual void SetCacheID(size_t nCacheID){m_nCacheID = nCacheID;};
	virtual bool IsCacheNeeded(void) const {return true;};
	virtual wxGISEnumDatasetType GetType(void) const {return enumGISAny;};
	virtual bool IsValid(void) const;
	virtual bool Draw(wxGISEnumDrawPhase DrawPhase, wxGISDisplay *pDisplay, ITrackCancel* const pTrackCancel = NULL) = 0;
    virtual void SetRenderer(wxGISRenderer* pRenderer);
    virtual wxGISRenderer* GetRenderer(void);
protected:
    wxGISDataset* m_pwxGISDataset;
    wxGISSpatialReference m_SpatialReference;
    OGREnvelope m_FullEnvelope;
    double m_dMaxScale, m_dMinScale;
    bool m_bVisible;
    wxString m_sName;
    size_t m_nCacheID;
    //renderer
    wxGISRenderer* m_pRenderer;
};
