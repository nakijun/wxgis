/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  GxObject Commands
 * Author:   Bishop (aka Baryshnikov Dmitriy), polimax@mail.ru
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
#include "wxgis/catalogui/gxobjcmd.h"
#include "wxgis/catalog/catalog.h"
#include "wxgis/catalogui/catalogui.h"
#include "wxgis/datasource/rasterdataset.h"
#include "wxgis/datasource/rasterop.h"
#include "wxgis/catalogui/gxcatalogui.h"
#include "wxgis/catalogui/gxdatasetui.h"

IMPLEMENT_DYNAMIC_CLASS(wxGISRasterCmd, wxObject)

wxGISRasterCmd::wxGISRasterCmd(void)
{
}

wxGISRasterCmd::~wxGISRasterCmd(void)
{
}

wxIcon wxGISRasterCmd::GetBitmap(void)
{
	switch(m_subtype)
	{
		case 0:
		//	if(!m_IconFolderUp.IsOk())
		//		m_IconFolderUp = wxIcon(folder_up_xpm);
		//	return m_IconFolderUp;
		default:
			return wxNullIcon;
	}
}

wxString wxGISRasterCmd::GetCaption(void)
{
	switch(m_subtype)
	{
		case 0:	
			return wxString(_("Create &world file"));
		default:
			return wxEmptyString;
	}
}

wxString wxGISRasterCmd::GetCategory(void)
{
	switch(m_subtype)
	{
		case 0:	
			return wxString(_("Raster"));
		default:
			return wxString(_("[No category]"));
	}
}

bool wxGISRasterCmd::GetChecked(void)
{
	return false;
}

bool wxGISRasterCmd::GetEnabled(void)
{
	switch(m_subtype)
	{
		case 0://Up One Level
		{
			//check if not root
			IGxApplication* pGxApp = dynamic_cast<IGxApplication*>(m_pApp);
			if(pGxApp)
			{
                wxGxCatalogUI* pGxCatalogUI = dynamic_cast<wxGxCatalogUI*>(pGxApp->GetCatalog());
				if(!pGxCatalogUI)
					return false;
				IGxSelection* pSel = pGxCatalogUI->GetSelection();
				if(pSel->GetCount() == 0)
					return false;
				IGxObjectSPtr pGxObject = pGxCatalogUI->GetRegisterObject(pSel->GetSelectedObjectID(0));
				wxGxRasterDataset* pGxRasterDataset = dynamic_cast<wxGxRasterDataset*>(pGxObject.get());
				if(pGxRasterDataset)
				{
					CPLString soWldFilePath = GetWorldFilePath(pGxRasterDataset->GetInternalName());
					if(soWldFilePath.empty())
						return true;
				}
			}
			return false;
		}
		default:
			return false;
	}
}

wxGISEnumCommandKind wxGISRasterCmd::GetKind(void)
{
	switch(m_subtype)
	{
		case 0://create world file
			return enumGISCommandNormal;
		default:
			return enumGISCommandNormal;
	}
}

wxString wxGISRasterCmd::GetMessage(void)
{
	switch(m_subtype)
	{
		case 0:	
			return wxString(_("Create world file"));
		default:
			return wxEmptyString;
	}
}

void wxGISRasterCmd::OnClick(void)
{
	switch(m_subtype)
	{
		case 0:	
			{
				IGxApplication* pGxApp = dynamic_cast<IGxApplication*>(m_pApp);
				if(pGxApp)
				{
                    wxGxCatalogUI* pGxCatalogUI = dynamic_cast<wxGxCatalogUI*>(pGxApp->GetCatalog());
					IGxSelection* pSel = pGxCatalogUI->GetSelection();
					if(pSel->GetCount() == 0)
						break;
					IGxObjectSPtr pGxObject = pGxCatalogUI->GetRegisterObject(pSel->GetSelectedObjectID(0));
					wxGxRasterDataset* pGxRasterDataset = dynamic_cast<wxGxRasterDataset*>(pGxObject.get());
					if(pGxRasterDataset)
					{
						wxGISDatasetSPtr pGISDataset = pGxRasterDataset->GetDataset(false);
						wxGISRasterDatasetSPtr pGISRasterDataset = boost::dynamic_pointer_cast<wxGISRasterDataset>(pGISDataset);
						wxGISAppConfigSPtr pConfig = GetConfig();
						wxGISEnumWldExtType eType = (wxGISEnumWldExtType)pConfig->ReadInt(enumGISHKCU, wxString(wxT("wxGISCommon/raster/worldfile_ext_type")), enumGISWldExt_ExtPlusWX);
						if(pGISRasterDataset->WriteWorldFile(eType))
							wxMessageBox(wxString(_("World file created successufuly!")), wxString(_("Information")), wxICON_INFORMATION | wxOK);
						else
						{
							const char* err = CPLGetLastErrorMsg();
							wxString sErr = wxString::Format(_("World file create failed! GDAL error: %s"), wxString(err, wxConvUTF8));
							wxLogError(sErr);
							wxMessageBox(sErr, _("Error"), wxOK | wxICON_ERROR);
						}
					}
				}
			}
			break;
		default:
			return;
	}
}

bool wxGISRasterCmd::OnCreate(IFrameApplication* pApp)
{
	m_pApp = pApp;
	return true;
}

wxString wxGISRasterCmd::GetTooltip(void)
{
	switch(m_subtype)
	{
		case 0:	
			return wxString(_("Create world file"));
		default:
			return wxEmptyString;
	}
}

unsigned char wxGISRasterCmd::GetCount(void)
{
	return 1;
}

