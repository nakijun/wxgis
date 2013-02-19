/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxObject.
 * Author:   Baryshnikov Dmitriy (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2012 Bishop
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
#pragma once

#include "wxgis/catalog/catalog.h"

#include <wx/event.h>
#include <wx/list.h>

#include <map>

/** \class wxGxObject gxobject.h
    \brief The base class for catalog items
*/

class WXDLLIMPEXP_GIS_CLT wxGxObject : 
    public wxEvtHandler
{
    wxDECLARE_ABSTRACT_CLASS(wxGxObject);
public:
    wxGxObject(void);
    wxGxObject(wxGxObject *oParent, const wxString &soName = wxEmptyString, const CPLString &soPath = "");
    virtual ~wxGxObject(void);
    virtual bool Create(wxGxObject *oParent = NULL, const wxString &soName = wxEmptyString, const CPLString &soPath = "");
    virtual bool Destroy(void);
    virtual void SetParent(wxGxObject *oParent){m_oParent = oParent;};
    virtual wxString GetName(void) const {return m_sName;};
    virtual CPLString GetPath(void) const {return m_sPath;};
    virtual void SetName(const wxString &soName){m_sName = soName;};
    virtual void SetPath(const CPLString &soPath){m_sPath = soPath;};
    //the GxObject name without extension
	virtual wxString GetBaseName(void) const; 
    virtual wxString GetFullName(void) const;
    virtual wxString GetCategory(void) const {return wxEmptyString;};
    virtual wxGxObject *GetParent(void) const {return m_oParent;};
    virtual void Refresh(void);
	virtual long GetId(void) const {return m_nId;};
	virtual void SetId(long nId){m_nId = nId;};
protected:
    wxString m_sName;
    CPLString m_sPath;
    wxGxObject *m_oParent;
	long m_nId;
};

WX_DECLARE_LIST_2(wxGxObject, wxGxObjectList, wxGxObjectListNode, class WXDLLIMPEXP_GIS_CLT);

/** \class wxGxObjectContainer catalog.h
    \brief A GxObject with children GxObjects.
*/

class WXDLLIMPEXP_GIS_CLT wxGxObjectContainer : public wxGxObject
{
    wxDECLARE_ABSTRACT_CLASS(wxGxObjectContainer);
public:
    wxGxObjectContainer(void);
    wxGxObjectContainer(wxGxObject *oParent, const wxString &soName = wxEmptyString, const CPLString &soPath = "");
    virtual ~wxGxObjectContainer(void);
    virtual void AddChild( wxGxObject *child );
    virtual void RemoveChild( wxGxObject *child );
    virtual bool DestroyChild( wxGxObject *child );
    wxGxObject *FindGxObject(const wxString &sPath) const;
    bool IsNameExist(const wxString &sName) const;
    virtual bool DestroyChildren();
    virtual bool HasChildren(void){return !m_Children.IsEmpty();};
    virtual bool AreChildrenViewable(void) = 0;
    const wxGxObjectList& GetChildren() const { return m_Children; };
    virtual bool CanCreate(long nDataType, long DataSubtype){return false;};
    //wxGxObject
    virtual bool Destroy(void);
    virtual wxGxObjectList& GetChildren() { return m_Children; }
    virtual void Refresh(void);
protected:
    wxGxObjectList m_Children;
};

/** \class wxGxCatalogBase gxobject.h
    \brief The root GxObject class for GxObject items
*/

class WXDLLIMPEXP_GIS_CLT wxGxCatalogBase : public wxGxObjectContainer
{
    wxDECLARE_ABSTRACT_CLASS(wxGxCatalogBase);
public:
    wxGxCatalogBase(wxGxObject *oParent = NULL, const wxString &soName = _("Catalog"), const CPLString &soPath = "");
    virtual ~wxGxCatalogBase(void);
    //wxGxObject
    virtual wxString GetFullName(void) const {return wxEmptyString;}    
    virtual wxString GetCategory(void) const {return wxString(wxT("Root"));};
    //wxGxObjectContainer
    virtual bool AreChildrenViewable(void){return true;};
    wxGxObject *FindGxObject(const wxString &sPath) const;
    //wxGxCatalog
    virtual wxString ConstructFullName(const wxGxObject* pObject) const;
    virtual bool Destroy(void);
	//register / unregister pointer and uniq ID
	virtual void RegisterObject(wxGxObject* pObj);
	virtual void UnRegisterObject(long nId);
	//get pointer by ID
	virtual wxGxObject* const GetRegisterObject(long nId);
    //Initialization
    virtual bool Init(void);
    //
    virtual bool GetShowHidden(void){return m_bShowHidden;};
	virtual bool GetShowExt(void){return m_bShowExt;};
	virtual void SetShowHidden(bool bShowHidden){m_bShowHidden = bShowHidden;};
	virtual void SetShowExt(bool bShowExt){m_bShowExt = bShowExt;};
    //gxobject children factory
    virtual bool GetChildren(wxGxObject* pParent, char** &pFileNames, wxArrayLong & pChildrenIds) = 0;
    //events
    virtual void ObjectAdded(long nObjectID) = 0;
	virtual void ObjectChanged(long nObjectID) = 0;
	virtual void ObjectDeleted(long nObjectID) = 0;
	virtual void ObjectRefreshed(long nObjectID) = 0;
protected:
	virtual wxString GetConfigName(void) const = 0;
    virtual void LoadObjectFactories() = 0;
    virtual void LoadChildren() = 0;
protected:
    long m_nGlobalId;
    std::map<long, wxGxObject*> m_moGxObject; //map of registered IGxObject pointers
    bool m_bIsInitialized;
    bool m_bShowHidden, m_bShowExt;
};

/** \fn wxGxCatalogBase * const GetGxCatalogBase(void)
 *  \brief Global GxCatalog getter.
 */	

WXDLLIMPEXP_GIS_CLT wxGxCatalogBase* const GetGxCatalog(void);

/** \fn void SetGxCatalog(wxGxCatalogBase* pCat)
    \brief Global Catalog setter.
	\param pCat The Catalog pointer.
 */	

WXDLLIMPEXP_GIS_CLT void SetGxCatalog(wxGxCatalogBase* pCat);

