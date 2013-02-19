/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxLocationComboBox class
 * Author:   Baryshnikov Dmitriy (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2009,2011,2012 Bishop
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
#include "wxgis/catalogui/catalogui.h"
#include "wxgis/catalogui/gxapplication.h"
#include "wxgis/catalogui/gxeventui.h"
#include "wxgis/catalogui/gxcontentview.h"

#include <wx/combo.h>
#include <wx/combobox.h>
#include <wx/popupwin.h>

#define LISTVIEWPOPUPID	1306 //wxGxCatalog contents view
#define LISTPOPUSTYLE (wxBORDER_SIMPLE | wxLC_SORT_ASCENDING | wxLC_AUTOARRANGE | wxLC_REPORT | wxLC_NO_HEADER | wxLC_SINGLE_SEL)

/** \class wxListViewComboPopup gxlocationcombobox.h
    \brief A list of gxobject full names with icons.
*/
class wxGxPathsListView : 
    public wxListCtrl
{
    DECLARE_CLASS(wxGxPathsListView)
public:
    wxGxPathsListView(void);
	wxGxPathsListView(wxWindow* parent, wxWindowID id = LISTVIEWPOPUPID, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = LISTPOPUSTYLE);
	virtual ~wxGxPathsListView(void);
    virtual bool Create(wxWindow* parent, wxWindowID id = LISTVIEWPOPUPID, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = LISTPOPUSTYLE, const wxString& name = wxT("ListViewComboPopup"));
    virtual void Append(const wxString& sFullName);
    //events
    // Do mouse hot-tracking (which is typical in list popups)
    virtual void OnMouseMove(wxMouseEvent& event);
     // On mouse left up, set the value and close the popup
    virtual void OnMouseClick(wxMouseEvent& event);
    virtual void OnChar(wxKeyEvent& event);
    //structures
	typedef struct _itemdata
	{
		long nObjectID;
		int iImageIndex;
	} ITEMDATA, *LPITEMDATA;
    typedef struct _icondata
	{
		wxIcon oIcon;
		int iImageIndex;
	} ICONDATA;
protected:
    wxVector<ICONDATA> m_IconsArray;
    int GetIconPos(const wxIcon &icon_small);
    virtual void Activate();
    wxImageList m_ImageListSmall;
protected:
    long m_HighLightItem;
private:
    DECLARE_EVENT_TABLE()
};


/** \class wxListViewComboPopup gxlocationcombobox.h
    \brief A popup with list of gxobject full names with icons.
*/
class wxListViewComboPopup : 
    public wxGxPathsListView,
    public wxComboPopup
{
    DECLARE_CLASS(wxListViewComboPopup)
public:
    // Initialize member variables
    virtual void Init();
    virtual void OnPopup();
    virtual void OnDismiss();
    // Create popup control
    virtual bool Create(wxWindow* parent);
    virtual bool Create(wxWindow* parent, wxWindowID id = LISTVIEWPOPUPID, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = LISTPOPUSTYLE, const wxString& name = wxT("ListViewComboPopup"));
    // Return pointer to the created control
    virtual wxWindow *GetControl() { return this; }
    // Translate string into a list selection
    virtual void SetStringValue(const wxString& s);
    // Get list selection as a string
    virtual wxString GetStringValue() const;
    virtual wxSize  GetAdjustedSize (int minWidth, int prefHeight, int maxHeight);
    //events
    virtual void OnMouseClick(wxMouseEvent& event);
    virtual void OnChar(wxKeyEvent& event);
protected:
    wxString m_sVal;
};

class wxGxPathsListViewPopup;

/** \class wxGxPathsListViewPopupParent gxlocationcombobox.h
    \brief A parent class for wxGxPathsListViewPopup.
*/
class wxGxPathsListViewPopupParent
{
public:
    wxGxPathsListViewPopupParent(void);
    virtual ~wxGxPathsListViewPopupParent(void);
    virtual void SetControlText(const wxString &str, bool bFireEvents = false) = 0;
    virtual void DestroyPathsPopup(void);
    virtual wxRect GetControlRect() const = 0;
    virtual wxSize GetControlSize() const = 0;
    //
    virtual void OnKillFocus(wxFocusEvent& event);
    virtual void OnChar(wxKeyEvent& event);
    virtual void OnMouseWheel(wxMouseEvent& event);
protected:
    wxGxPathsListViewPopup* m_pProbablePathsPopup;
};

/** \class wxGxPathsListViewPopup gxlocationcombobox.h
    \brief A popup window with list of gxobject probably full names with icons.
*/
class wxGxPathsListViewPopup :
    public wxPopupWindow
{
    DECLARE_CLASS(wxGxPathsListViewPopup)
public:
    wxGxPathsListViewPopup(wxWindow* parent, int nMaxHeight = 500);
    virtual ~wxGxPathsListViewPopup(void);
    void Append(const wxString& sFullName);
    //events
    void OnSize(wxSizeEvent& WXUNUSED(event));
    void OnMouseWheel(wxMouseEvent& event);
    void OnChar(wxKeyEvent& event);
    void OnMouseClick(wxMouseEvent& event);
    //
    wxRect GetViewRect() const;
    void Show(const wxString &sPath);
    void Update(const wxString &sPath);
protected:
    wxGxPathsListView* m_pGxPathsListView;
    int m_nItemHeight;
    wxGxPathsListViewPopupParent* m_pParent;
    int m_nMaxHeight;
private:
    DECLARE_EVENT_TABLE()
};


/** \class wxGxLocationComboBox gxlocationcombobox.h
    \brief A combobox to set gxcatalog path (selection).
*/

class wxGxLocationComboBox : 
	public wxComboCtrl,
    public wxGxPathsListViewPopupParent,
	public IToolBarControl
{
public:
	wxGxLocationComboBox(wxWindow* parent, wxWindowID id, const wxSize& size);
	virtual ~wxGxLocationComboBox(void);
    //wxGxPathsListViewPopupParent
    virtual void SetControlText(const wxString &str, bool bFireEvents = false);
    virtual wxRect GetControlRect() const;
    virtual wxSize GetControlSize() const;
	//events
	void OnTextEnter(wxCommandEvent& event);
	void OnText(wxCommandEvent& event);
	//IToolBarControl
	virtual void Activate(wxGISApplicationBase* pApp);
	virtual void Deactivate(void);
	//events
	virtual void OnSelectionChanged(wxGxSelectionEvent& event);
    virtual void OnKillFocus(wxFocusEvent& event);
    virtual void OnChar(wxKeyEvent& event);
    virtual void OnMouseWheel(wxMouseEvent& event);
protected:
	wxGxApplication* m_pApp;
	wxArrayString m_ValuesArr;
	long m_ConnectionPointSelectionCookie;
    wxGxSelection* m_pSelection;
    wxGxCatalogUI* m_pCatalog;
    wxListViewComboPopup* m_pListViewComboPopup;    
    wxWindow* m_pWnd;
private:
    DECLARE_EVENT_TABLE()
};
