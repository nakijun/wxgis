/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  Carto Main Commands class.
 * Author:   Bishop (aka Barishnikov Dmitriy), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2009,2011 Bishop
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
#include "wxgis/cartoui/cartocmd.h"
#include "wxgis/display/rubberband.h"
#include "wxgis/core/config.h"
//#include "wxgis/display/simplefillsymbol.h"

#include "../../art/fullext.xpm"
#include "../../art/prevext.xpm"
#include "../../art/nextext.xpm"
#include "../../art/zoomin.xpm"
#include "../../art/zoomout.xpm"
#include "../../art/info.xpm"
#include "../../art/pan1_cur.xpm"
#include "../../art/pan2_cur.xpm"
#include "../../art/cancel_map_rotate.xpm"
#include "../../art/get_info_cur.xpm"
#include "../../art/map_rotate.xpm"
#include "../../art/rotate_cur.xpm"
#include "../../art/zoom_in_cur.xpm"
#include "../../art/zoom_out_cur.xpm"

//#include "../../art/geography16.xpm"
//#include "../../art/cursors_16.xpm"

//	0	Full Extent
//	1	Prev Extent
//	2	Next Extent
//	3	?

//--------------------------------------------------
// wxGISCartoMainCmd
//--------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxGISCartoMainCmd, wxObject)


wxGISCartoMainCmd::wxGISCartoMainCmd(void) : m_pMapView(NULL)
{
}

wxGISCartoMainCmd::~wxGISCartoMainCmd(void)
{
}

wxIcon wxGISCartoMainCmd::GetBitmap(void)
{
	switch(m_subtype)
	{
		case 0:
			if(!m_IconFullExt.IsOk())
				m_IconFullExt = wxIcon(fullext_xpm);
			return m_IconFullExt;
		case 1:
			if(!m_IconPrevExt.IsOk())
				m_IconPrevExt = wxIcon(prevext_xpm);
			return m_IconPrevExt;
		case 2:
			if(!m_IconNextExt.IsOk())
				m_IconNextExt = wxIcon(nextext_xpm);
			return m_IconNextExt;
		default:
			return wxNullIcon;
	}
}

wxString wxGISCartoMainCmd::GetCaption(void)
{
	switch(m_subtype)
	{
		case 0:
			return wxString(_("&Full Extent"));
		case 1:
			return wxString(_("&Previous Extent"));
		case 2:
			return wxString(_("&Next Extent"));
		default:
			return wxEmptyString;
	}
}

wxString wxGISCartoMainCmd::GetCategory(void)
{
	switch(m_subtype)
	{
		case 0:
		case 1:
		case 2:
			return wxString(_("Geography"));
		default:
			return wxString(_("[No category]"));
	}
}

bool wxGISCartoMainCmd::GetChecked(void)
{
	switch(m_subtype)
	{
		case 0:
		case 1:
		case 2:
			return false;
		default:
			return false;
	}
}

bool wxGISCartoMainCmd::GetEnabled(void)
{
	if(!m_pMapView)
	{
		const WINDOWARRAY* pWinArr = m_pApp->GetChildWindows();
		if(pWinArr)
		{
			for(size_t i = 0; i < pWinArr->size(); ++i)
			{
				wxGISMapView* pMapView = dynamic_cast<wxGISMapView*>(pWinArr->at(i));
				if(pMapView)
				{
					m_pMapView = pMapView;
					break;
				}
			}
		}
	}
	if(!m_pMapView)
        return false;

	switch(m_subtype)
	{
		case 0:
			return m_pMapView->IsShown();
		case 1:
			return m_pMapView->IsShown() && m_pMapView->CanUndo();
		case 2:
			return m_pMapView->IsShown() && m_pMapView->CanRedo();
		default:
			return false;
	}
}

wxGISEnumCommandKind wxGISCartoMainCmd::GetKind(void)
{
	switch(m_subtype)
	{
		case 0://Full Extent
		case 1://Previous Extent
		case 2://Next Extent
			return enumGISCommandNormal;
		default:
			return enumGISCommandNormal;
	}
}

wxString wxGISCartoMainCmd::GetMessage(void)
{
	switch(m_subtype)
	{
		case 0:
			return wxString(_("Full extent of the map"));
		case 1:
			return wxString(_("Previous extent of the map"));
		case 2:
			return wxString(_("Next extent of the map"));
		default:
			return wxEmptyString;
	}
}

void wxGISCartoMainCmd::OnClick(void)
{
	switch(m_subtype)
	{
		case 0:
			return m_pMapView->SetFullExtent();
		case 1:
			return m_pMapView->Undo();
		case 2:
			return m_pMapView->Redo();
		default:
			break;
	}
}

bool wxGISCartoMainCmd::OnCreate(IFrameApplication* pApp)
{
	m_pApp = pApp;
	return true;
}

wxString wxGISCartoMainCmd::GetTooltip(void)
{
	switch(m_subtype)
	{
		case 0:
			return wxString(_("Full Extent"));
		case 1:
			return wxString(_("Previous Extent"));
		case 2:
			return wxString(_("Next Extent"));
		default:
			return wxEmptyString;
	}
}

unsigned char wxGISCartoMainCmd::GetCount(void)
{
	return 3;
}

//--------------------------------------------------
// wxGISCartoMainTool
//--------------------------------------------------

//	0	//Zoom In
//	1	//Zoom Out
//	2	//Pan
//	3	//Info
//	4	//?

IMPLEMENT_DYNAMIC_CLASS(wxGISCartoMainTool, wxObject)


wxGISCartoMainTool::wxGISCartoMainTool(void) : m_pMapView(NULL), m_bCheck(false)
{
}

wxGISCartoMainTool::~wxGISCartoMainTool(void)
{
}

wxIcon wxGISCartoMainTool::GetBitmap(void)
{
	switch(m_subtype)
	{
		case 0:
			if(!m_IconZoomIn.IsOk())
				m_IconZoomIn = wxIcon(zoomin_xpm);
			return m_IconZoomIn;
		case 1:
			if(!m_IconZoomOut.IsOk())
				m_IconZoomOut = wxIcon(zoomout_xpm);
			return m_IconZoomOut;
		case 2:
			if(!m_IconPan.IsOk())
				m_IconPan = wxIcon(pan1_cur_xpm);
			return m_IconPan;
		case 3:
			if(!m_IconInfo.IsOk())
				m_IconInfo = wxIcon(info_xpm);
			return m_IconInfo;
		default:
			return wxNullIcon;
	}
}

wxString wxGISCartoMainTool::GetCaption(void)
{
	switch(m_subtype)
	{
		case 0:
			return wxString(_("Zoom &In"));
		case 1:
			return wxString(_("Zoom &Out"));
		case 2:
			return wxString(_("&Pan"));
		case 3:
			return wxString(_("&Information"));
		default:
			return wxEmptyString;
	}
}

wxString wxGISCartoMainTool::GetCategory(void)
{
	switch(m_subtype)
	{
		case 0:
		case 1:
		case 2:
		case 3:
			return wxString(_("Geography"));
		default:
			return wxString(_("[No category]"));
	}
}

bool wxGISCartoMainTool::GetChecked(void)
{
	return m_bCheck;
}

bool wxGISCartoMainTool::GetEnabled(void)
{
	if(!m_pMapView)
	{
		const WINDOWARRAY* pWinArr = m_pApp->GetChildWindows();
		if(pWinArr)
		{
			for(size_t i = 0; i < pWinArr->size(); ++i)
			{
				wxGISMapView* pMapView = dynamic_cast<wxGISMapView*>(pWinArr->at(i));
				if(pMapView)
				{
					m_pMapView = pMapView;
					break;
				}
			}
		}
	}
	if(!m_pMapView)
        return false;

	switch(m_subtype)
	{
		case 0:
		case 1:
		case 2:
		case 3:
			return m_pMapView->IsShown();
		default:
			return false;
	}
}

wxGISEnumCommandKind wxGISCartoMainTool::GetKind(void)
{
	switch(m_subtype)
	{
		case 0://ZoomIn
		case 1://ZoomOut
		case 2://Pan
		case 3://Information
			return enumGISCommandCheck;
		default:
			return enumGISCommandNormal;
	}
}

wxString wxGISCartoMainTool::GetMessage(void)
{
	switch(m_subtype)
	{
		case 0:
			return wxString(_("Zoom map in"));
		case 1:
			return wxString(_("Zoom map out"));
		case 2:
			return wxString(_("Pan map"));
		case 3:
			return wxString(_("Feature information"));
		default:
			return wxEmptyString;
	}
}

void wxGISCartoMainTool::OnClick(void)
{
	switch(m_subtype)
	{
		case 0:
		case 1:
		case 2:
		case 3:
			break;
		default:
			break;
	}
}

bool wxGISCartoMainTool::OnCreate(IFrameApplication* pApp)
{
	m_pApp = pApp;
	return true;
}

wxString wxGISCartoMainTool::GetTooltip(void)
{
	switch(m_subtype)
	{
		case 0:
			return wxString(_("Zoom In"));
		case 1:
			return wxString(_("Zoom Out"));
		case 2:
			return wxString(_("Pan"));
		case 3:
			return wxString(_("Information"));
		default:
			return wxEmptyString;
	}
}

unsigned char wxGISCartoMainTool::GetCount(void)
{
	return 4;
}

wxCursor wxGISCartoMainTool::GetCursor(void)
{
	switch(m_subtype)
	{
		case 0:	//z_in
			if(!m_CurZoomIn.IsOk())
			{
				wxImage CursorImage = wxBitmap(zoom_in_cur_xpm).ConvertToImage();
				CursorImage.SetOption(wxIMAGE_OPTION_CUR_HOTSPOT_X, 6);
				CursorImage.SetOption(wxIMAGE_OPTION_CUR_HOTSPOT_Y, 6);
				m_CurZoomIn = wxCursor(CursorImage);
			}
			return m_CurZoomIn;
		case 1:	//z_out
			if(!m_CurZoomIn.IsOk())
			{
				wxImage CursorImage = wxBitmap(zoom_out_cur_xpm).ConvertToImage();
				CursorImage.SetOption(wxIMAGE_OPTION_CUR_HOTSPOT_X, 6);
				CursorImage.SetOption(wxIMAGE_OPTION_CUR_HOTSPOT_Y, 6);
				m_CurZoomOut = wxCursor(CursorImage);
			}
			return m_CurZoomOut;
		case 2:	//pan
			if(!m_CurPan1.IsOk())
			{
				wxImage CursorImage = wxBitmap(pan1_cur_xpm).ConvertToImage();
				CursorImage.SetOption(wxIMAGE_OPTION_CUR_HOTSPOT_X, 7);
				CursorImage.SetOption(wxIMAGE_OPTION_CUR_HOTSPOT_Y, 7);
				m_CurPan1 = wxCursor(CursorImage);
			}
			return m_CurPan1;
		case 3:	//inf
			if(!m_CurInfo.IsOk())
			{
				wxImage CursorImage = wxBitmap(get_info_cur_xpm).ConvertToImage();
				CursorImage.SetOption(wxIMAGE_OPTION_CUR_HOTSPOT_X, 0);
				CursorImage.SetOption(wxIMAGE_OPTION_CUR_HOTSPOT_Y, 0);
				m_CurInfo = wxCursor(CursorImage);
			}
			return m_CurInfo;
		default:
			return wxNullCursor;
	}
}

void wxGISCartoMainTool::SetChecked(bool bCheck)
{
	m_bCheck = bCheck;
	if(m_bCheck && m_pMapView)
		m_pMapView->SetCursor(GetCursor());
}

void wxGISCartoMainTool::OnMouseDown(wxMouseEvent& event)
{
    event.Skip();
	switch(m_subtype)
	{
		case 0:	//z_in
		{
			wxGISAppConfigSPtr pConfig = GetConfig();
			int nR = pConfig->ReadInt(enumGISHKCU, m_pApp->GetAppName() + wxString(wxT("/rabberband/rcolor")), 0);
			int nG = pConfig->ReadInt(enumGISHKCU, m_pApp->GetAppName() + wxString(wxT("/rabberband/gcolor")), 0);
			int nB = pConfig->ReadInt(enumGISHKCU, m_pApp->GetAppName() + wxString(wxT("/rabberband/bcolor")), 255);
			int nWidth = pConfig->ReadInt(enumGISHKCU, m_pApp->GetAppName() + wxString(wxT("/rabberband/width")), 2);
			wxGISRubberEnvelope RubberEnvelope(wxPen(wxColour(nR, nG, nB), nWidth), m_pMapView, m_pMapView->GetDisplay());
			OGREnvelope Env = RubberEnvelope.TrackNew( event.GetX(), event.GetY() );
			if(IsDoubleEquil(Env.MaxX, Env.MinX) || IsDoubleEquil(Env.MaxY, Env.MinY))
			{
				OGREnvelope CurrentEnv = m_pMapView->GetCurrentExtent();
				double widthdiv4 = (CurrentEnv.MaxX - CurrentEnv.MinX) / 4;
				double heightdiv4 = (CurrentEnv.MaxY - CurrentEnv.MinY) / 4;

				Env.MinX -= widthdiv4;
				Env.MinY -= heightdiv4;
				Env.MaxX += widthdiv4;
				Env.MaxY += heightdiv4;
			}

			if(m_pMapView->GetScaleRatio(Env, wxClientDC(m_pMapView)) > 1.0)
                m_pMapView->Do(Env);
		}
		break;
		case 1:	//z_out
		{
			wxGISAppConfigSPtr pConfig = GetConfig();
			int nR = pConfig->ReadInt(enumGISHKCU, m_pApp->GetAppName() + wxString(wxT("/rabberband/rcolor")), 0);
			int nG = pConfig->ReadInt(enumGISHKCU, m_pApp->GetAppName() + wxString(wxT("/rabberband/gcolor")), 0);
			int nB = pConfig->ReadInt(enumGISHKCU, m_pApp->GetAppName() + wxString(wxT("/rabberband/bcolor")), 255);
			int nWidth = pConfig->ReadInt(enumGISHKCU, m_pApp->GetAppName() + wxString(wxT("/rabberband/width")), 2);
			wxGISRubberEnvelope RubberEnvelope(wxPen(wxColour(nR, nG, nB), nWidth), m_pMapView, m_pMapView->GetDisplay());
			OGREnvelope Env = RubberEnvelope.TrackNew( event.GetX(), event.GetY() );
			OGREnvelope CurrentEnv = m_pMapView->GetCurrentExtent();
			OGREnvelope NewEnv;
			NewEnv.MinX = CurrentEnv.MinX + CurrentEnv.MinX - Env.MinX;
			NewEnv.MinY = CurrentEnv.MinY + CurrentEnv.MinY - Env.MinY;
			NewEnv.MaxX = CurrentEnv.MaxX + CurrentEnv.MaxX - Env.MaxX;
			NewEnv.MaxY = CurrentEnv.MaxY + CurrentEnv.MaxY - Env.MaxY;
            m_pMapView->Do(NewEnv);
		}
		break;
		case 2:	//pan
			if(!m_CurPan2.IsOk())
			{
				wxImage CursorImage = wxBitmap(pan2_cur_xpm).ConvertToImage();
				CursorImage.SetOption(wxIMAGE_OPTION_CUR_HOTSPOT_X, 7);
				CursorImage.SetOption(wxIMAGE_OPTION_CUR_HOTSPOT_Y, 7);
				m_CurPan2 = wxCursor(CursorImage);
			}
			m_pMapView->SetCursor(m_CurPan2);
			m_pMapView->PanStart(event.GetPosition());
			break;
		case 3:	//inf
		{
			wxGISAppConfigSPtr pConfig = GetConfig();
			int nR = pConfig->ReadInt(enumGISHKCU, m_pApp->GetAppName() + wxString(wxT("/rabberband/rcolor")), 0);
			int nG = pConfig->ReadInt(enumGISHKCU, m_pApp->GetAppName() + wxString(wxT("/rabberband/gcolor")), 0);
			int nB = pConfig->ReadInt(enumGISHKCU, m_pApp->GetAppName() + wxString(wxT("/rabberband/bcolor")), 255);
			int nWidth = pConfig->ReadInt(enumGISHKCU, m_pApp->GetAppName() + wxString(wxT("/rabberband/width")), 2);
			wxGISRubberEnvelope RubberEnvelope(wxPen(wxColour(nR, nG, nB), nWidth), m_pMapView, m_pMapView->GetDisplay());
			OGREnvelope Env = RubberEnvelope.TrackNew( event.GetX(), event.GetY() );
            if(IsDoubleEquil(Env.MaxX, Env.MinX) || IsDoubleEquil(Env.MaxY, Env.MinY))
			{
				Env.MinX -= widthdiv4;
				Env.MinY -= heightdiv4;
				Env.MaxX += widthdiv4;
				Env.MaxY += heightdiv4;
			}

			m_pMapView->Identify(Env);
			//m_pMapView->Refresh(false);
		}
		break;
		default:
			break;
	}
}

void wxGISCartoMainTool::OnMouseUp(wxMouseEvent& event)
{
//    event.Skip();
	switch(m_subtype)
	{
		case 0:	//z_in
			break;
		case 1:	//z_out
			break;
		case 2:	//pan
			m_pMapView->PanStop(event.GetPosition());
			m_pMapView->SetCursor(GetCursor());
			break;
		case 3:	//inf
			break;
		default:
			break;
	}
}

void wxGISCartoMainTool::OnMouseMove(wxMouseEvent& event)
{
    //event.Skip();
	switch(m_subtype)
	{
		case 0:	//z_in
			break;
		case 1:	//z_out
			break;
		case 2:	//pan
			if(event.Dragging())
				m_pMapView->PanMoveTo(event.GetPosition());
			break;
		case 3:	//inf
			break;
		default:
			break;
	}
}

void wxGISCartoMainTool::OnMouseDoubleClick(wxMouseEvent& event)
{
    event.Skip();
}

//--------------------------------------------------
// wxGISCartoFrameTool
//--------------------------------------------------

//	0	//Rotate
//	1	//Cancel Rotate
//	2	//Input Rotate Angle
//	3	//?

IMPLEMENT_DYNAMIC_CLASS(wxGISCartoFrameTool, wxObject)


wxGISCartoFrameTool::wxGISCartoFrameTool(void) : m_pMapView(NULL), m_bCheck(false)
{
}

wxGISCartoFrameTool::~wxGISCartoFrameTool(void)
{
}

wxIcon wxGISCartoFrameTool::GetBitmap(void)
{
	switch(m_subtype)
	{
		case 0:
			if(!m_IconRotate.IsOk())
				m_IconRotate = wxIcon(map_rotate_xpm);
			return m_IconRotate;
		case 1:
			if(!m_IconCancelRotate.IsOk())
				m_IconCancelRotate = wxIcon(cancel_map_rotate_xpm);
			return m_IconCancelRotate;
		case 2:
		default:
			return wxNullIcon;
	}
}

wxString wxGISCartoFrameTool::GetCaption(void)
{
	switch(m_subtype)
	{
		case 0:
			return wxString(_("&Rotate"));
		case 1:
			return wxString(_("Cancel rotate"));
		case 2:
			return wxString(_("Input rotate angle"));
		default:
			return wxEmptyString;
	}
}

wxString wxGISCartoFrameTool::GetCategory(void)
{
	switch(m_subtype)
	{
		case 0:
		case 1:
		case 2:
			return wxString(_("View"));
		default:
			return wxString(_("[No category]"));
	}
}

bool wxGISCartoFrameTool::GetChecked(void)
{
	return m_bCheck;
}

bool wxGISCartoFrameTool::GetEnabled(void)
{
	if(!m_pMapView)
	{
		const WINDOWARRAY* pWinArr = m_pApp->GetChildWindows();
		if(pWinArr)
		{
			for(size_t i = 0; i < pWinArr->size(); ++i)
			{
				wxGISMapView* pMapView = dynamic_cast<wxGISMapView*>(pWinArr->at(i));
				if(pMapView)
				{
					m_pMapView = pMapView;
					break;
				}
			}
		}
	}
	if(!m_pMapView)
        return false;

	switch(m_subtype)
	{
		case 0:
		case 1:
			//check if angle == 0
		case 2:
			if(m_pMapView)
				return m_pMapView->IsShown();
			return false;
		default:
			return false;
	}
}

wxGISEnumCommandKind wxGISCartoFrameTool::GetKind(void)
{
	switch(m_subtype)
	{
		case 0://Rotate
			return enumGISCommandCheck;
		case 1://Cancel rotate
			return enumGISCommandNormal;
    	case 2://Input rotate
			return enumGISCommandControl;
		default:
			return enumGISCommandNormal;
	}
}

wxString wxGISCartoFrameTool::GetMessage(void)
{
	switch(m_subtype)
	{
		case 0:
			return wxString(_("Rotate map frame"));
		case 1:
			return wxString(_("Cancel rotate map frame"));
		case 2:
			return wxString(_("Input map frame rotate angle"));
		default:
			return wxEmptyString;
	}
}

void wxGISCartoFrameTool::OnClick(void)
{
	switch(m_subtype)
	{
		case 0:
			break;
		case 1:
			if(m_pMapView)
			{
				m_pMapView->SetRotate(0.0);
			}
			break;
		case 2:
			break;
		default:
			break;
	}
}

bool wxGISCartoFrameTool::OnCreate(IFrameApplication* pApp)
{
	m_pApp = pApp;

	return true;
}

wxString wxGISCartoFrameTool::GetTooltip(void)
{
	switch(m_subtype)
	{
		case 0:
			return wxString(_("Rotate map frame"));
		case 1:
			return wxString(_("Cancel rotate map frame"));
		case 2:
			return wxString(_("Input map frame rotate angle"));
		default:
			return wxEmptyString;
	}
}

unsigned char wxGISCartoFrameTool::GetCount(void)
{
	return 3;
}

wxCursor wxGISCartoFrameTool::GetCursor(void)
{
	switch(m_subtype)
	{
		case 0:	//rotate
			if(!m_CurRotate.IsOk())
			{
				wxImage CursorImage = wxBitmap(rotate_cur_xpm).ConvertToImage();
				CursorImage.SetOption(wxIMAGE_OPTION_CUR_HOTSPOT_X, 7);
				CursorImage.SetOption(wxIMAGE_OPTION_CUR_HOTSPOT_Y, 7);
				m_CurRotate = wxCursor(CursorImage);
			}
			return m_CurRotate;
		default:
			return wxNullCursor;
	}
}

void wxGISCartoFrameTool::SetChecked(bool bCheck)
{
	m_bCheck = bCheck;
	if(m_bCheck)
		m_pMapView->SetCursor(GetCursor());
}

void wxGISCartoFrameTool::OnMouseDown(wxMouseEvent& event)
{
    event.Skip();
	switch(m_subtype)
	{
		case 0:	//rotate
			m_pMapView->RotateStart(event.GetPosition());
			break;
		default:
			break;
	}
}

void wxGISCartoFrameTool::OnMouseUp(wxMouseEvent& event)
{
//    event.Skip();
	switch(m_subtype)
	{
		case 0:	//rotate
			m_pMapView->RotateStop(event.GetPosition());
			m_pMapView->SetCursor(GetCursor());
			break;
		default:
			break;
	}
}

void wxGISCartoFrameTool::OnMouseMove(wxMouseEvent& event)
{
    //event.Skip();
	switch(m_subtype)
	{
		case 0:	//rotate
			if(event.Dragging())
			{
				m_pMapView->RotateBy(event.GetPosition());
			}
			break;
		case 2:
			break;
		default:
			break;
	}
}

void wxGISCartoFrameTool::OnMouseDoubleClick(wxMouseEvent& event)
{
    event.Skip();
}

IToolBarControl* wxGISCartoFrameTool::GetControl(void)
{
	switch(m_subtype)
	{
		case 2:	
			//if(!m_pRotationComboBox)
			{
				wxArrayString ValuesArray;
				wxGISRotationComboBox* pRotationComboBox = new wxGISRotationComboBox(dynamic_cast<wxWindow*>(m_pApp), wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 65, 22 ), ValuesArray);
				return static_cast<IToolBarControl*>(pRotationComboBox);
			}
		default:
			return NULL;
	}
}

wxString wxGISCartoFrameTool::GetToolLabel(void)
{
	switch(m_subtype)
	{
		case 2:	
			return wxEmptyString;
		default:
			return wxEmptyString;
	}
}

bool wxGISCartoFrameTool::HasToolLabel(void)
{
	switch(m_subtype)
	{
		case 2:	
			return false;
		default:
			return false;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////
// wxGISRotationComboBox
////////////////////////////////////////////////////////////////////////////////////////////

BEGIN_EVENT_TABLE(wxGISRotationComboBox, wxComboBox)
	EVT_TEXT_ENTER(wxID_ANY, wxGISRotationComboBox::OnTextEnter)
	EVT_COMBOBOX(wxID_ANY, wxGISRotationComboBox::OnTextEnter)
	EVT_MXMAP_ROTATED(wxGISRotationComboBox::OnMapRotated)
END_EVENT_TABLE()

wxGISRotationComboBox::wxGISRotationComboBox(wxWindow* parent, wxWindowID id, const wxString& value, const wxPoint& pos, const wxSize& size, const wxArrayString& choices, long style, const wxValidator& validator, const wxString& name) : wxComboBox(parent, id, value, pos, size, choices, style, validator, name), m_nConnectionPointMapCookie(wxNOT_FOUND)
{
	m_pMapView = NULL;
	AppendString(wxT("0.00"));
	AppendString(wxT("45.00"));
	AppendString(wxT("90.00"));
	AppendString(wxT("135.00"));
	AppendString(wxT("180.00"));
	AppendString(wxT("225.00"));
	AppendString(wxT("270.00"));
	AppendString(wxT("315.00"));

	SetValue(wxT("0.00"));
}

wxGISRotationComboBox::~wxGISRotationComboBox(void)
{
}

void wxGISRotationComboBox::OnTextEnter(wxCommandEvent& event)
{
	if(m_pMapView)
	{
		wxString sVal = GetValue();
		double dAngleGrad = wxAtof(sVal);
		wxString sFloatVal = wxString::Format(wxT("%.2f"), dAngleGrad);
		double dAngleRad = dAngleGrad * PIDEG;
		m_pMapView->SetRotate(-dAngleRad);
		//check uniq
		if(FindString(sFloatVal) == wxNOT_FOUND)
			AppendString(sFloatVal);
		SetStringSelection(sFloatVal);
	}
}

void wxGISRotationComboBox::OnMapRotated(wxMxMapViewEvent& event)
{
	UpdateAngle();
}

void wxGISRotationComboBox::Activate(IFrameApplication* pApp)
{
	const WINDOWARRAY* pWinArr = pApp->GetChildWindows();
	if(pWinArr)
	{
		for(size_t i = 0; i < pWinArr->size(); ++i)
		{
			wxGISMapView* pMapView = dynamic_cast<wxGISMapView*>(pWinArr->at(i));
			if(pMapView)
			{
				m_pMapView = pMapView;
				break;
			}
		}
	}

	m_pConnectionPointMap = dynamic_cast<wxGISConnectionPointContainer*>( m_pMapView );
	if(m_pConnectionPointMap != NULL)
		m_nConnectionPointMapCookie = m_pConnectionPointMap->Advise(this);
}

void wxGISRotationComboBox::Deactivate(void)
{
	if(m_nConnectionPointMapCookie != wxNOT_FOUND)
		m_pConnectionPointMap->Unadvise(m_nConnectionPointMapCookie);
}

void wxGISRotationComboBox::UpdateAngle(void)
{
	if(m_pMapView)
	{
		double dRotate = m_pMapView->GetCurrentRotate();
		wxString sVal = wxString::Format(wxT("%.2f"), dRotate * DEGPI);
		SetValue(sVal);
	}
}

