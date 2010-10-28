/******************************************************************************
 * Project:  wxGIS (GIS Toolbox)
 * Purpose:  geoprocessing tool parameters domains.
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

#pragma once

#include "wxgis/geoprocessing/geoprocessing.h"
#include "wxgis/catalog/catalog.h"

///////////////////////////////////////////////////////////////////////////////
/// Class wxGISGPGxObjectDomain
///////////////////////////////////////////////////////////////////////////////

class WXDLLIMPEXP_GIS_GP wxGISGPGxObjectDomain : public IGPDomain
{
public:
    wxGISGPGxObjectDomain (void);
    virtual ~wxGISGPGxObjectDomain (void);
	virtual void AddFilter(IGxObjectFilter* pFilter);
    virtual IGxObjectFilter* GetFilter(size_t nIndex);
    virtual size_t GetFilterCount(void);
    virtual void SetSelFilter(size_t nIndex);
    virtual size_t GetSelFilter(void);
protected:
	OBJECTFILTERS m_FilterArray;
    size_t m_nSelFilterIndex;
};


///////////////////////////////////////////////////////////////////////////////
/// Class wxGISGPStringDomain
///////////////////////////////////////////////////////////////////////////////

class WXDLLIMPEXP_GIS_GP wxGISGPStringDomain : public IGPDomain
{
public:
    wxGISGPStringDomain (void);
    virtual ~wxGISGPStringDomain (void);
	virtual void AddString(wxString soStr);
    virtual size_t GetStringCount(void);
    virtual wxString GetString(size_t dIndex);
    virtual wxArrayString GetArrayString() const;
    virtual void SetSelString(size_t nIndex);
    virtual size_t GetSelString(void);
protected:
	wxArrayString m_asoData;
    size_t m_nSelFilterIndex;
};