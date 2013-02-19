/******************************************************************************
 * Project:  wxGIS
 * Purpose:  wxGISApplication class. Base application functionality (commands, menues, etc.)
 * Author:   Baryshnikov Dmitriy (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2009-2012 Bishop
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
#include "wxgis/framework/application.h"
#include "wxgis/framework/toolbarmenu.h"
#include "wxgis/framework/propertypages.h"
#include "wxgis/core/app.h"
#include "wxgis/core/config.h"

#include "../../art/tool_16.xpm"
#include "../../art/options.xpm"

#include "wx/propdlg.h"
#include "wx/tokenzr.h"

//#include "wx/wxhtml.h"

//--------------------------------------------------------------------------
// wxGISApplication
//--------------------------------------------------------------------------

IMPLEMENT_CLASS2(wxGISApplication, wxFrame, wxGISApplicationBase)

BEGIN_EVENT_TABLE(wxGISApplication, wxFrame)
    EVT_ERASE_BACKGROUND(wxGISApplication::OnEraseBackground)
    EVT_SIZE(wxGISApplication::OnSize)
	EVT_RIGHT_DOWN(wxGISApplication::OnRightDown)
	EVT_MENU_RANGE(ID_PLUGINCMD, ID_TOOLBARCMDMAX, wxGISApplication::OnCommand)
	EVT_MENU_RANGE(ID_MENUCMD, ID_MENUCMDMAX, wxGISApplication::OnDropDownCommand)
	EVT_UPDATE_UI_RANGE(ID_PLUGINCMD, ID_PLUGINCMDMAX, wxGISApplication::OnCommandUI)
    EVT_AUITOOLBAR_RIGHT_CLICK(wxID_ANY, wxGISApplication::OnAuiRightDown)
    EVT_AUITOOLBAR_TOOL_DROPDOWN(wxID_ANY, wxGISApplication::OnToolDropDown)
    EVT_CLOSE(wxGISApplication::OnClose)
END_EVENT_TABLE()

wxGISApplication::wxGISApplication(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style) : wxFrame(parent, id, title, pos, size, style), wxGISApplicationBase()
{
    m_pGISAcceleratorTable = NULL;
    m_pDropDownCommand = NULL;
    m_pTrackCancel = NULL;
    //m_pszOldLocale = NULL;
    m_pLocale = NULL;
    m_sDecimalPoint = wxString(wxT("."));
}

wxGISApplication::~wxGISApplication(void)
{
  //   if(m_pszOldLocale != NULL)
		//setlocale(LC_NUMERIC, m_pszOldLocale);
    wxDELETE(m_pLocale);
	//wxDELETE(m_pszOldLocale);
}

wxStatusBar* wxGISApplication::OnCreateStatusBar(int number, long style, wxWindowID id, const wxString& name)
{
	wxGISStatusBar *sb = new wxGISStatusBar(this, id, style, name);
	return static_cast<wxStatusBar*>(sb);
}

wxGISStatusBar* wxGISApplication::GetStatusBar(void) const
{
	return wxDynamicCast(wxFrame::GetStatusBar(), wxGISStatusBar);//dynamic_cast<wxGISStatusBar*>(wxFrame::GetStatusBar());
}

void wxGISApplication::OnEraseBackground(wxEraseEvent& event)
{
}

void wxGISApplication::OnSize(wxSizeEvent& event)
{
    event.Skip();
}

void wxGISApplication::OnCommand(wxCommandEvent& event)
{
    event.Skip();
	if(event.GetId() >= ID_PLUGINCMD && event.GetId() <= ID_PLUGINCMDMAX)
		Command(GetCommand(event.GetId()));
	else if(event.GetId() >= ID_TOOLBARCMD && event.GetId() <= ID_TOOLBARCMDMAX)
	{
		wxGISToolBarMenu* pToolBarMenu = static_cast<wxGISToolBarMenu*>(GetCommandBar(TOOLBARMENUNAME));
		if(pToolBarMenu)
			pToolBarMenu->OnCommand(event);
	}
}

void wxGISApplication::OnDropDownCommand(wxCommandEvent& event)
{
    event.Skip();
    if(m_pDropDownCommand)
        m_pDropDownCommand->OnDropDownCommand(event.GetId());
}

void wxGISApplication::OnCommandUI(wxUpdateUIEvent& event)
{
    //event.Skip();
    int nCmdId = event.GetId();
    wxString sAcc = m_pGISAcceleratorTable->GetText(nCmdId);
	wxGISCommand* pCmd = GetCommand(nCmdId);
	if(pCmd)
	{
        wxString sCaption = pCmd->GetCaption();
        if(!sAcc.IsEmpty())
        {
			sCaption.Append( wxT("\t") );
            sCaption.Append(sAcc);
        }

        event.SetText(sCaption);
#ifdef __WXMSW__
		if(pCmd->GetKind() == enumGISCommandCheck)
#endif
            event.Check(pCmd->GetChecked());

        event.Enable(pCmd->GetEnabled());
    }

//#ifdef __WXGTK__
//    m_pMenuBar->UpdateMenus();
//#endif
		//wxMenu* pMenu = dynamic_cast<wxMenu*>(m_CommandBarArray[i]);
  //      if(pMenu)
  //      {
  //          wxMenuItem *pItem = pMenu->FindItem(nCmdId);
  //          continue;
  //      }
//        return;

    for(size_t i = 0; i < m_CommandBarArray.size(); ++i)
    {
        switch(m_CommandBarArray[i]->GetType())
        {
        case enumGISCBSubMenu:
        case enumGISCBMenubar:
#ifdef __WXGTK__
            {
                wxMenu* pMenu = dynamic_cast<wxMenu*>(m_CommandBarArray[i]);
//// dirty hack
//                    //wxMenuItemList& pLst = pMenu->GetMenuItems();
//                    //wxMenuItemList::iterator iter;
//                    //for (iter = pLst.begin(); iter != pLst.end(); ++iter)
//                    //{
//                    //    wxMenuItem* pItem = *iter;
//                    //    if(pItem->IsSubMenu())
//                    //    {
//                    //        pItem->SetBitmap(wxNullBitmap);
//                    //        //wxString sT = pItem->GetText();
//                    //        //pItem->SetItemLabel(wxT(" "));// derty hack
//                    //        //pItem->SetItemLabel(sT);
//                    //    }
//                    //}
//// dirty hack end
                wxMenuItem *pItem = pMenu->FindItem(nCmdId);
                if(pItem != NULL && pCmd)
                {
                    if(!pItem->IsSubMenu())
                    {
//						wxIcon Bmp = pCmd->GetBitmap();
//						//if(Bmp.IsOk())
//							pItem->SetBitmap(Bmp);//double text??
//						//pItem->SetItemLabel(wxT(" ")); // derty hack
                    if(sAcc.IsEmpty())
                        pItem->SetItemLabel(pCmd->GetCaption());
                    else
                        pItem->SetItemLabel(pCmd->GetCaption() + wxT("\t") + sAcc);
                    }
                }
            }
#endif	//__WXGTK__
            break;
//			case enumGISCBContextmenu:
//				{
//					wxMenu* pMenu = dynamic_cast<wxMenu*>(m_CommandBarArray[i]);
//// dirty hack
//                    //wxMenuItemList& pLst = pMenu->GetMenuItems();
//                    //wxMenuItemList::iterator iter;
//                    //for (iter = pLst.begin(); iter != pLst.end(); ++iter)
//                    //{
//                    //    wxMenuItem* pItem = *iter;
//                    //    if(pItem->IsSubMenu())
//                    //    {
//                    //        pItem->SetBitmap(wxNullBitmap);
//                    //        //wxString sT = pItem->GetText();
//                    //        //pItem->SetItemLabel(wxT(" "));// derty hack
//                    //        //pItem->SetItemLabel(sT);
//                    //    }
//                    //}
//// dirty hack end
//                    wxMenuItem *pItem = pMenu->FindItem(nCmdId);
//					if(pItem != NULL)
//					{
//                        if(pItem->IsSubMenu())
//                            break;
//						wxIcon Bmp = pCmd->GetBitmap();
//						//if(Bmp.IsOk())
//							pItem->SetBitmap(Bmp);//double text??
//						//pItem->SetItemLabel(wxT(" ")); // derty hack
//						if(sAcc.IsEmpty())
//							pItem->SetItemLabel(pCmd->GetCaption());
//						else
//						    pItem->SetItemLabel(pCmd->GetCaption() + wxT("\t") + sAcc);
//                    }
//				}
//				break;
        case enumGISCBToolbar:
            {
                wxGISToolBar* pGISToolBar = dynamic_cast<wxGISToolBar*>(m_CommandBarArray[i]);
                if(pGISToolBar)
                    pGISToolBar->UpdateControls();
                wxAuiToolBar* pToolbar = dynamic_cast<wxAuiToolBar*>(m_CommandBarArray[i]);
                if(pToolbar && pCmd)
                {
                    wxAuiToolBarItem* pTool = pToolbar->FindTool(nCmdId);
                    if(pTool)
                    {
                        if(!sAcc.IsEmpty())
                            pTool->SetShortHelp(pCmd->GetTooltip() + wxT(" (") + sAcc + wxT(")"));
                        else
                            pTool->SetShortHelp(pCmd->GetTooltip());
#ifdef __WXMSW__
                        if(!pTool->GetBitmap().IsOk())
                        {
                            wxIcon Bmp = pCmd->GetBitmap();
                            if(Bmp.IsOk())
                                pTool->SetBitmap(Bmp);
                            else
                                pTool->SetBitmap(wxBitmap(tool_16_xpm));
                        }
#endif //__WXMSW__
                    }
                }
            }
            break;
        case enumGISCBNone:
        default:
            break;
		}
	}
	//event.Skip();
}

void wxGISApplication::SerializeFramePos(bool bSave)
{
	wxGISAppConfig oConfig = GetConfig();
	if(!oConfig.IsOk())
		return;

	if(bSave)
	{
		if( IsMaximized() )
			oConfig.Write(enumGISHKCU, GetAppName() + wxString(wxT("/frame/maxi")), true);
		else
		{
			int x, y, w, h;
			GetClientSize(&w, &h);
			GetPosition(&x, &y);
			oConfig.Write(enumGISHKCU, GetAppName() + wxString(wxT("/frame/maxi")), false);
			oConfig.Write(enumGISHKCU, GetAppName() + wxString(wxT("/frame/width")), w);
			oConfig.Write(enumGISHKCU, GetAppName() + wxString(wxT("/frame/height")), h);
			oConfig.Write(enumGISHKCU, GetAppName() + wxString(wxT("/frame/xpos")), x);
			oConfig.Write(enumGISHKCU, GetAppName() + wxString(wxT("/frame/ypos")), y);
		}
		//status bar shown state
		oConfig.Write(enumGISHKCU, GetAppName() + wxString(wxT("/frame/statusbar/shown")), IsStatusBarShown());
	}
	else
	{
		//load
		bool bMaxi = oConfig.ReadBool(enumGISHKCU, GetAppName() + wxString(wxT("/frame/maxi")), false);
		if(!bMaxi)
		{
			int x = oConfig.ReadInt(enumGISHKCU, GetAppName() + wxString(wxT("/frame/xpos")), 50);
			int y = oConfig.ReadInt(enumGISHKCU, GetAppName() + wxString(wxT("/frame/ypos")), 50);
			int w = oConfig.ReadInt(enumGISHKCU, GetAppName() + wxString(wxT("/frame/width")), 850);
			int h = oConfig.ReadInt(enumGISHKCU, GetAppName() + wxString(wxT("/frame/height")), 530);
			Move(x, y);
			SetClientSize(w, h);
		}
		else
		{
			Maximize();
		}
		//status bar shown state
		wxGISApplication::ShowStatusBar(oConfig.ReadBool(enumGISHKCU, GetAppName() + wxString(wxT("/frame/statusbar/shown")), true));
	}
}

wxGISAcceleratorTable* wxGISApplication::GetGISAcceleratorTable(void) const
{
	return m_pGISAcceleratorTable;
}

void wxGISApplication::ShowStatusBar(bool bShow)
{
	wxFrame::GetStatusBar()->Show(bShow);
}

bool wxGISApplication::IsStatusBarShown(void)
{
    wxCHECK_MSG( wxFrame::GetStatusBar(), false, wxT("The status bar is not created") );
	return wxFrame::GetStatusBar()->IsShown();
}

void wxGISApplication::OnRightDown(wxMouseEvent& event)
{
	event.Skip();
	ShowToolBarMenu();
}

void wxGISApplication::OnAuiRightDown(wxAuiToolBarEvent& event)
{
	event.Skip();
	ShowToolBarMenu();
}

void wxGISApplication::ShowToolBarMenu(void)
{
	wxGISToolBarMenu* pToolBarMenu = static_cast<wxGISToolBarMenu*>(GetCommandBar(TOOLBARMENUNAME));
	if(pToolBarMenu)
	{
		PopupMenu(pToolBarMenu);
	}
}

void wxGISApplication::OnToolDropDown(wxAuiToolBarEvent& event)
{
    if(event.IsDropDownClicked())
    {
        wxGISCommand* pCmd = GetCommand(event.GetToolId());
        m_pDropDownCommand = dynamic_cast<IDropDownCommand*>(pCmd);
        if(m_pDropDownCommand)
        {
            wxMenu* pMenu = m_pDropDownCommand->GetDropDownMenu();
            if(pMenu)
            {
                PushEventHandler(pMenu);
                PopupMenu(pMenu, event.GetItemRect().GetBottomLeft());
                PopEventHandler();
                delete pMenu;
                return;
            }
        }
    }
    event.Skip();
}

bool wxGISApplication::CreateApp(void)
{
	CreateStatusBar();
	wxFrame::GetStatusBar()->SetStatusText(_("Ready"));

    if(!wxGISApplicationBase::CreateApp())
        return false;
    //load commandbars
	SerializeCommandBars();

	//load accelerators
    m_pGISAcceleratorTable = new wxGISAcceleratorTable(this);


    wxGISAppConfig oConfig = GetConfig();
	if(!oConfig.IsOk())
		return false;    
    // create MenuBar
	wxXmlNode* pMenuBarNode = oConfig.GetConfigNode(enumGISHKCU, GetAppName() + wxString(wxT("/frame/menubar")));

    m_pMenuBar = new wxGISMenuBar(wxMB_DOCKABLE, this, pMenuBarNode); //wxMB_DOCKABLE
    SetMenuBar(static_cast<wxMenuBar*>(m_pMenuBar));

	//mark menues from menu bar as enumGISTAMMenubar
	for(size_t i = 0; i < m_CommandBarArray.GetCount(); ++i)
		if(m_pMenuBar->IsMenuBarMenu(m_CommandBarArray[i]->GetName()))
			m_CommandBarArray[i]->SetType(enumGISCBMenubar);

    // min size for the frame itself isn't completely done.
    // see the end up wxAuiManager::Update() for the test
    // code. For now, just hard code a frame minimum size
    SetMinSize(wxSize(800,480));

	SerializeFramePos(false);
	SetAcceleratorTable(m_pGISAcceleratorTable->GetAcceleratorTable());
    SetApplication( this );

//    wxHtmlWindow *pHtmlText = new wxHtmlWindow(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxHW_DEFAULT_STYLE | wxBORDER_THEME);
//    pHtmlText->SetPage(wxT("<html><body><h1>Error</h1>Some error occurred :-H)</body></hmtl>"));
//    pHtmlText->Show(false);
//    RegisterChildWindow(pHtmlText->GetId());
//
//#ifdef __WXGTK__
// //   wxGISToolBarMenu* pToolBarMenu =  dynamic_cast<wxGISToolBarMenu*>(GetCommandBar(TOOLBARMENUNAME));
//	//if(pToolBarMenu)
//	//    PushEventHandler(pToolBarMenu);
////	m_pMenuBar->PushEventHandler(this);
//#endif
    return true;
}

void wxGISApplication::OnClose(wxCloseEvent& event)
{
    event.Skip();

    if(m_pGISAcceleratorTable)
		m_pGISAcceleratorTable->Store();
	wxDELETE(m_pGISAcceleratorTable);

	wxGISAppConfig oConfig = GetConfig();
	if(m_pMenuBar && oConfig.IsOk())
	{
		wxXmlNode* pMenuBarNode = oConfig.GetConfigNode(enumGISHKCU, GetAppName() + wxString(wxT("/frame/menubar")));
		if(!pMenuBarNode)
			pMenuBarNode = oConfig.CreateConfigNode(enumGISHKCU, GetAppName() + wxString(wxT("/frame/menubar")));
		m_pMenuBar->Serialize(pMenuBarNode);
	}
	//wxDELETE(m_pMenuBar); delete by wxApp
	SerializeCommandBars(true);

	SerializeFramePos(true);
    
    wxFrame::OnCloseWindow(event);
}

void wxGISApplication::OnAppOptions(void)
{
    //read the config node for property pages and its names
	wxGISAppConfig oConfig = GetConfig();
	if(!oConfig.IsOk())
    {
        wxMessageBox(_("Get config failed!"), _("Error"), wxICON_ERROR | wxOK );
        return;
    }

    wxXmlNode *pPPXmlNode = oConfig.GetConfigNode(enumGISHKCU, GetAppName() + wxString(wxT("/propertypages")));
    if(!pPPXmlNode)
    {
        wxMessageBox(_("No Property Pages"), _("Error"), wxICON_ERROR | wxOK );
        return;
    }
    //load pages to the dialog and show
    wxPropertySheetDialog PropertySheetDialog;
    //(this, wxID_ANY, _("Options"), wxDefaultPosition, wxSize( 480,600 ), wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER);
    if (!PropertySheetDialog.Create(this, wxID_ANY, _("Options"), wxDefaultPosition, wxSize( 480,600 ), wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER))
        return;
    //PropertySheetDialog.SetMinSize( wxSize( 480,600 ));
    //PropertySheetDialog.SetSizeHints( wxSize( 480,600 ), wxDefaultSize );
    //PropertySheetDialog. SetClientSize( wxSize( 480,600 ));

    PropertySheetDialog.SetIcon(options_xpm);

    PropertySheetDialog.CreateButtons(wxOK|wxCANCEL);

    wxWindow* pParentWnd = static_cast<wxWindow*>(PropertySheetDialog.GetBookCtrl());
    // Add page
    wxXmlNode *pPropPageNode = pPPXmlNode->GetChildren();
    while(pPropPageNode)
    {
 		wxString sClass = pPropPageNode->GetAttribute(wxT("class"), ERR);
	    wxObject *obj = wxCreateDynamicObject(sClass);
		IPropertyPage *pPage = wxStaticCast(obj, IPropertyPage);//static_cast<IPropertyPage*>(obj);
		if(pPage != NULL)
		{
            if(pPage->Create(this, PropertySheetDialog.GetBookCtrl(), wxID_ANY))
            {
                wxPanel* panel = static_cast<wxPanel*>(pPage);
                if(panel)
                    PropertySheetDialog.GetBookCtrl()->AddPage(panel, pPage->GetPageName());
            }
        }
        pPropPageNode = pPropPageNode->GetNext();
    }

    //PropertySheetDialog.LayoutDialog();

    if(PropertySheetDialog.ShowModal() == wxID_OK)
    {
        //apply changes and exit
        for(size_t i = 0; i < PropertySheetDialog.GetBookCtrl()->GetPageCount(); ++i)
        {
            IPropertyPage *pPage = wxDynamicCast(PropertySheetDialog.GetBookCtrl()->GetPage(i), IPropertyPage);//dynamic_cast<IPropertyPage*>(PropertySheetDialog.GetBookCtrl()->GetPage(i));
            if(pPage)
                pPage->Apply();
        }
    }
}

bool wxGISApplication::SetupLog(const wxString &sLogPath, const wxString &sNamePrefix)
{
	if(sLogPath.IsEmpty())
	{
		wxLogError(_("wxGISApplication: Failed to get log folder"));
        return false;
	}

	if(!wxDirExists(sLogPath))
		wxFileName::Mkdir(sLogPath, 0777, wxPATH_MKDIR_FULL);


	wxDateTime dt(wxDateTime::Now());
	wxString logfilename = sLogPath + wxFileName::GetPathSeparator() + wxString::Format(wxT("%slog_%.4d%.2d%.2d.log"),sNamePrefix.c_str(), dt.GetYear(), dt.GetMonth() + 1, dt.GetDay());

    if(m_LogFile.IsOpened())
        m_LogFile.Close();

	if(!m_LogFile.Open(logfilename.GetData(), wxT("a+")))
		wxLogError(_("wxGISApplication: Failed to open log file %s"), logfilename.c_str());

	wxLog::SetActiveTarget(new wxLogStderr(m_LogFile.fp()));

#ifdef WXGISPORTABLE
	wxLogMessage(wxT("Portable"));
#endif
	wxLogMessage(wxT(" "));
	wxLogMessage(wxT("####################################################################"));
	wxLogMessage(wxT("##                    %s                    ##"),wxNow().c_str());
	wxLogMessage(wxT("####################################################################"));
    wxLongLong nFreeMem = wxGetFreeMemory();
    wxString sFreeMem = wxFileName::GetHumanReadableSize(wxULongLong(nFreeMem.GetHi(), nFreeMem.GetLo()));
//	long dFreeMem =  wxMemorySize(wxGetFreeMemory() / 1048576).ToLong();
	wxLogMessage(_("HOST '%s': OS desc - %s, free memory - %s"), wxGetFullHostName().c_str(), wxGetOsDescription().c_str(), sFreeMem.c_str());
	wxLogMessage(_("wxGISApplication: %s %s is initializing..."), GetAppName().c_str(), GetAppVersionString().c_str());
	wxLogMessage(_("wxGISApplication: Log file: %s"), logfilename.c_str());

    return true;
}

bool wxGISApplication::SetupSys(const wxString &sSysPath)
{
	//setup sys dir
	if(!wxDirExists(sSysPath))
	{
		wxLogError(wxString::Format(_("wxGISCatalogApp: System folder is absent! Lookup path '%s'"), sSysPath.c_str()));
		return false;
	}
    return true;
}

bool wxGISApplication::SetupLoc(const wxString &sLoc, const wxString &sLocPath)
{
    wxLogMessage(_("wxGISApplication: Initialize locale"));

 //   if(m_pszOldLocale != NULL)
	//	setlocale(LC_NUMERIC, m_pszOldLocale);
	//wxDELETE(m_pszOldLocale);
    wxDELETE(m_pLocale);

	//init locale
    if ( !sLoc.IsEmpty() )
    {
		int iLocale(0);
		const wxLanguageInfo* loc_info = wxLocale::FindLanguageInfo(sLoc);
		if(loc_info != NULL)
		{
			iLocale = loc_info->Language;
			wxLogMessage(_("wxGISApplication: Language is set to %s"), loc_info->Description.c_str());
		}

        // don't use wxLOCALE_LOAD_DEFAULT flag so that Init() doesn't return
        // false just because it failed to load wxstd catalog

        m_pLocale = new wxLocale();
        if ( !m_pLocale->Init(iLocale) )
        {
            wxLogError(wxT("wxGISApplication: This language is not supported by the system."));
            return false;
        }
    }

	//m_locale.Init(wxLANGUAGE_DEFAULT);

    // normally this wouldn't be necessary as the catalog files would be found
    // in the default locations, but when the program is not installed the
    // catalogs are in the build directory where we wouldn't find them by
    // default
	wxString sLocalePath = sLocPath + wxFileName::GetPathSeparator() + sLoc;
	if(wxDirExists(sLocalePath))
	{
		wxLocale::AddCatalogLookupPathPrefix(sLocalePath);

		// Initialize the catalogs we'll be using
		//load multicat from locale
		wxArrayString trans_arr;
		wxDir::GetAllFiles(sLocalePath, &trans_arr, wxT("*.mo"));

		for(size_t i = 0; i < trans_arr.size(); ++i)
		{
			wxFileName name(trans_arr[i]);
			m_pLocale->AddCatalog(name.GetName());
		}

		// this catalog is installed in standard location on Linux systems and
		// shows that you may make use of the standard message catalogs as well
		//
		// if it's not installed on your system, it is just silently ignored
	#ifdef __LINUX__
		{
			wxLogNull noLog;
			m_pLocale->AddCatalog(_T("fileutils"));
		}
	#endif
	}

	//support of dot in doubles and floats
	//m_pszOldLocale = strdup(setlocale(LC_NUMERIC, NULL));
 //   if( setlocale(LC_NUMERIC,"C") == NULL )
 //       m_pszOldLocale = NULL;

    m_sDecimalPoint = wxLocale::GetInfo(wxLOCALE_DECIMAL_POINT, wxLOCALE_CAT_NUMBER);

    return true;
}

void wxGISApplication::LoadToolbars(wxXmlNode* pRootNode)
{
	if(pRootNode)
	{
		wxXmlNode *child = pRootNode->GetChildren();
		while(child)
		{
			bool bAdd = true;
			wxString sName = child->GetAttribute(wxT("name"), wxT(""));
			for(size_t i = 0; i < m_CommandBarArray.size(); ++i)
			{
				if(m_CommandBarArray[i]->GetName() == sName)
				{
					bAdd = false;
					break;
				}
			}
			if(bAdd)
			{
				wxString sCaption = child->GetAttribute(wxT("caption"), wxT("No Title"));
				wxGISToolBar* pTB = new wxGISToolBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxAUI_TB_DEFAULT_STYLE | wxAUI_TB_OVERFLOW, sName, sCaption, enumGISCBToolbar);
				pTB->Serialize(this, child, false);
				pTB->Reference();
				m_CommandBarArray.Add(pTB);
			}
			child = child->GetNext();
		}
	}
}

void wxGISApplication::SerializeCommandBars(bool bSave)
{
	wxGISAppConfig oConfig = GetConfig();
	if(!oConfig.IsOk())
		return;

	wxXmlNode* pMenuesNode = oConfig.GetConfigNode(enumGISHKCU, GetAppName() + wxString(wxT("/frame/menues")));
	wxXmlNode* pToolbarsNode = oConfig.GetConfigNode(enumGISHKCU, GetAppName() + wxString(wxT("/frame/toolbars")));

	if(bSave)
	{
		if(!pMenuesNode)
			pMenuesNode = oConfig.CreateConfigNode(enumGISHKCU, GetAppName() + wxString(wxT("/frame/menues")));
		else
			oConfig.DeleteNodeChildren(pMenuesNode);

		if(!pToolbarsNode)
			pToolbarsNode = oConfig.CreateConfigNode(enumGISHKCU, GetAppName() + wxString(wxT("/frame/toolbars")));
		else
			oConfig.DeleteNodeChildren(pToolbarsNode);


        for(size_t i = m_CommandBarArray.GetCount(); i > 0; i--)
		{
			////skip wxGISToolBarMenu
			//if(m_CommandBarArray[i - 1]->GetName() == TOOLBARMENUNAME)
			//	continue;
			switch(m_CommandBarArray[i - 1]->GetType())
			{
			case enumGISCBMenubar:
			case enumGISCBContextmenu:
				{
					wxXmlNode* pNewNode = new wxXmlNode(pMenuesNode, wxXML_ELEMENT_NODE, wxString(wxT("menu")));
					m_CommandBarArray[i - 1]->Serialize(this, pNewNode, bSave);
				}
				break;
			case enumGISCBToolbar:
				{
					wxXmlNode* pNewNode = new wxXmlNode(pToolbarsNode, wxXML_ELEMENT_NODE, wxString(wxT("toolbar")));
					m_CommandBarArray[i - 1]->Serialize(this, pNewNode, bSave);
				}
				break;
			case enumGISCBSubMenu:
			case enumGISCBNone:
			default:
				break;
			}
		}
	}
	else
	{

		//create wxGISToolBarMenu
		wxGISToolBarMenu* pToolBarMenu = new wxGISToolBarMenu();
		pToolBarMenu->OnCreate(this);
		pToolBarMenu->Reference();
		m_CommandBarArray.Add(pToolBarMenu);
		pToolBarMenu->Reference();
		m_CommandArray.Add(pToolBarMenu);

		//load from current user config first & local machine config last
		//load menues
		LoadMenues(pMenuesNode);
		pMenuesNode = oConfig.GetConfigNode(enumGISHKLM, GetAppName() + wxString(wxT("/frame/menues")));
		LoadMenues(pMenuesNode);

		//load toolbars
		LoadToolbars(pToolbarsNode);
		pToolbarsNode = oConfig.GetConfigNode(enumGISHKLM, GetAppName() + wxString(wxT("/frame/toolbars")));
		LoadToolbars(pToolbarsNode);

		pToolBarMenu->Update();
	}
}

