/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  catalog operations.
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

#include "wxgis/catalog/catop.h"
/*
bool OverWriteGxObject(IGxObject* pGxObject, ITrackCancel* pTrackCancel)
{
	if(!pGxObject) //the object is not exist
		return true;
    IGxObjectEdit* pGxDstObjectEdit = dynamic_cast<IGxObjectEdit*>(pGxObject);
    if(pGxDstObjectEdit)
    {
        //add messages to pTrackCancel
		if(pTrackCancel)
			pTrackCancel->PutMessage(_("Overwrite destination object"), -1, enumGISMessageInfo);
		if(!pGxDstObjectEdit->CanDelete() || !pGxDstObjectEdit->Delete())
		{
			if(pTrackCancel)
				pTrackCancel->PutMessage(_("Failed overwrite destination object"), -1, enumGISMessageErr);
			return false;
		}
    }
	else
	{
		if(pTrackCancel)
			pTrackCancel->PutMessage(_("Cannot delete item!"), -1, enumGISMessageErr);
        return false;
	}
	return true;
}

IGxObject* GetParentGxObjectFromPath(const wxString &sFullPath, IGxObjectContainer* pGxObjectContainer, ITrackCancel* pTrackCancel)
{
	if(!pGxObjectContainer)
		return NULL;
    wxFileName sDstFileName(sFullPath);
    wxString sPath = sDstFileName.GetPath();
    IGxObject* pGxDstObject = pGxObjectContainer->SearchChild(sPath);
    if(!pGxDstObject)
    {
        if(pTrackCancel)
            pTrackCancel->PutMessage(_("Error get destination object"), -1, enumGISMessageErr);
    }
    return pGxDstObject;
}
*/