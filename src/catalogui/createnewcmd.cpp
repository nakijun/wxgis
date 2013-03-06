/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  Catalog Create New Commands class.
 * Author:   Baryshnikov Dmitriy (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2011-2012 Bishop
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
#include "wxgis/catalogui/createnewcmd.h"

#include "wxgis/catalogui/gxcatalogui.h"
#include "wxgis/catalogui/remoteconndlg.h"
#include "wxgis/datasource/sysop.h"
#include "wxgis/catalogui/gxselection.h"
#include "wxgis/catalogui/gxdbconnectionsui.h"

#include "../../art/rdb_create.xpm"
#include "../../art/rdb_conn_16.xpm"
#include "../../art/rdb_conn_48.xpm"
#include "../../art/rdb_disconn_16.xpm"
#include "../../art/rdb_disconn_48.xpm"

//	0	Create new remote connection
//  1   ?

IMPLEMENT_DYNAMIC_CLASS(wxGISCreateNewCmd, wxObject)

wxGISCreateNewCmd::wxGISCreateNewCmd(void)
{
}

wxGISCreateNewCmd::~wxGISCreateNewCmd(void)
{
}

wxIcon wxGISCreateNewCmd::GetBitmap(void)
{
	switch(m_subtype)
	{
		case 0:
			if(!m_IconCreateRemoteConn.IsOk())
				m_IconCreateRemoteConn = wxIcon(rdb_create_xpm);
			return m_IconCreateRemoteConn;
		default:
			return wxNullIcon;
	}
}

wxString wxGISCreateNewCmd::GetCaption(void)
{
	switch(m_subtype)
	{
		case 0:
			return wxString(_("&Remote connection"));
		default:
			return wxEmptyString;
	}
}

wxString wxGISCreateNewCmd::GetCategory(void)
{
	switch(m_subtype)
	{
		case 0:
			return wxString(_("New"));
		default:
			return wxString(_("[No category]"));
	}
}

bool wxGISCreateNewCmd::GetChecked(void)
{
	return false;
}

bool wxGISCreateNewCmd::GetEnabled(void)
{
    wxCHECK_MSG(m_pGxApp && m_pApp, false, wxT("Application pointer is null"));

    wxGxSelection* pSel = m_pGxApp->GetGxSelection();
    wxGxCatalogBase* pCat = GetGxCatalog();

    switch(m_subtype)
	{
		case 0://Create new remote connection
            if(pCat && pSel)
            {
                wxGxObject* pGxObject = pCat->GetRegisterObject(pSel->GetFirstSelectedObjectId());
                wxGxObjectContainer* pGxObjectContainer = wxDynamicCast(pGxObject, wxGxObjectContainer);
                if(pGxObjectContainer && pGxObjectContainer->CanCreate(enumGISContainer, enumContRemoteConnection))
                {
                    return true;
                }
            }
            return false;
		default:
			return false;
	}
}

wxGISEnumCommandKind wxGISCreateNewCmd::GetKind(void)
{
	switch(m_subtype)
	{
		case 0://Create new remote connection
		default:
			return enumGISCommandNormal;
	}
}

wxString wxGISCreateNewCmd::GetMessage(void)
{
	switch(m_subtype)
	{
		case 0:
			return wxString(_("Create new remote connection"));
		default:
			return wxEmptyString;
	}
}

void wxGISCreateNewCmd::OnClick(void)
{
    wxCHECK_RET(m_pGxApp && m_pApp, wxT("Application pointer is null"));

    wxGxSelection* pSel = m_pGxApp->GetGxSelection();
    wxGxCatalogBase* pCat = GetGxCatalog();
    
	switch(m_subtype)
	{
		case 0:
#ifdef wxGIS_USE_POSTGRES
            if(pCat && pSel)
            {
                wxGxObject* pGxObject = pCat->GetRegisterObject(pSel->GetFirstSelectedObjectId());
                wxGxDBConnectionsUI* pGxDBConnectionsUI = wxDynamicCast(pGxObject, wxGxDBConnectionsUI);
                if(pGxDBConnectionsUI)
                {
                    CPLString pszConnFolder = pGxDBConnectionsUI->GetPath();
                    CPLString pszConnName(CheckUniqName(pszConnFolder, wxString(_("new DB connection")), wxString(wxT("xconn"))).mb_str(wxConvUTF8));

                    wxGxView* pGxView = dynamic_cast<wxGxView*>(wxWindow::FindFocus());
                    pGxDBConnectionsUI->BeginRenameOnAdd(pGxView, pszConnName);

					wxGISRemoteConnDlg dlg(CPLFormFilename(pszConnFolder, pszConnName, "xconn"), dynamic_cast<wxWindow*>(m_pApp));
					if(dlg.ShowModal() != wxID_OK)
                    {
                        pGxDBConnectionsUI->BeginRenameOnAdd(NULL, "");
                    }
                }
            }
#endif //wxGIS_USE_POSTGRES
            break;
		default:
			return;
	}
}

bool wxGISCreateNewCmd::OnCreate(wxGISApplicationBase* pApp)
{
	m_pApp = pApp;
    m_pGxApp = dynamic_cast<wxGxApplicationBase*>(pApp);
	return true;
}

wxString wxGISCreateNewCmd::GetTooltip(void)
{
	switch(m_subtype)
	{
		case 0:
			return wxString(_("Create new remote connection"));
		default:
			return wxEmptyString;
	}
}

unsigned char wxGISCreateNewCmd::GetCount(void)
{
	return 1;
}
