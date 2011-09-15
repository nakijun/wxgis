/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxFolderUI class.
 * Author:   Bishop (aka Barishnikov Dmitriy), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2009-2011 Bishop
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
#include "wxgis/catalogui/gxfolderui.h"
#include "wxgis/catalogui/gxcatalogui.h"
#include "wxgis/framework/progressdlg.h"
#include "wxgis/core/globalfn.h"


wxGxFolderUI::wxGxFolderUI(CPLString Path, wxString Name, wxIcon LargeIcon, wxIcon SmallIcon) : wxGxFolder(Path, Name)
{
    m_oLargeIcon = LargeIcon;
    m_oSmallIcon = SmallIcon;
}

wxGxFolderUI::~wxGxFolderUI(void)
{
}

wxIcon wxGxFolderUI::GetLargeImage(void)
{
    return m_oLargeIcon;
}

wxIcon wxGxFolderUI::GetSmallImage(void)
{
    return m_oSmallIcon;
}

void wxGxFolderUI::EditProperties(wxWindow *parent)
{
}

void wxGxFolderUI::EmptyChildren(void)
{
	for(size_t i = 0; i < m_Children.size(); ++i)
	{
		m_Children[i]->Detach();
		wxDELETE( m_Children[i] );
	}
    m_Children.clear();
	m_bIsChildrenLoaded = false;
}

wxDragResult wxGxFolderUI::CanDrop(wxDragResult def)
{
    return def;
}

bool wxGxFolderUI::Drop(const wxArrayString& filenames, bool bMove)
{
   if(filenames.GetCount() == 0)
        return false;
    char **papszFileList = NULL;    
    CPLString szPath;
    for(size_t i = 0; i < filenames.GetCount(); ++i)
    {
        //Change to CPLString
        CPLString szFilePath = filenames[i].mb_str(wxConvUTF8);
        if(i == 0)
            szPath = CPLGetPath(szFilePath);
        papszFileList = CSLAddString( papszFileList, szFilePath );        
    }

	GxObjectArray Array;	
    if(!m_pCatalog->GetChildren(szPath, papszFileList, Array))
    {
        CSLDestroy( papszFileList );
        return false;
    }
    CSLDestroy( papszFileList );

    //create progress dialog
	wxString sTitle = wxString::Format(_("%s %d objects (files)"), bMove == true ? _("Move") : _("Copy"), filenames.GetCount());
	wxWindow* pParentWnd = dynamic_cast<wxWindow*>(GetApplication());
	wxGISProgressDlg ProgressDlg(sTitle, _("Begin operation..."), 100, pParentWnd, wxPD_APP_MODAL | wxPD_AUTO_HIDE | wxPD_SMOOTH | wxPD_CAN_ABORT | wxPD_ELAPSED_TIME | wxPD_ESTIMATED_TIME | wxPD_REMAINING_TIME);

    for(size_t i = 0; i < Array.size(); ++i)
    {
		wxString sMessage = wxString::Format(_("%s %d object (file) from %d"), bMove == true ? _("Move") : _("Copy"), i + 1, Array.size());
		ProgressDlg.SetTitle(sMessage);
		ProgressDlg.PutMessage(sMessage);
        if(!ProgressDlg.Continue())
            break;

        IGxObjectEdit* pGxObjectEdit = dynamic_cast<IGxObjectEdit*>(Array[i]);
        if(pGxObjectEdit)
        {
            if(bMove && pGxObjectEdit->CanMove(m_sPath))
            {
                if(pGxObjectEdit->Move(m_sPath, &ProgressDlg))
                {
                    bool ret_code = AddChild(Array[i]);
                    if(!ret_code)
                    {
                        wxDELETE(Array[i]);
                    }
                    else
                    {
                        m_pCatalog->ObjectAdded(Array[i]->GetID());
                    }
                }
            }
            else if(!bMove && pGxObjectEdit->CanCopy(m_sPath))
            {
                if(pGxObjectEdit->Copy(m_sPath, &ProgressDlg))
                {
                    bool ret_code = AddChild(Array[i]);
                    if(!ret_code)
                    {
                        wxDELETE(Array[i]);
                    }
                    else
                    {
                        m_pCatalog->ObjectAdded(Array[i]->GetID());
                    }
                }
            }
            else
                return false;
        }
    }
    return true;
}

