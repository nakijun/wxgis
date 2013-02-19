/******************************************************************************
 * Project:  wxGIS
 * Purpose:  Cursor class.
 * Author:   Baryshnikov Dmitriy (aka Bishop), polimax@mail.ru
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


#include "wxgis/datasource/gdalinh.h"

#include <list>

/** \class wxFeatureCursor cursor.h
    \brief The class represents an array of OGRFeatures, received by some selection.
*/

class WXDLLIMPEXP_GIS_DS wxFeatureCursor : public wxObject
{
    DECLARE_CLASS(wxFeatureCursor)
public:
	wxFeatureCursor(void);
	virtual ~wxFeatureCursor(void);

    bool IsOk() const;

    bool operator == ( const wxFeatureCursor& obj ) const;
    bool operator != (const wxFeatureCursor& obj) const { return !(*this == obj); };


	virtual void Add(wxGISFeature Feature);
	virtual void Reset(void);
	virtual void Clear(void);
	virtual wxGISFeature Next(void);
	virtual size_t GetCount(void) const;
protected:
    virtual wxObjectRefData *CreateRefData() const;
    virtual wxObjectRefData *CloneRefData(const wxObjectRefData *data) const;
protected:	
	std::list<wxGISFeature>::const_iterator m_Iterator;
};


/** \class wxFeatureCursorRefData cursor.h
    \brief The reference data class for wxFeatureCursor
*/

class  wxFeatureCursorRefData : public wxObjectRefData
{
    friend class wxFeatureCursor;
public:
    wxFeatureCursorRefData( void ) : wxObjectRefData()
    {
    }

    virtual ~wxFeatureCursorRefData()
    {
        m_olFeatures.clear();
    }
    
    bool operator == (const wxFeatureCursorRefData& data) const
    {
        return m_olFeatures == data.m_olFeatures;
    }

    wxFeatureCursorRefData( const wxFeatureCursorRefData& data )
        : wxObjectRefData()
    {
        m_olFeatures = data.m_olFeatures;
    }

    std::list<wxGISFeature>::const_iterator Begin(void) const
    {
        return m_olFeatures.begin();
    }


protected:
	std::list<wxGISFeature> m_olFeatures;
};
