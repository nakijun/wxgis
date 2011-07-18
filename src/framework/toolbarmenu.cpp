/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  toolbar check menu class.
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
#include "wxgis/framework/toolbarmenu.h"
#include "wxgis/framework/commandbar.h"


BEGIN_EVENT_TABLE(wxGISToolBarMenu, wxMenu)
	EVT_MENU_RANGE(ID_TOOLBARCMD, ID_TOOLBARCMDMAX, wxGISToolBarMenu::OnCommand)
END_EVENT_TABLE()

wxGISToolBarMenu::wxGISToolBarMenu(const wxString& sName, const wxString& sCaption, wxGISEnumCommandBars type, const wxString& title, long style) : wxGISMenu(sName, sCaption, type, title, style)
{
}

wxGISToolBarMenu::~wxGISToolBarMenu(void)
{
}

void wxGISToolBarMenu::Update(void)
{

	//cleare contents
	for(size_t i = 0; i < m_delitems.size(); ++i)
		Destroy(m_delitems[i]);
	m_delitems.clear();


	COMMANDBARARRAY* pCommandBars = m_pApp->GetCommandBars();

	for(size_t i = 0; i < pCommandBars->size(); ++i)
	{
		IGISCommandBar* pCmdBar = pCommandBars->at(i);
		if(pCmdBar->GetType() == enumGISCBToolbar)
		{
			wxWindow* pWnd = dynamic_cast<wxWindow*>(pCmdBar);
			bool bIsShown = pWnd->IsShown();
			//sort!!!
			wxMenuItem* pItem = Prepend(ID_TOOLBARCMD + i, pCmdBar->GetCaption(), wxT(""), wxITEM_CHECK);
			pItem->Check(bIsShown);
			m_delitems.push_back(pItem);
		}
	}
	m_delitems.push_back(AppendSeparator());
	ICommand* pCmd = m_pApp->GetCommand(wxT("wxGISCommonCmd"), 2);
	if(pCmd)
		m_delitems.push_back(Append(pCmd->GetID(), pCmd->GetCaption(), pCmd->GetTooltip(), (wxItemKind)pCmd->GetKind()));
}

void wxGISToolBarMenu::OnCommand(wxCommandEvent& event)
{
	int pos = event.GetId() - (ID_TOOLBARCMD);
	COMMANDBARARRAY* pCommandBars = m_pApp->GetCommandBars();
	wxWindow* pWnd = dynamic_cast<wxWindow*>(pCommandBars->at(pos));
	m_pApp->ShowApplicationWindow(pWnd, !pWnd->IsShown());
}

wxIcon wxGISToolBarMenu::GetBitmap(void)
{
	return wxNullIcon;
}

wxString wxGISToolBarMenu::GetCaption(void)
{
	return wxString(_("Toolbars"));
}

wxString wxGISToolBarMenu::GetCategory(void)
{
	return wxString(_("[Menus]"));
}

bool wxGISToolBarMenu::GetChecked(void)
{
	return false;
}

bool wxGISToolBarMenu::GetEnabled(void)
{
	return true;
}

wxString wxGISToolBarMenu::GetMessage(void)
{
	return wxString(_("Toolbars"));
}

wxGISEnumCommandKind wxGISToolBarMenu::GetKind(void)
{
	return enumGISCommandMenu;
}

void wxGISToolBarMenu::OnClick(void)
{
}

bool wxGISToolBarMenu::OnCreate(IFrameApplication* pApp)
{
	m_pApp = dynamic_cast<wxGISApplication*>(pApp);
	return true;
}

wxString wxGISToolBarMenu::GetTooltip(void)
{
	return wxString(_("Toolbars"));
}

unsigned char wxGISToolBarMenu::GetCount(void)
{
	return 1;
}

