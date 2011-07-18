/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  List View with check box
 * Author:   Bishop (aka Barishnikov Dmitriy), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2010  Bishop
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

#include "wxgis/framework/checklist.h"
#include "../../art/check_marks.xpm"

BEGIN_EVENT_TABLE(wxGISCheckList, wxListView)
	EVT_LEFT_DOWN( wxGISCheckList::OnLeftDown )
END_EVENT_TABLE()


wxGISCheckList::wxGISCheckList(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxValidator& validator, const wxString& name) : wxListView(parent, id, pos, size, style, validator, name)
{
    m_CheckImageList.Create(16, 16);
	m_CheckImageList.Add(wxBitmap(check_marks_xpm));
    SetImageList(&m_CheckImageList, wxIMAGE_LIST_SMALL);
}

wxGISCheckList::~wxGISCheckList(void)
{
    for(size_t i = 0; i < GetItemCount(); ++i)
    {
        LPITEM_DATA pdata = (LPITEM_DATA)wxListView::GetItemData(i);
        wxDELETE(pdata);
    }
}

long wxGISCheckList::InsertItem(const wxString& label, int nChecked)
{
    long nItem = wxListView::InsertItem(-1, label, nChecked);//some state data
    LPITEM_DATA pdata = new ITEM_DATA;
    pdata->nCheckState = nChecked;
    pdata->bChanged = false;
    pdata->pUserData = 0;
    wxListView::SetItemData(nItem, (long)pdata);
    return nItem;
}

bool wxGISCheckList::SetItemData(long item, long data)
{
    LPITEM_DATA pdata = (LPITEM_DATA)wxListView::GetItemData(item);
    pdata->pUserData = data;
    return true;
}

long wxGISCheckList::GetItemData(long item) const
{
    LPITEM_DATA pdata = (LPITEM_DATA)wxListView::GetItemData(item);
    return pdata->pUserData;
}

void wxGISCheckList::OnLeftDown(wxMouseEvent& event)
{
	event.Skip();

	wxPoint pt = event.GetPosition();
	unsigned long nFlags(0);
	long nItemId = HitTest(pt, (int &)nFlags);
	if(nItemId != wxNOT_FOUND && (nFlags & wxLIST_HITTEST_ONITEMICON))
	{        
        LPITEM_DATA pdata = (LPITEM_DATA)wxListView::GetItemData(nItemId);
        pdata->bChanged = !pdata->bChanged;
        pdata->nCheckState = !pdata->nCheckState;
        bool bCheck = pdata->nCheckState != 0;
        SetItemImage(nItemId, bCheck == true ? 1 : 0, bCheck == true ? 1 : 0);
	}
}

bool wxGISCheckList::IsItemChanged(long item)
{
    LPITEM_DATA pdata = (LPITEM_DATA)wxListView::GetItemData(item);
    return pdata->bChanged;
}


int wxGISCheckList::GetItemCheckState(long item)
{
    LPITEM_DATA pdata = (LPITEM_DATA)wxListView::GetItemData(item);
    return pdata->nCheckState;
}


