/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxObjectDialog filters of GxObjects to show.
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

#include "wxgis/catalog/catalog.h"

//------------------------------------------------------------
// wxGxObjectFilter
//------------------------------------------------------------

class WXDLLIMPEXP_GIS_CLT wxGxObjectFilter : public IGxObjectFilter
{
public:
	wxGxObjectFilter(void);
	virtual ~wxGxObjectFilter(void);
	virtual bool CanChooseObject( IGxObject* pObject );
	virtual bool CanDisplayObject( IGxObject* pObject );
	virtual wxGISEnumSaveObjectResults CanSaveObject( IGxObject* pLocation, wxString sName );
	virtual wxString GetName(void);
    virtual wxString GetExt(void);
    virtual wxString GetDriver(void);
    virtual int GetSubType(void);
};

//------------------------------------------------------------
// wxGxPrjFileFilter
//------------------------------------------------------------

class WXDLLIMPEXP_GIS_CLT wxGxPrjFileFilter : public wxGxObjectFilter
{
public:
	wxGxPrjFileFilter(void);
	virtual ~wxGxPrjFileFilter(void);
	virtual bool CanChooseObject( IGxObject* pObject );
	virtual bool CanDisplayObject( IGxObject* pObject );
	virtual wxGISEnumSaveObjectResults CanSaveObject( IGxObject* pLocation, wxString sName );
	virtual wxString GetName(void);
    virtual wxString GetExt(void);
};

//------------------------------------------------------------
// wxGxRasterDatasetFilter
//------------------------------------------------------------

class WXDLLIMPEXP_GIS_CLT wxGxRasterDatasetFilter : public wxGxObjectFilter
{
public:
	wxGxRasterDatasetFilter(void);
	virtual ~wxGxRasterDatasetFilter(void);
	virtual bool CanChooseObject( IGxObject* pObject );
	virtual bool CanDisplayObject( IGxObject* pObject );
	virtual wxString GetName(void);
};

//------------------------------------------------------------
// wxGxShapeFileFilter
//------------------------------------------------------------

class WXDLLIMPEXP_GIS_CLT wxGxShapeFileFilter : public wxGxObjectFilter
{
public:
	wxGxShapeFileFilter(void);
	virtual ~wxGxShapeFileFilter(void);
	virtual bool CanChooseObject( IGxObject* pObject );
	virtual bool CanDisplayObject( IGxObject* pObject );
	virtual wxString GetName(void);
    virtual wxString GetExt(void);
    virtual wxString GetDriver(void);
    virtual int GetSubType(void);
};

//------------------------------------------------------------
// wxGxMapInfoFilter
//------------------------------------------------------------

class WXDLLIMPEXP_GIS_CLT wxGxMapInfoFilter : public wxGxShapeFileFilter
{
public:
	wxGxMapInfoFilter(bool bIsTab = true);
	virtual ~wxGxMapInfoFilter(void);
	virtual wxString GetName(void);
    virtual wxString GetExt(void);
    virtual wxString GetDriver(void);
    virtual int GetSubType(void);
protected:
    bool m_bIsTab;
};

//------------------------------------------------------------
// wxGxKMLFilter
//------------------------------------------------------------

class WXDLLIMPEXP_GIS_CLT wxGxKMLFilter : public wxGxShapeFileFilter
{
public:
	wxGxKMLFilter(void);
	virtual ~wxGxKMLFilter(void);
	virtual wxString GetName(void);
    virtual wxString GetExt(void);
    virtual wxString GetDriver(void);
    virtual int GetSubType(void);
};


//------------------------------------------------------------
// wxGxDXFFilter
//------------------------------------------------------------

class WXDLLIMPEXP_GIS_CLT wxGxDXFFilter : public wxGxShapeFileFilter
{
public:
	wxGxDXFFilter(void);
	virtual ~wxGxDXFFilter(void);
	virtual wxString GetName(void);
    virtual wxString GetExt(void);
    virtual wxString GetDriver(void);
    virtual int GetSubType(void);
};

//------------------------------------------------------------
// wxGxFolderFilter
//------------------------------------------------------------

class WXDLLIMPEXP_GIS_CLT wxGxFolderFilter : public wxGxObjectFilter
{
public:
	wxGxFolderFilter(void);
	virtual ~wxGxFolderFilter(void);
	virtual bool CanChooseObject( IGxObject* pObject );
	virtual bool CanDisplayObject( IGxObject* pObject );
	virtual wxString GetName(void);
};