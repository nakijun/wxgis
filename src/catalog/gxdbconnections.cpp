/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxDBConnections class.
 * Author:   Baryshnikov Dmitriy (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2011,2013 Bishop
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

#include "wxgis/catalog/gxdbconnections.h"
#include "wxgis/core/config.h"
#include "wxgis/datasource/datasource.h"

//---------------------------------------------------------------------------
// wxGxDBConnections
//---------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxGxDBConnections, wxGxFolder)

wxGxDBConnections::wxGxDBConnections(void) : wxGxFolder()
{
}

bool wxGxDBConnections::Create(wxGxObject *oParent, const wxString &soName, const CPLString &soPath)
{
    if( !wxGxFolder::Create(oParent, _("DataBase connections"), soPath) )
    {
        wxLogError(_("wxGxDBConnections::Create failed. GxObject %s"), wxString(_("DataBase connections")).c_str());
        return false;
    }
    return true;
}

wxGxDBConnections::~wxGxDBConnections(void)
{
}

void wxGxDBConnections::Init(wxXmlNode* const pConfigNode)
{
    m_sInternalPath = pConfigNode->GetAttribute(wxT("path"), NON);
    if(m_sInternalPath.IsEmpty() || m_sInternalPath == wxString(NON))
    {
		wxGISAppConfig oConfig = GetConfig();
		if(!oConfig.IsOk())
			return;

		m_sInternalPath = oConfig.GetLocalConfigDir() + wxFileName::GetPathSeparator() + wxString(wxT("dbconnections"));
    }

    m_sInternalPath.Replace(wxT("\\"), wxT("/"));
    wxLogMessage(_("wxGxDBConnections: The path is set to '%s'"), m_sInternalPath.c_str());
    CPLSetConfigOption("wxGxDBConnections", m_sInternalPath.mb_str(wxConvUTF8));

    m_sPath = CPLString(m_sInternalPath.mb_str(wxConvUTF8));

	if(!wxDirExists(m_sInternalPath))
		wxFileName::Mkdir(m_sInternalPath, 0755, wxPATH_MKDIR_FULL);
}

void wxGxDBConnections::Serialize(wxXmlNode* pConfigNode)
{
    pConfigNode->AddAttribute(wxT("path"), m_sInternalPath);
}

bool wxGxDBConnections::CanCreate(long nDataType, long DataSubtype)
{
	if(nDataType != enumGISContainer)
		return false;
	if(DataSubtype != enumContFolder && DataSubtype != enumContRemoteDBConnection)
		return false;
	return wxGxFolder::CanCreate(nDataType, DataSubtype);
}

