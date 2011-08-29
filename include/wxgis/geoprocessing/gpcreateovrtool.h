/******************************************************************************
 * Project:  wxGIS (GIS Toolbox)
 * Purpose:  create overviews tools.
 * Author:   Bishop (aka Barishnikov Dmitriy), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2010 Bishop
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

#include "wxgis/geoprocessing/gptool.h"

/** \class wxGISGPCreateOverviewsTool gpcreateovrtool.h
    \brief The geoprocessing tool create raster overviews
*/

class WXDLLIMPEXP_GIS_GP wxGISGPCreateOverviewsTool : 
    public wxGISGPTool
{
   DECLARE_DYNAMIC_CLASS(wxGISGPCreateOverviewsTool)

public:
    wxGISGPCreateOverviewsTool(void);
    virtual ~wxGISGPCreateOverviewsTool(void);
    //IGPTool
    virtual const wxString GetDisplayName(void);
    virtual const wxString GetName(void);
    virtual const wxString GetCategory(void);
    virtual bool Execute(ITrackCancel* pTrackCancel);
    virtual bool Validate(void);
    virtual GPParameters GetParameterInfo(void);
protected:
	wxString m_sInputPath;
};