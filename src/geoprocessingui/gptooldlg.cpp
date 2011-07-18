/******************************************************************************
 * Project:  wxGIS (GIS Toolbox)
 * Purpose:  tool dialog class.
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

#include "wxgis/geoprocessingui/gptooldlg.h"
//#include "wxgis/geoprocessingui/gptasksview.h"
//#include "wxgis/framework/application.h"
#include "wxgis/core/globalfn.h"

#include "../../art/tool_16.xpm"

#include "wx/icon.h"

///////////////////////////////////////////////////////////////////////////
BEGIN_EVENT_TABLE(wxGISGPToolDlg, wxFrame)
	EVT_BUTTON(wxID_HELP, wxGISGPToolDlg::OnHelp)
	EVT_UPDATE_UI(wxID_HELP, wxGISGPToolDlg::OnHelpUI)
	EVT_BUTTON(wxID_CANCEL, wxGISGPToolDlg::OnCancel)
	EVT_BUTTON(wxID_OK, wxGISGPToolDlg::OnOk)
	EVT_UPDATE_UI(wxID_OK, wxGISGPToolDlg::OnOkUI)
END_EVENT_TABLE()

wxGISGPToolDlg::wxGISGPToolDlg(wxGxRootToolbox* pGxRootToolbox, IGPToolSPtr pTool, IGPCallBack* pCallBack, bool bSync, wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxFrame( parent, id, title, pos, size, style )
{
    IFrameApplication* pApp = dynamic_cast<IFrameApplication*>(GetApplication());
    pApp->RegisterChildWindow(this);

    m_pTool = pTool;
	m_pPropNode = pGxRootToolbox->GetAttributes();
    m_pGxRootToolbox = pGxRootToolbox;
    m_pCallBack = pCallBack;
    m_bSync = bSync;


#ifdef __WXMSW__
	SetIcon( wxIcon(tool_16_xpm) );
	this->SetSizeHints( wxSize( 350,500 ) );
//	SetLabel(m_pTool->GetDisplayName());
#endif

	//this->SetSizeHints( wxDefaultSize, wxDefaultSize );
    m_DataWidth = 350;
    m_HtmlWidth = 150;

	wxBoxSizer* bMainSizer = new wxBoxSizer( wxVERTICAL );

	m_splitter = new wxSplitterWindow( this, SASHCTRLID, wxDefaultPosition, wxDefaultSize, 0 );
	m_splitter->Connect( wxEVT_IDLE, wxIdleEventHandler( wxGISGPToolDlg::m_splitterOnIdle ), NULL, this );

	m_toolpanel = new wxPanel( m_splitter, wxID_ANY, wxDefaultPosition, wxSize(m_DataWidth, size.y)/*wxDefaultSize*/, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer2 = new wxBoxSizer( wxVERTICAL );

    //m_tools = new wxPanel( m_toolpanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL/*|wxVSCROLL*/|wxBORDER_SUNKEN );
    wxScrolledWindow* m_tools = new wxScrolledWindow(m_toolpanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL|wxBORDER_SUNKEN|wxVSCROLL );
    m_tools->SetScrollbars(20, 20, 50, 50);

	wxBoxSizer* bSizer4;
	bSizer4 = new wxBoxSizer( wxVERTICAL );

    GPParameters* pParams = m_pTool->GetParameterInfo();
    if(pParams)
    {
        for(size_t i = 0; i < pParams->size(); ++i)
        {
            IGPParameter* pParam = pParams->operator[](i);
            if(!pParam)
            {
                m_pControlsArray.push_back(NULL);
                continue;
            }
            switch(pParam->GetDataType())
            {
            case enumGISGPParamDTPath:
                {
                    wxGISDTPath* pPath = new wxGISDTPath(pParam, m_pTool->GetCatalog(), m_tools);
                    bSizer4->Add( pPath, 0, wxEXPAND, 5 );
                    m_pControlsArray.push_back(pPath);
                }
                break;
            case enumGISGPParamDTSpatRef:
                {
                    wxGISDTSpatRef* pSpatRef = new wxGISDTSpatRef(pParam, m_pTool->GetCatalog(), m_tools);
                    bSizer4->Add( pSpatRef, 0, wxEXPAND, 5 );
                    m_pControlsArray.push_back(pSpatRef);
                }
                break;
            case enumGISGPParamDTString:
            case enumGISGPParamDTInteger:
            case enumGISGPParamDTDouble:
                {
                    wxGISDTDigit* poDigit = new wxGISDTDigit(pParam, m_pTool->GetCatalog(), m_tools);
                    bSizer4->Add( poDigit, 0, wxEXPAND, 5 );
                    m_pControlsArray.push_back(poDigit);
                }
                break;
            case enumGISGPParamDTStringChoice:
            case enumGISGPParamDTIntegerChoice:
            case enumGISGPParamDTDoubleChoice:
                {
                    wxGISDTChoice* poChoice = new wxGISDTChoice(pParam, m_pTool->GetCatalog(), m_tools);
                    bSizer4->Add( poChoice, 0, wxEXPAND, 5 );
                    m_pControlsArray.push_back(poChoice);
                }
                break;
		   case enumGISGPParamDTStringList:
		   case enumGISGPParamDTIntegerList:
		   case enumGISGPParamDTDoubleList:
                {
                    wxGISDTList* poList = new wxGISDTList(pParam, m_pTool->GetCatalog(), m_tools);
                    bSizer4->Add( poList, 0, wxEXPAND, 5 );
                    m_pControlsArray.push_back(poList);
                }
                break;
           case enumGISGPParamDTBool:
                {
                    wxGISDTBool* poCheck = new wxGISDTBool(pParam, m_pTool->GetCatalog(), m_tools);
                    bSizer4->Add( poCheck, 0, wxEXPAND, 5 );
                    m_pControlsArray.push_back(poCheck);
                }
                break;
            case enumGISGPParamDTParamArray:
                {
                    wxGISDTMultiParam* poMultiParam = new wxGISDTMultiParam(pParam, m_pTool->GetCatalog(), m_tools);
                    bSizer4->Add( poMultiParam, 0, wxEXPAND, 5 );
                    m_pControlsArray.push_back(poMultiParam);
                }
                break;
            case enumGISGPParamDTQuery:
                {
					wxGISSQLQueryCtrl* poSQLQueryParam = new wxGISSQLQueryCtrl(pParam, m_pTool->GetCatalog(), m_tools);
                    bSizer4->Add( poSQLQueryParam, 0, wxEXPAND, 5 );
                    m_pControlsArray.push_back(poSQLQueryParam);
                }
                break;
            default:
                m_pControlsArray.push_back(NULL);
                break;
            }
        }
    }

    m_tools->SetSizer( bSizer4 );
	m_tools->Layout();
	bSizer4->Fit( m_tools );

	bSizer2->Add( m_tools/*bSizer4*/, 1, wxEXPAND, 5 );

	m_sdbSizer1 = new wxStdDialogButtonSizer();
	m_sdbSizer1OK = new wxButton( m_toolpanel, wxID_OK, wxString(_("OK")) );
    m_sdbSizer1OK->Enable(false);
	m_sdbSizer1->AddButton( m_sdbSizer1OK );
	m_sdbSizer1Cancel = new wxButton( m_toolpanel, wxID_CANCEL, wxString(_("Cancel")) );
	m_sdbSizer1->AddButton( m_sdbSizer1Cancel );
	m_sdbSizer1Help = new wxButton( m_toolpanel, wxID_HELP, wxString(_("Show Help")) );
    m_sdbSizer1Help->Enable(false);
	m_sdbSizer1->AddButton( m_sdbSizer1Help );
	m_sdbSizer1->Realize();
	bSizer2->Add( m_sdbSizer1, 0, wxEXPAND|wxALL, 5 );

	m_toolpanel->SetSizer( bSizer2 );
	m_toolpanel->Layout();
	bSizer2->Fit( m_toolpanel );

	//m_helppanel = new wxPanel( m_splitter, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	//wxBoxSizer* bSizer3;
	//bSizer3 = new wxBoxSizer( wxVERTICAL );

	m_htmlWin = new wxHtmlWindow( m_splitter/*m_helppanel*/, wxID_ANY, wxDefaultPosition, wxSize(m_HtmlWidth, size.y) /*wxDefaultSize*/, wxHW_SCROLLBAR_AUTO | wxBORDER_THEME  );
	//bSizer3->Add( m_htmlWin2, 1, wxEXPAND, 5 );

	//m_helppanel->SetSizer( bSizer3 );
	//m_helppanel->Layout();
	//bSizer3->Fit( m_helppanel );

	m_splitter->SetSashGravity(1.0);
	//m_splitter1->SplitVertically(m_commandbarlist, m_buttonslist, 100);


	m_splitter->SplitVertically( m_toolpanel, m_htmlWin/*m_helppanel*/, m_DataWidth );

	bMainSizer->Add( m_splitter, 1, wxEXPAND, 5 );

	this->SetSizer( bMainSizer );
	this->Layout();


    m_splitter->Unsplit(m_htmlWin);
    wxSize DlgSize = size;
    DlgSize.x = m_DataWidth;
    SetSize(DlgSize);

    SerializeFramePos(false);
}

wxGISGPToolDlg::~wxGISGPToolDlg()
{
    SerializeFramePos(true);
}

void wxGISGPToolDlg::OnHelp(wxCommandEvent& event)
{
    wxSize DlgSize = GetSize();
    wxSize DataSize = m_toolpanel->GetSize();
    if(m_splitter->IsSplit())
    {
        wxSize HtmlSize = m_htmlWin->GetSize();
        m_HtmlWidth = HtmlSize.x;
        DlgSize.x = DataSize.x;
        SetSize(DlgSize);
        m_splitter->Unsplit(m_htmlWin);
    }
    else
    {
        DlgSize.x += m_HtmlWidth;
        SetSize(DlgSize);
        m_splitter->SetSashGravity(1.0);
        m_splitter->SplitVertically( m_toolpanel, m_htmlWin, DataSize.x);
    }
}

void wxGISGPToolDlg::OnHelpUI(wxUpdateUIEvent& event)
{
    event.SetText(m_splitter->IsSplit() == true ? _("Hide Help") : _("Show Help"));
}

void wxGISGPToolDlg::OnOk(wxCommandEvent& event)
{
    //event.Skip();

    //wxGxTaskExecDlg dlg(pGPToolManager, pTool, pParentWnd, wxID_ANY);
    //dlg.ShowModal();

    //IApplication* pApp = ::GetApplication();
    ////create panel - get progress with messages
    //ITrackCancel* pTrackCancel(NULL);
    //IGPCallBack* pCallBack(NULL);
    //wxGxTasksView* pGxTasksView(NULL);

    //WINDOWARRAY* pWndArr = pApp->GetChildWindows();
    //if(pWndArr)
    //{
    //    for(size_t i = 0; i < pWndArr->size(); ++i)
    //    {
    //        wxWindow* pWnd = pWndArr->operator[](i);
    //        if(!pWnd)
    //            continue;
    //        pGxTasksView = dynamic_cast<wxGxTasksView*>(pWnd);
    //        if(!pGxTasksView)
    //            continue;
    //        break;
    //    }
    //}

    //wxGxTaskPanel* pGxTaskPanel;
    //if(pGxTasksView)
    //{
    //    pGxTaskPanel = new wxGxTaskPanel(m_pToolManager, m_pTool, pGxTasksView);

    //    pTrackCancel = dynamic_cast<ITrackCancel*>(pGxTaskPanel);
    //    pCallBack = dynamic_cast<IGPCallBack*>(pGxTaskPanel);

    //    pGxTasksView->InsertPanel(pGxTaskPanel);
    //}

    ////mngr - run execute
    ////long nThreadId = m_pToolManager->OnExecute(m_pTool, pTrackCancel, pCallBack);
    //if(pGxTasksView)
    //{
    //    //pGxTaskPanel->SetTaskThreadId(nThreadId);
    //    pGxTaskPanel->SetToolDialog(GetParent(), m_pPropNode);
    //}
    ////to prevent destroy tool in this dialog (the tool should destroy in panel)
    //m_pTool = NULL;

    IFrameApplication* pApp = dynamic_cast<IFrameApplication*>(GetApplication());
    pApp->UnRegisterChildWindow(this);

    wxWindow* pParentWnd = this->GetParent();
    pParentWnd->SetFocus();
    this->Destroy();

    //begin execution
    m_pGxRootToolbox->OnExecuteTool(pParentWnd, m_pTool, m_pCallBack, m_bSync);
}

void wxGISGPToolDlg::OnCancel(wxCommandEvent& event)
{
    IFrameApplication* pApp = dynamic_cast<IFrameApplication*>(GetApplication());
    pApp->UnRegisterChildWindow(this);

    this->GetParent()->SetFocus();
    this->Destroy();
}

void wxGISGPToolDlg::OnOkUI(wxUpdateUIEvent& event)
{
    //TODO: Fix tools state
    event.Enable(false);
	wxBusyCursor wait;
    //internal control validate
    for(size_t i = 0; i < m_pControlsArray.size(); ++i)
    {
        if(m_pControlsArray[i])
            m_pControlsArray[i]->Validate();
    }

    //tool validate
    bool bIsValid = m_pTool->Validate();

    short nNonValid(0);
    GPParameters* pParams = m_pTool->GetParameterInfo();
    if(!pParams)
        return;
    //update controls state
    for(size_t i = 0; i < pParams->size(); ++i)
    {
        IGPParameter* pParam = pParams->operator[](i);
        if(!pParam)
            continue;
        if(!pParam->GetIsValid())
            nNonValid++;
        if(pParam->GetHasBeenValidated())
            continue;
        if(i < m_pControlsArray.size() && m_pControlsArray[i])
        {
            if(bIsValid)
                m_pControlsArray[i]->Validate();
            m_pControlsArray[i]->Update();
        }
        pParam->SetHasBeenValidated(true);
    }
    if(nNonValid > 0)
        return;
    event.Enable(true);
}

void wxGISGPToolDlg::SerializeFramePos(bool bSave)
{
	if(!m_pPropNode)
		return;
	if(bSave)
	{

		if( IsMaximized() )
		{
			if(m_pPropNode->HasAttribute(wxT("maxi")))
				m_pPropNode->DeleteAttribute(wxT("maxi"));
			m_pPropNode->AddAttribute(wxT("maxi"), wxT("1"));
		}
		else
		{
			int x, y, w, h;
			GetClientSize(&w, &h);
			GetPosition(&x, &y);

			if(m_pPropNode->HasAttribute(wxT("Height")))
				m_pPropNode->DeleteAttribute(wxT("Height"));
			m_pPropNode->AddAttribute(wxT("Height"), wxString::Format(wxT("%u"), h));
			if(m_pPropNode->HasAttribute(wxT("Width")))
				m_pPropNode->DeleteAttribute(wxT("Width"));
			m_pPropNode->AddAttribute(wxT("Width"), wxString::Format(wxT("%u"), w));
			if(m_pPropNode->HasAttribute(wxT("YPos")))
				m_pPropNode->DeleteAttribute(wxT("YPos"));
			m_pPropNode->AddAttribute(wxT("YPos"), wxString::Format(wxT("%d"), y));
			if(m_pPropNode->HasAttribute(wxT("XPos")))
				m_pPropNode->DeleteAttribute(wxT("XPos"));
			m_pPropNode->AddAttribute(wxT("XPos"), wxString::Format(wxT("%d"), x));
			if(m_pPropNode->HasAttribute(wxT("maxi")))
				m_pPropNode->DeleteAttribute(wxT("maxi"));
			m_pPropNode->AddAttribute(wxT("maxi"), wxT("0"));
		}
	}
	else
	{
		//load
		bool bMaxi = wxAtoi(m_pPropNode->GetAttribute(wxT("maxi"), wxT("0"))) != 0;
		if(!bMaxi)
		{
			int x = wxAtoi(m_pPropNode->GetAttribute(wxT("XPos"), wxT("50")));
			int y = wxAtoi(m_pPropNode->GetAttribute(wxT("YPos"), wxT("50")));
			int w = wxAtoi(m_pPropNode->GetAttribute(wxT("Width"), wxT("850")));
			int h = wxAtoi(m_pPropNode->GetAttribute(wxT("Height"), wxT("530")));

			Move(x, y);
			SetClientSize(w, h);
		}
		else
		{
			Maximize();
		}
	}
}
