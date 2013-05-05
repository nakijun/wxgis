/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGISIdentifyDlg class - dialog/dock window with the results of identify.
 * Author:   Baryshnikov Dmitriy (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2011-2013 Bishop
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
#include "wxgis/cartoui/identifydlg.h"
#include "wxgis/core/config.h"

#include <wx/clipbrd.h>
#include <cmath>

#include "../../art/splitter_switch.xpm"
#include "../../art/layers.xpm"
#include "../../art/layer16.xpm"
#include "../../art/id.xpm"
#include "../../art/small_arrow.xpm"

int wxCALLBACK FieldValueCompareFunction(wxIntPtr item1, wxIntPtr item2, wxIntPtr sortData)
{
    LPFIELDSORTDATA psortdata = (LPFIELDSORTDATA)sortData;
    if(psortdata->nSortAsc == 0)
    {
        return item1 - item2;
    }
    else
    {
        wxString str1, str2;
        if(psortdata->currentSortCol == 0)
        {
            str1 = psortdata->Feature.GetFieldName(item1);
            str2 = psortdata->Feature.GetFieldName(item2);
        }
        else
        {
            str1 = psortdata->Feature.GetFieldAsString(item1);
            str2 = psortdata->Feature.GetFieldAsString(item2);
        }
        return str1.CmpNoCase(str2) * psortdata->nSortAsc;
    }
}
//-------------------------------------------------------------------
// wxGISFeatureDetailsPanel
//-------------------------------------------------------------------

BEGIN_EVENT_TABLE(wxGISFeatureDetailsPanel, wxPanel)
    EVT_CONTEXT_MENU(wxGISFeatureDetailsPanel::OnContextMenu)
	EVT_MENU_RANGE(ID_WG_COPY_NAME, ID_WG_RESET_SORT, wxGISFeatureDetailsPanel::OnMenu)
 	EVT_UPDATE_UI_RANGE(ID_WG_COPY_NAME, ID_WG_RESET_SORT, wxGISFeatureDetailsPanel::OnMenuUpdateUI)
    EVT_LIST_COL_CLICK(ID_LISTCTRL, wxGISFeatureDetailsPanel::OnColClick)
	EVT_BUTTON(ID_MASKBTN, wxGISFeatureDetailsPanel::OnMaskMenu)
END_EVENT_TABLE()

wxGISFeatureDetailsPanel::wxGISFeatureDetailsPanel( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	wxImage img = wxBitmap(small_arrow_xpm).ConvertToImage();
	wxBitmap oDownArrow = img.Rotate90();
	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxVERTICAL );

	wxFlexGridSizer* fgSizer1;
	fgSizer1 = new wxFlexGridSizer( 1, 3, 0, 0 );
	fgSizer1->AddGrowableCol( 1 );
	fgSizer1->SetFlexibleDirection( wxBOTH );
	fgSizer1->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	m_staticText1 = new wxStaticText( this, wxID_ANY, _("Location:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1->Wrap( -1 );
	fgSizer1->Add( m_staticText1, 1, wxALIGN_CENTER_VERTICAL|wxLEFT, 5 );

	m_textCtrl = new wxTextCtrl(  this, wxID_ANY, wxT("..."), wxDefaultPosition, wxDefaultSize, wxTE_READONLY, wxGenericValidator( &m_sLocation ));
	//m_textCtrl->Enable( false );

	fgSizer1->Add( m_textCtrl, 1, wxALIGN_CENTER|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxEXPAND|wxRIGHT|wxLEFT, 5 );

	m_bpSelStyleButton = new wxBitmapButton( this, ID_MASKBTN, oDownArrow, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
	m_bpSelStyleButton->SetToolTip( _("Select location text style") );

	fgSizer1->Add( m_bpSelStyleButton, 0, wxRIGHT, 5 );

	bSizer1->Add( fgSizer1, 0, wxEXPAND, 5 );

	m_listCtrl = new wxListCtrl( this, ID_LISTCTRL, wxDefaultPosition, wxDefaultSize, wxLC_REPORT | wxLC_HRULES | wxLC_VRULES );
	m_listCtrl->InsertColumn(0, _("Field"), wxLIST_FORMAT_LEFT, 90);
	m_listCtrl->InsertColumn(1, _("Value"), wxLIST_FORMAT_LEFT, 120);

	m_ImageListSmall.Create(16, 16);

    wxBitmap SmallA(small_arrow_xpm);// > arrow
    wxImage SmallImg = SmallA.ConvertToImage();
    SmallImg = SmallImg.Rotate90();
    wxBitmap SmallDown(SmallImg);
    SmallImg = SmallImg.Mirror(false);
    wxBitmap SmallUp(SmallImg);

	m_ImageListSmall.Add(wxBitmap(SmallDown));
	m_ImageListSmall.Add(wxBitmap(SmallUp));

	m_listCtrl->SetImageList(&m_ImageListSmall, wxIMAGE_LIST_SMALL);

    m_listCtrl->Bind(wxEVT_LEFT_DOWN, &wxGISFeatureDetailsPanel::OnMouseLeftUp, this);
    m_listCtrl->Bind(wxEVT_SET_CURSOR, &wxGISFeatureDetailsPanel::OnSetCursor, this);

	bSizer1->Add( m_listCtrl, 1, wxALL|wxEXPAND, 5 );

	this->SetSizer( bSizer1 );
	this->Layout();
	m_pCFormat = new wxGISCoordinatesFormatMenu();
    wxString sMask( wxT("X: dd.dddd[ ]Y: dd.dddd") );
	wxGISAppConfig oConfig = GetConfig();
    m_sAppName = GetApplication()->GetAppName();
	if(oConfig.IsOk())
    {
        sMask = oConfig.Read(enumGISHKCU, m_sAppName + wxString(wxT("/identifydlg/format_mask")), sMask);
    }

	m_pCFormat->Create(sMask);

	m_pMenu = new wxMenu;
	m_pMenu->Append(ID_WG_COPY_NAME, wxString::Format(_("Copy %s"), _("Field")), wxString::Format(_("Copy '%s' value"), _("Field")), wxITEM_NORMAL);
	m_pMenu->Append(ID_WG_COPY_VALUE, wxString::Format(_("Copy %s"), _("Value")), wxString::Format(_("Copy '%s' value"), _("Value")), wxITEM_NORMAL);
	m_pMenu->Append(ID_WG_COPY, wxString(_("Copy")), wxString(_("Copy")), wxITEM_NORMAL);
	m_pMenu->Append(ID_WG_HIDE, wxString(_("Hide")), wxString(_("Hide rows")), wxITEM_NORMAL);
	m_pMenu->AppendSeparator();
	m_pMenu->Append(ID_WG_RESET_SORT, wxString(_("Remove sort")), wxString(_("Remove sort")), wxITEM_NORMAL);
	m_pMenu->Append(ID_WG_RESET_HIDE, wxString(_("Show all rows")), wxString(_("Show all rows")), wxITEM_NORMAL);

    m_currentSortCol = 0;
    m_nSortAsc = 0;
	m_dfX = 0;
	m_dfY =	0;
}

wxGISFeatureDetailsPanel::~wxGISFeatureDetailsPanel()
{
	wxGISAppConfig oConfig = GetConfig();
	if(oConfig.IsOk() && m_pCFormat)
    {
        oConfig.Write(enumGISHKCU, m_sAppName + wxString(wxT("/identifydlg/format_mask")), m_pCFormat->GetMask());
    }
	wxDELETE(m_pMenu);
	wxDELETE(m_pCFormat);
}

void wxGISFeatureDetailsPanel::FillPanel(const OGRPoint *pPt)
{
    if(!pPt)
        return;
    //TODO: OGRPoint has spatial reference. This should be used in m->deg and vice versa transform
	m_dfX = pPt->getX();
	m_dfY = pPt->getY();
	m_sLocation = m_pCFormat->Format(m_dfX, m_dfY);
	TransferDataToWindow();
}

void wxGISFeatureDetailsPanel::FillPanel(wxGISFeature &Feature)
{
	m_Feature = Feature;
	Clear();
	for(int i = 0; i < Feature.GetFieldCount(); ++i)
	{
		if(m_anExcludeFields.Index(i) != wxNOT_FOUND)
			continue;

        wxString sName = Feature.GetFieldName(i);

		long pos = m_listCtrl->InsertItem(i, sName, 2);

        wxListItem item_val;
        item_val.SetColumn(1);
        wxString sValue = Feature.GetFieldAsString(i);
        item_val.SetText(sValue);
        item_val.SetId(pos);
		m_listCtrl->SetItem(item_val);

        if(IsURL(sValue))
        {
            wxFont Font = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
            Font.SetUnderlined(true);
            m_listCtrl->SetItemFont(pos, Font);
            m_listCtrl->SetItemTextColour(pos, wxSystemSettings::GetColour(wxSYS_COLOUR_HOTLIGHT));
        }
        else if(IsLocalURL(sValue))
        {
            wxFont Font = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
            Font.SetUnderlined(true);
            m_listCtrl->SetItemFont(pos, Font);
            m_listCtrl->SetItemTextColour(pos, wxSystemSettings::GetColour(wxSYS_COLOUR_GRAYTEXT));
        }
		m_listCtrl->SetItemData(pos, (long)i);
	}
	//m_listCtrl->Update();
    FIELDSORTDATA sortdata = {m_nSortAsc, m_currentSortCol, m_Feature};
	m_listCtrl->SortItems(FieldValueCompareFunction, (long)&sortdata);
}

void wxGISFeatureDetailsPanel::Clear(bool bFull)
{
	m_listCtrl->DeleteAllItems();
	if(bFull)
		m_anExcludeFields.Clear();
}

void wxGISFeatureDetailsPanel::OnContextMenu(wxContextMenuEvent& event)
{
	wxRect rc = m_listCtrl->GetRect();
	wxPoint point = event.GetPosition();
    // If from keyboard
    if (point.x == -1 && point.y == -1)
	{
        wxSize size = GetSize();
		point.x = rc.GetLeft() + rc.GetWidth() / 2;
		point.y = rc.GetTop() + rc.GetHeight() / 2;
    }
	else
	{
        point = m_listCtrl->ScreenToClient(point);
    }
	if(!rc.Contains(point))
	{
		event.Skip();
		return;
	}
    PopupMenu(m_pMenu, point.x, point.y);
}

void wxGISFeatureDetailsPanel::OnMaskMenu(wxCommandEvent& event)
{
	wxRect rc = m_bpSelStyleButton->GetRect();
	m_pCFormat->PrepareMenu();//fill new masks from config
	PopupMenu(m_pCFormat, rc.GetBottomLeft());
	m_sLocation = m_pCFormat->Format(m_dfX, m_dfY);
	TransferDataToWindow();
}

void wxGISFeatureDetailsPanel::WriteStringToClipboard(const wxString &sData)
{
	// Write some text to the clipboard
    if (wxTheClipboard->Open())
    {
        // This data objects are held by the clipboard,
        // so do not delete them in the app.
        wxTheClipboard->SetData( new wxTextDataObject(sData) );
        wxTheClipboard->Close();
    }
}

void wxGISFeatureDetailsPanel::OnMenu(wxCommandEvent& event)
{
    long nItem = wxNOT_FOUND;
	wxString sOutput;
	switch(event.GetId())
	{
	case ID_WG_COPY_NAME:
		for ( ;; )
		{
			nItem = m_listCtrl->GetNextItem(nItem, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
			if ( nItem == wxNOT_FOUND )
				break;
			sOutput += m_listCtrl->GetItemText(nItem, 0);
			sOutput += wxT("\n");
		}
		WriteStringToClipboard(sOutput);
		break;
	case ID_WG_COPY_VALUE:
		for ( ;; )
		{
			nItem = m_listCtrl->GetNextItem(nItem, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
			if ( nItem == wxNOT_FOUND )
				break;
			sOutput += m_listCtrl->GetItemText(nItem, 1);
			sOutput += wxT("\n");
		}
		WriteStringToClipboard(sOutput);
		break;
	case ID_WG_COPY:
		for ( ;; )
		{
			nItem = m_listCtrl->GetNextItem(nItem, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
			if ( nItem == wxNOT_FOUND )
				break;
			sOutput += m_listCtrl->GetItemText(nItem, 0);
			sOutput += wxT("\t");
			sOutput += m_listCtrl->GetItemText(nItem, 1);
			sOutput += wxT("\n");
		}
		WriteStringToClipboard(sOutput);
		break;
	case ID_WG_HIDE:
		for ( ;; )
		{
			nItem = m_listCtrl->GetNextItem(nItem, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
			if ( nItem == wxNOT_FOUND )
				break;
	        int nFieldNo = (int)m_listCtrl->GetItemData(nItem);
			m_anExcludeFields.Add(nFieldNo);
		}
		FillPanel(m_Feature);
		break;
	case ID_WG_RESET_SORT:
        {
            m_nSortAsc = 0;

            wxListItem item;
            item.SetMask(wxLIST_MASK_IMAGE);

            //reset image
            item.SetImage(wxNOT_FOUND);
            for(size_t i = 0; i < m_listCtrl->GetColumnCount(); ++i)
                m_listCtrl->SetColumn(i, item);

            FIELDSORTDATA sortdata = {m_nSortAsc, m_currentSortCol, m_Feature};
	        m_listCtrl->SortItems(FieldValueCompareFunction, (long)&sortdata);
        }
		break;
	case ID_WG_RESET_HIDE:
		m_anExcludeFields.Clear();
		FillPanel(m_Feature);
		break;
	default:
		break;
	}
}

void wxGISFeatureDetailsPanel::OnMenuUpdateUI(wxUpdateUIEvent& event)
{
	if(event.GetId() == ID_WG_RESET_SORT)
		return;
	if(m_listCtrl->GetSelectedItemCount() == 0)
		event.Enable(false);
	else
		event.Enable(true);
}

void wxGISFeatureDetailsPanel::OnColClick(wxListEvent& event)
{
    //event.Skip();
    m_currentSortCol = event.GetColumn();
    if(m_nSortAsc == 0)
        m_nSortAsc = 1;
    else
        m_nSortAsc *= -1;

    FIELDSORTDATA sortdata = {m_nSortAsc, m_currentSortCol, m_Feature};
	m_listCtrl->SortItems(FieldValueCompareFunction, (long)&sortdata);

    wxListItem item;
    item.SetMask(wxLIST_MASK_IMAGE);

    ////reset image
    //item.SetImage(wxNOT_FOUND);
    //for(size_t i = 0; i < m_listCtrl->GetColumnCount(); ++i)
    //    m_listCtrl->SetColumn(i, item);

    item.SetImage(m_nSortAsc == 1 ? 0 : 1);
    m_listCtrl->SetColumn(m_currentSortCol, item);
}

void wxGISFeatureDetailsPanel::OnSetCursor(wxSetCursorEvent& event)
{
    //event.Skip();
    wxPoint pt(event.GetX(), event.GetY());
    //pt = ClientToScreen(pt);
    //pt = m_listCtrl->ScreenToClient(pt);
    int flags;
    long item = m_listCtrl->HitTest(pt, flags);
    if (item > -1 && (flags & wxLIST_HITTEST_ONITEM))
    {
        wxListItem row_info;
        row_info.m_itemId = item;
        row_info.m_col = 1;
        row_info.m_mask = wxLIST_MASK_TEXT;
        m_listCtrl->GetItem( row_info );
        if(IsURL(row_info.m_text) || IsLocalURL(row_info.m_text))
        {
            event.SetCursor(wxCursor(wxCURSOR_HAND));
        }
    }
}

void wxGISFeatureDetailsPanel::OnMouseLeftUp(wxMouseEvent& event)
{
    event.Skip();
    wxPoint pt(event.GetX(), event.GetY());
    //pt = ClientToScreen(pt);
    //pt = m_listCtrl->ScreenToClient(pt);
    int flags;
    long item = m_listCtrl->HitTest(pt, flags);
    if (item > -1 && (flags & wxLIST_HITTEST_ONITEM))
    {
        wxListItem row_info;
        row_info.m_itemId = item;
        row_info.m_col = 1;
        row_info.m_mask = wxLIST_MASK_TEXT;
        m_listCtrl->GetItem( row_info );
        if(IsURL(row_info.m_text))
        {
            wxLaunchDefaultBrowser( row_info.m_text );
        }
        else if(IsLocalURL(row_info.m_text))
        {
            wxLaunchDefaultApplication( row_info.m_text );
        }
    }
}

bool wxGISFeatureDetailsPanel::IsURL(const wxString &sText)
{
    return sText.StartsWith(wxT("http:")) || sText.StartsWith(wxT("www.")) || sText.StartsWith(wxT("https:")) || sText.StartsWith(wxT("ftp:")) || sText.StartsWith(wxT("ftp.")) || sText.StartsWith(wxT("www2."));

}

bool wxGISFeatureDetailsPanel::IsLocalURL(const wxString &sText)
{
    return sText.StartsWith(wxT("file:"));
}

//-------------------------------------------------------------------
// wxGISIdentifyDlg
//-------------------------------------------------------------------
IMPLEMENT_DYNAMIC_CLASS(wxGISIdentifyDlg, wxPanel)

BEGIN_EVENT_TABLE(wxGISIdentifyDlg, wxPanel)
	EVT_BUTTON(wxGISIdentifyDlg::ID_SWITCHSPLIT, wxGISIdentifyDlg::OnSwitchSplit)
	EVT_TREE_SEL_CHANGED(wxGISIdentifyDlg::ID_WXGISTREECTRL, wxGISIdentifyDlg::OnSelChanged)
	EVT_MENU_RANGE(ID_WGMENU_FLASH, ID_WGMENU_ZOOM, wxGISIdentifyDlg::OnMenu)
    EVT_TREE_ITEM_RIGHT_CLICK(ID_WXGISTREECTRL, wxGISIdentifyDlg::OnItemRightClick)
	EVT_SPLITTER_DCLICK(wxID_ANY, wxGISIdentifyDlg::OnDoubleClickSash)
END_EVENT_TABLE()


wxGISIdentifyDlg::wxGISIdentifyDlg( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style )
{
	Create(parent, id, pos, size, style);
}

wxGISIdentifyDlg::wxGISIdentifyDlg(void)
{
}

bool wxGISIdentifyDlg::Create(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name)
{
	if(!wxPanel::Create( parent, id, pos, size, style, name ))
		return false;

    m_nSashPos = 150;
    wxSplitMode eMode = wxSPLIT_VERTICAL;
	wxGISAppConfig oConfig = GetConfig();
    m_sAppName = GetApplication()->GetAppName();
	if(oConfig.IsOk())
    {
        m_nSashPos = oConfig.ReadInt(enumGISHKCU, m_sAppName + wxString(wxT("/identifydlg/sashpos1")), m_nSashPos);
        eMode = (wxSplitMode)oConfig.ReadInt(enumGISHKCU, m_sAppName + wxString(wxT("/identifydlg/mode")), eMode);
    }

	m_bMainSizer = new wxBoxSizer( wxVERTICAL );

	m_fgTopSizer = new wxFlexGridSizer( 1, 3, 0, 0 );
	m_fgTopSizer->AddGrowableCol( 1 );
	m_fgTopSizer->SetFlexibleDirection( wxBOTH );
	m_fgTopSizer->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	m_staticText1 = new wxStaticText( this, wxID_ANY, _("Identify from:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1->Wrap( -1 );
	m_fgTopSizer->Add( m_staticText1, 0, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 5 );

	wxArrayString m_LayerChoiceChoices;
	m_LayerChoiceChoices.Add(_("<Top layer>"));
	m_LayerChoice = new wxChoice( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_LayerChoiceChoices, 0 );
	m_LayerChoice->SetSelection( 0 );
	m_LayerChoice->Enable(false);
	m_fgTopSizer->Add( m_LayerChoice, 0, wxALL|wxALIGN_CENTER_VERTICAL|wxEXPAND, 5 );

	m_BmpVert = wxBitmap(splitter_switch_xpm);
	wxImage oImg = m_BmpVert.ConvertToImage().Rotate90();
	m_BmpHorz = wxBitmap(oImg);
	//get splitter from conf
	m_bpSplitButton = new wxBitmapButton( this, ID_SWITCHSPLIT, m_BmpVert, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
	m_fgTopSizer->Add( m_bpSplitButton, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	m_bMainSizer->Add( m_fgTopSizer, 0, wxEXPAND, 5 );

	m_splitter = new wxSplitterWindow( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSP_3D );
	//m_splitter->Connect( wxEVT_IDLE, wxIdleEventHandler( wxGISIdentifyDlg::SplitterOnIdle ), NULL, this );
    m_splitter->Bind(wxEVT_IDLE, &wxGISIdentifyDlg::SplitterOnIdle, this);

	m_bMainSizer->Add( m_splitter, 1, wxEXPAND, 5 );

	m_TreeImageList.Create(16, 16);
	m_TreeImageList.Add(wxBitmap(layers_xpm));
	m_TreeImageList.Add(wxBitmap(layer16_xpm));
	m_TreeImageList.Add(wxBitmap(id_xpm));

    int nOSMajorVer(0);
    wxGetOsVersion(&nOSMajorVer);
	m_pTreeCtrl = new wxTreeCtrl( m_splitter, ID_WXGISTREECTRL, wxDefaultPosition, wxDefaultSize, wxTR_HAS_BUTTONS | wxTR_TWIST_BUTTONS | wxTR_HIDE_ROOT | wxSTATIC_BORDER | (nOSMajorVer > 5 ? wxTR_NO_LINES : wxTR_LINES_AT_ROOT) );
	m_pTreeCtrl->SetImageList(&m_TreeImageList);
	//m_pTreeCtrl->Connect( wxEVT_LEFT_DOWN, wxMouseEventHandler( wxGISIdentifyDlg::OnLeftDown ), NULL, this );
    m_pTreeCtrl->Bind(wxEVT_LEFT_DOWN, &wxGISIdentifyDlg::OnLeftDown, this);


	m_splitter->SetSashGravity(0.5);
	m_pFeatureDetailsPanel = new wxGISFeatureDetailsPanel(m_splitter);

	switch(eMode)
	{
	case wxSPLIT_HORIZONTAL:
		m_bpSplitButton->SetBitmap(m_BmpHorz);
		m_splitter->SplitHorizontally(m_pTreeCtrl, m_pFeatureDetailsPanel, 150);
		break;
    default:
	case wxSPLIT_VERTICAL:
		m_bpSplitButton->SetBitmap(m_BmpVert);
		m_splitter->SplitVertically(m_pTreeCtrl, m_pFeatureDetailsPanel, 150);
		break;
	};
    //m_splitter->SplitVertically(m_pTreeCtrl, m_pFeatureDetailsPanel, 150);

	this->SetSizer( m_bMainSizer );
	this->Layout();

	m_pMenu = new wxMenu;
	m_pMenu->Append(ID_WGMENU_FLASH, wxString(_("Flash")), wxString(_("Flash geometry")), wxITEM_NORMAL);
	m_pMenu->Append(ID_WGMENU_PAN, wxString(_("Pan")), wxString(_("Pan to geometry center")), wxITEM_NORMAL);
	m_pMenu->Append(ID_WGMENU_ZOOM, wxString(_("Zoom")), wxString(_("Zoom to geometry")), wxITEM_NORMAL);

    return true;
}

wxGISIdentifyDlg::~wxGISIdentifyDlg()
{
	wxGISAppConfig oConfig = GetConfig();
	if(oConfig.IsOk())
    {
        oConfig.Write(enumGISHKCU, m_sAppName + wxString(wxT("/identifydlg/sashpos1")), m_splitter->GetSashPosition());
        oConfig.Write(enumGISHKCU, m_sAppName + wxString(wxT("/identifydlg/mode")), (int)m_splitter->GetSplitMode());
    }
    wxDELETE(m_pMenu);
}

void wxGISIdentifyDlg::OnSwitchSplit(wxCommandEvent& event)
{
	wxSplitMode eMode = m_splitter->GetSplitMode();
	int nSplitPos = m_splitter->GetSashPosition();
	m_splitter->Unsplit(m_pTreeCtrl);
	m_splitter->Unsplit(m_pFeatureDetailsPanel);
	if(eMode == wxSPLIT_HORIZONTAL)
		eMode = wxSPLIT_VERTICAL;
	else
		eMode = wxSPLIT_HORIZONTAL;

	switch(eMode)
	{
	case wxSPLIT_HORIZONTAL:
		m_bpSplitButton->SetBitmap(m_BmpHorz);
		m_splitter->SplitHorizontally(m_pTreeCtrl, m_pFeatureDetailsPanel, nSplitPos);
		break;
    default:
	case wxSPLIT_VERTICAL:
		m_bpSplitButton->SetBitmap(m_BmpVert);
		m_splitter->SplitVertically(m_pTreeCtrl, m_pFeatureDetailsPanel, nSplitPos);
		break;
	};
}

void wxGISIdentifyDlg::OnSelChanged(wxTreeEvent& event)
{
    wxTreeItemId TreeItemId = event.GetItem();
    if(TreeItemId.IsOk())
    {
        wxIdentifyTreeItemData* pData = (wxIdentifyTreeItemData*)m_pTreeCtrl->GetItemData(TreeItemId);
        if(pData == NULL)
		{
			m_pFeatureDetailsPanel->Clear();
			return;
		}
		if(pData->m_nOID == wxNOT_FOUND)
		{
			m_pFeatureDetailsPanel->Clear();
			return;
		}
		wxGISFeature Feature = pData->m_pDataset->GetFeature(pData->m_nOID);
		m_pFeatureDetailsPanel->FillPanel(Feature);
    }
}

void wxGISIdentifyDlg::OnLeftDown(wxMouseEvent& event)
{
}

void wxGISIdentifyDlg::OnMenu(wxCommandEvent& event)
{
}

void wxGISIdentifyDlg::OnItemRightClick(wxTreeEvent& event)
{
 	wxTreeItemId item = event.GetItem();
	if(!item.IsOk())
		return;
    m_pTreeCtrl->SelectItem(item);
    PopupMenu(m_pMenu, event.GetPoint());
}


void wxGISIdentifyDlg::OnDoubleClickSash(wxSplitterEvent& event)
{
	event.Veto();
}

//-------------------------------------------------------------------
// wxAxToolboxView
//-------------------------------------------------------------------
IMPLEMENT_DYNAMIC_CLASS(wxAxIdentifyView, wxGISIdentifyDlg)

wxAxIdentifyView::wxAxIdentifyView(void)
{
}

wxAxIdentifyView::wxAxIdentifyView(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size) : wxGISIdentifyDlg(parent, id, pos, size, wxNO_BORDER | wxTAB_TRAVERSAL)
{
    Create(parent, id, pos, size);
}

wxAxIdentifyView::~wxAxIdentifyView(void)
{
}

bool wxAxIdentifyView::Create(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name)
{
	m_pMapView = NULL;
    m_sViewName = wxString(_("Identify"));
    return wxGISIdentifyDlg::Create(parent, id, pos, size, style, name);
}

bool wxAxIdentifyView::Activate(IApplication* const pApplication, wxXmlNode* const pConf)
{
    m_pApp = dynamic_cast<wxGISApplicationBase*>(pApplication);
    if(NULL == m_pApp)
        return false;

	////get split from config and apply it
	//m_pConf = pConf;
	//wxSplitMode eMode = (wxSplitMode)wxAtoi(m_pConf->GetAttribute(wxT("split_mode"), wxT("2")));//wxSPLIT_VERTICAL = 2
	//m_nSashPos = wxAtoi(m_pConf->GetAttribute(wxT("split_pos"), wxT("100")));

	////int w = wxAtoi(m_pConf->GetAttribute(wxT("width"), wxT("-1")));
	////int h = wxAtoi(m_pConf->GetAttribute(wxT("height"), wxT("-1")));
	////SetClientSize(w, h);

	//m_splitter->Unsplit(m_pTreeCtrl);
	//m_splitter->Unsplit(m_pFeatureDetailsPanel);
	//switch(eMode)
	//{
	//case wxSPLIT_HORIZONTAL:
	//	m_bpSplitButton->SetBitmap(m_BmpHorz);
	//	m_splitter->SplitHorizontally(m_pTreeCtrl, m_pFeatureDetailsPanel, 150);
	//	break;
	//case wxSPLIT_VERTICAL:
	//	m_bpSplitButton->SetBitmap(m_BmpVert);
	//	m_splitter->SplitVertically(m_pTreeCtrl, m_pFeatureDetailsPanel, 150);
	//	break;
	//};
	return true;
}

void wxAxIdentifyView::Deactivate(void)
{
	//set split to config
	//int w, h;
	//GetClientSize(&w, &h);
	//if(m_pConf->HasAttribute(wxT("width")))
	//	m_pConf->DeleteAttribute(wxT("width"));
	//m_pConf->AddAttribute(wxT("width"), wxString::Format(wxT("%d"), w));
	//if(m_pConf->HasAttribute(wxT("height")))
	//	m_pConf->DeleteAttribute(wxT("height"));
	//m_pConf->AddAttribute(wxT("height"), wxString::Format(wxT("%d"), h));

	//wxSplitMode eMode = m_splitter->GetSplitMode();
	//int nSplitPos = m_splitter->GetSashPosition();
	//if(m_pConf->HasAttribute(wxT("split_mode")))
	//	m_pConf->DeleteAttribute(wxT("split_mode"));
	//m_pConf->AddAttribute(wxT("split_mode"), wxString::Format(wxT("%d"), eMode));
	//if(m_pConf->HasAttribute(wxT("split_pos")))
	//	m_pConf->DeleteAttribute(wxT("split_pos"));
	//m_pConf->AddAttribute(wxT("split_pos"), wxString::Format(wxT("%d"), nSplitPos));
}

WXGISRGBA wxAxIdentifyView::GetDrawStyle(wxGISEnumDrawStyle eDrawStyle)
{

    wxGISAppConfig oConfig = GetConfig();
    WXGISRGBA ret = {1.0, 1.0, 1.0, 1.0};
    switch(eDrawStyle)
    {
    case enumGISDrawStyleOutline:
        ret.dRed = 0;
        ret.dGreen = 0.8235294117647059;
        ret.dBlue = 1.0;
        if(oConfig.IsOk())
        {
	        ret.dRed = double(oConfig.ReadInt(enumGISHKCU, wxString(wxT("wxGISCommon/identify/flash_line/red")), 0)) / 255;
	        ret.dGreen = double(oConfig.ReadInt(enumGISHKCU, wxString(wxT("wxGISCommon/identify/flash_line/green")), 210)) / 255;
	        ret.dBlue = double(oConfig.ReadInt(enumGISHKCU, wxString(wxT("wxGISCommon/identify/flash_line/blue")), 255)) / 255;
        }
        break;
    case enumGISDrawStyleFill:
        ret.dRed = 0;
        ret.dGreen = 1.0;
        ret.dBlue = 1.0;
        if(oConfig.IsOk())
        {
	        ret.dRed = double(oConfig.ReadInt(enumGISHKCU, wxString(wxT("wxGISCommon/identify/flash_fill/red")), 0)) / 255;
	        ret.dGreen = double(oConfig.ReadInt(enumGISHKCU, wxString(wxT("wxGISCommon/identify/flash_fill/green")), 255)) / 255;
	        ret.dBlue = double(oConfig.ReadInt(enumGISHKCU, wxString(wxT("wxGISCommon/identify/flash_fill/blue")), 255)) / 255;
        }
        break;
    case enumGISDrawStylePoint:
        break;
    };
    return ret;
}

void wxAxIdentifyView::Identify(wxGISGeometry &GeometryBounds)
{
	if(!m_pMapView)//TODO: add/remove layer map events connection point
	{
        wxWindow* pWnd = m_pApp->GetRegisteredWindowByType(wxCLASSINFO(wxGISMapView));
        m_pMapView = dynamic_cast<wxGISMapView*>(pWnd);
	}
	if(!m_pMapView)
        return;

	wxBusyCursor wait;
	wxGISSpatialReference SpaRef = m_pMapView->GetSpatialReference();
    double dfWidth(3), dfHeight(3);

    wxGISAppConfig oConfig = GetConfig();
    if(oConfig.IsOk())
    {
        dfWidth = oConfig.ReadDouble(enumGISHKCU, wxString(wxT("wxGISCommon/identify/search_width")), dfWidth);
        dfHeight = oConfig.ReadDouble(enumGISHKCU, wxString(wxT("wxGISCommon/identify/search_height")), dfHeight);
    }

    if(m_pMapView->GetDisplay())
    {
        m_pMapView->GetDisplay()->DC2WorldDist(&dfWidth, &dfHeight);
        dfWidth = std::fabs(dfWidth);
        dfHeight = std::fabs(dfHeight);
    }

    OGREnvelope Env = GeometryBounds.GetEnvelope();
    bool bChanged(false);
    //if we got a small envelope or it's a point
    if(Env.MaxX - Env.MinX < dfWidth)
    {
        Env.MinX -= dfWidth;
        Env.MaxX += dfWidth;
        bChanged = true;
    }

    if(Env.MaxY - Env.MinY < dfHeight)
    {
        Env.MinY -= dfHeight;
        Env.MaxY += dfHeight;
        bChanged = true;
    }

    if(bChanged)
    {
        GeometryBounds = EnvelopeToGeometry(Env, SpaRef);
    }

    OGRPoint *pt = GeometryBounds.GetCentroid();

 	//get top layer
	wxGISLayer* const pTopLayer = m_pMapView->GetLayer(m_pMapView->GetLayerCount() - 1);
	if(!pTopLayer)
		return;

	wxGISEnumDatasetType eType = pTopLayer->GetType();
	switch(eType)
	{
	case enumGISFeatureDataset:
		{
			wxGISFeatureLayer* pFLayer = wxDynamicCast(pTopLayer, wxGISFeatureLayer);
			if(!pFLayer)
				return;

			wxGISQuadTreeCursor Cursor = pFLayer->Idetify(GeometryBounds);
			//flash on map
            WXGISRGBA stFillColor = GetDrawStyle(enumGISDrawStyleFill);
            WXGISRGBA stLineColor = GetDrawStyle(enumGISDrawStyleOutline);
            for(size_t i = 0; i < Cursor.GetCount(); ++i)
            {
                wxGISQuadTreeItem* pItem = Cursor[i];
                if(!pItem)
                    continue;
                m_pMapView->AddFlashGeometry(pItem->GetGeometry(), stFillColor, stLineColor);
            }
            m_pMapView->StartFlashing();

            //fill IdentifyDlg
			m_pFeatureDetailsPanel->Clear(true);
			m_pFeatureDetailsPanel->FillPanel(pt);

			FillTree(pFLayer, Cursor);
		}
		break;
	default:
		break;
	};

    OGRGeometryFactory::destroyGeometry(pt);
}

void wxAxIdentifyView::FillTree(wxGISFeatureLayer* const pFLayer, const wxGISQuadTreeCursor &Cursor)
{
	m_pTreeCtrl->DeleteAllItems();
	m_pFeatureDetailsPanel->Clear();
	if(Cursor.GetCount() < 1)
		return;

	//add root
	wxTreeItemId nRootId = m_pTreeCtrl->AddRoot(wxT("Layers"), 0);
	m_pTreeCtrl->SetItemBold(nRootId);
	//add layers
	wxTreeItemId nLayerId = m_pTreeCtrl->AppendItem(nRootId, pFLayer->GetName(), 1);
	m_pTreeCtrl->SetItemData(nLayerId, new wxIdentifyTreeItemData(wxDynamicCast(pFLayer->GetDataset(), wxGISFeatureDataset)));
	wxTreeItemId nFirstFeatureId = nLayerId;
    for(size_t i = 0; i < Cursor.GetCount(); ++i)
    {
        wxGISQuadTreeItem* pItem = Cursor[i];
        if(!pItem)
            continue;
		wxTreeItemId nFeatureId = m_pTreeCtrl->AppendItem(nLayerId, wxString::Format(wxT("%d"), pItem->GetOID()), 2);
		m_pTreeCtrl->SetItemData(nFeatureId, new wxIdentifyTreeItemData(wxDynamicCast(pFLayer->GetDataset(), wxGISFeatureDataset), pItem->GetOID(), pItem->GetGeometry()));
		if(i == 0)
			nFirstFeatureId = nFeatureId;
    }
	m_pTreeCtrl->ExpandAllChildren(nLayerId);
	m_pTreeCtrl->SelectItem(nFirstFeatureId);
}

void wxAxIdentifyView::OnSelChanged(wxTreeEvent& event)
{
    event.Skip();
    wxTreeItemId TreeItemId = event.GetItem();
    if(TreeItemId.IsOk())
    {
        wxIdentifyTreeItemData* pData = (wxIdentifyTreeItemData*)m_pTreeCtrl->GetItemData(TreeItemId);
        if(pData == NULL)
		{
			m_pFeatureDetailsPanel->Clear();
			return;
		}
		if(pData->m_nOID == wxNOT_FOUND)
		{
			m_pFeatureDetailsPanel->Clear();
			return;
		}

        m_pMapView->AddFlashGeometry(pData->m_Geometry, GetDrawStyle(enumGISDrawStyleFill), GetDrawStyle(enumGISDrawStyleOutline));
        m_pMapView->StartFlashing();
        wxGISFeature Feature = pData->m_pDataset->GetFeatureByID(pData->m_nOID);
		m_pFeatureDetailsPanel->FillPanel(Feature);
    }
}

void wxAxIdentifyView::OnLeftDown(wxMouseEvent& event)
{
	event.Skip();
	wxPoint pt = event.GetPosition();
	unsigned long nFlags(0);
	wxTreeItemId TreeItemId = m_pTreeCtrl->HitTest(pt, (int &)nFlags);
	if(TreeItemId.IsOk() && ((nFlags & wxTREE_HITTEST_ONITEMLABEL) || (nFlags & wxTREE_HITTEST_ONITEMICON)))
	{
        wxIdentifyTreeItemData* pData = (wxIdentifyTreeItemData*)m_pTreeCtrl->GetItemData(TreeItemId);
        if(pData && pData->m_Geometry.IsOk())
        {
            m_pMapView->AddFlashGeometry(pData->m_Geometry, GetDrawStyle(enumGISDrawStyleFill), GetDrawStyle(enumGISDrawStyleOutline));
            m_pMapView->StartFlashing();
        }
    }
}

void wxAxIdentifyView::OnMenu(wxCommandEvent& event)
{
	wxTreeItemId TreeItemId = m_pTreeCtrl->GetSelection();
    wxIdentifyTreeItemData* pData = (wxIdentifyTreeItemData*)m_pTreeCtrl->GetItemData(TreeItemId);
    if(pData == NULL)
		return;

	switch(event.GetId())
	{
	case ID_WGMENU_FLASH:
	{
		if(pData->m_Geometry.IsOk())
		    m_pMapView->AddFlashGeometry(pData->m_Geometry, GetDrawStyle(enumGISDrawStyleFill), GetDrawStyle(enumGISDrawStyleOutline));
		else
		{
            WXGISRGBA stFillColor = GetDrawStyle(enumGISDrawStyleFill);
            WXGISRGBA stLineColor = GetDrawStyle(enumGISDrawStyleOutline);
			wxTreeItemIdValue cookie;
			for ( wxTreeItemId item = m_pTreeCtrl->GetFirstChild(TreeItemId, cookie); item.IsOk(); item = m_pTreeCtrl->GetNextChild(TreeItemId, cookie) )
			{
				pData = (wxIdentifyTreeItemData*)m_pTreeCtrl->GetItemData(item);
				if(pData)
                    m_pMapView->AddFlashGeometry(pData->m_Geometry, stFillColor, stLineColor);
			}
		}
        m_pMapView->StartFlashing();
	}
	break;
	case ID_WGMENU_PAN:
	{
        wxGISGeometryArray Arr;
		if(pData->m_Geometry.IsOk())
		    m_pMapView->AddFlashGeometry(pData->m_Geometry, GetDrawStyle(enumGISDrawStyleFill), GetDrawStyle(enumGISDrawStyleOutline));
		else
		{
            WXGISRGBA stFillColor = GetDrawStyle(enumGISDrawStyleFill);
            WXGISRGBA stLineColor = GetDrawStyle(enumGISDrawStyleOutline);
			wxTreeItemIdValue cookie;
			for ( wxTreeItemId item = m_pTreeCtrl->GetFirstChild(TreeItemId, cookie); item.IsOk(); item = m_pTreeCtrl->GetNextChild(TreeItemId, cookie) )
			{
				pData = (wxIdentifyTreeItemData*)m_pTreeCtrl->GetItemData(item);
				if(pData)
                    m_pMapView->AddFlashGeometry(pData->m_Geometry, stFillColor, stLineColor);
			}
		}
		OGREnvelope Env;
		for(size_t i = 0; i < Arr.GetCount(); ++i)
		{
            OGREnvelope TempEnv = Arr[i].GetEnvelope();
			Env.Merge(TempEnv);
		}
		OGREnvelope CurrentEnv = m_pMapView->GetCurrentExtent();
		MoveEnvelope(CurrentEnv, Env);
		m_pMapView->Do(CurrentEnv);
        m_pMapView->StartFlashing();
	}
	break;
	case ID_WGMENU_ZOOM:
	{
        wxGISGeometryArray Arr;
		if(pData->m_Geometry.IsOk())
		    m_pMapView->AddFlashGeometry(pData->m_Geometry, GetDrawStyle(enumGISDrawStyleFill), GetDrawStyle(enumGISDrawStyleOutline));
		else
		{
            WXGISRGBA stFillColor = GetDrawStyle(enumGISDrawStyleFill);
            WXGISRGBA stLineColor = GetDrawStyle(enumGISDrawStyleOutline);
			wxTreeItemIdValue cookie;
			for ( wxTreeItemId item = m_pTreeCtrl->GetFirstChild(TreeItemId, cookie); item.IsOk(); item = m_pTreeCtrl->GetNextChild(TreeItemId, cookie) )
			{
				pData = (wxIdentifyTreeItemData*)m_pTreeCtrl->GetItemData(item);
				if(pData)
                    m_pMapView->AddFlashGeometry(pData->m_Geometry, stFillColor, stLineColor);
			}
		}
		OGREnvelope Env;
		for(size_t i = 0; i < Arr.GetCount(); ++i)
		{
			OGREnvelope TempEnv = Arr[i].GetEnvelope();
			Env.Merge(TempEnv);
		}
		m_pMapView->Do(Env);
        m_pMapView->StartFlashing();
	}
	break;
	default:
	break;
	}
}
