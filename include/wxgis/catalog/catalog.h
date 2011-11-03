/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  Catalog main header.
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

#pragma once

#include "wxgis/datasource/datasource.h"
#include "wxgis/core/config.h"

#include <wx/filename.h>

#define NOTFIRESELID    1010
#define CONNDIR wxT("connections")

enum wxGISEnumSaveObjectResults
{
	enumGISSaveObjectNone = 0x0000,
	enumGISSaveObjectAccept = 0x0001,
	enumGISSaveObjectExists = 0x0002,
	enumGISSaveObjectDeny = 0x0004
};

class IGxObject;
typedef std::vector<IGxObject*> GxObjectArray;
DEFINE_SHARED_PTR(IGxObject);

class IGxObjectFactory;
typedef std::vector<IGxObjectFactory*> GxObjectFactoryArray;

class IGxCatalog
{
public:
	virtual ~IGxCatalog(void){};
	virtual wxString ConstructFullName(IGxObject* pObject) = 0;
	virtual bool GetChildren(CPLString sParentDir, char** &pFileNames, GxObjectArray &ObjArray) = 0;
	virtual GxObjectArray* const GetDisabledRootItems(void){return &m_aRootItems;};
    virtual GxObjectFactoryArray* const GetObjectFactories(void){return &m_ObjectFactoriesArray;};
	virtual void ObjectAdded(long nObjectID) = 0;
	virtual void ObjectChanged(long nObjectID) = 0;
	virtual void ObjectDeleted(long nObjectID) = 0;
	virtual void ObjectRefreshed(long nObjectID) = 0;
	virtual IGxObject* ConnectFolder(wxString sPath, bool bSelect = true) = 0;
	virtual void DisconnectFolder(CPLString sPath) = 0;
    virtual void EnableRootItem(IGxObject* pRootItem, bool bEnable) = 0;
	virtual bool GetShowHidden(void){return m_bShowHidden;};
	virtual bool GetShowExt(void){return m_bShowExt;};
	virtual void SetShowHidden(bool bShowHidden){m_bShowHidden = bShowHidden;};
	virtual void SetShowExt(bool bShowExt){m_bShowExt = bShowExt;};
	//register / unregister pointer and uniq ID
	virtual void RegisterObject(IGxObject* pObj) = 0;
	virtual void UnRegisterObject(long nID) = 0;
	//get smart pointer by ID
	virtual IGxObjectSPtr GetRegisterObject(long nID) = 0;
protected:
    GxObjectArray m_aRootItems;
    GxObjectFactoryArray m_ObjectFactoriesArray;
	bool m_bShowHidden, m_bShowExt;
	std::map<long, IGxObject*> GxObjectMap; //map of registered IGxObject pointers
};

class IGxObject
{
public:
	virtual ~IGxObject(void){};
	virtual bool Attach(IGxObject* pParent, IGxCatalog* pCatalog)
	{
		m_pParent = pParent;
		m_pCatalog = pCatalog;
		if(m_pCatalog)
			m_pCatalog->RegisterObject(this);
		return true;
	};
	virtual void Detach(void)
	{
		wxCriticalSectionLocker locker(m_DestructCritSect);
		if(m_pCatalog)
			m_pCatalog->UnRegisterObject(m_nID);
		m_pParent = NULL;
		m_pCatalog = NULL;
	};
	virtual wxString GetName(void) = 0;
	virtual wxString GetBaseName(void) = 0; //the name without ext
	virtual wxString GetFullName(void)
	{
		if(m_pCatalog)
			return m_pCatalog->ConstructFullName(this);
		else
			return wxEmptyString;
	};
	virtual CPLString GetInternalName(void) = 0;
	virtual wxString GetCategory(void) = 0;
	virtual IGxObject* const GetParent(void){return m_pParent;};
	virtual void Refresh(void){};
public:
	//uniq ID for selection
	virtual long GetID(void){return m_nID;};
	virtual void SetID(long nID){m_nID = nID;};
	//lock and unlock delete/move cupubility
	virtual void Lock(void){m_DestructCritSect.Enter();};
	virtual void Unlock(void){m_DestructCritSect.Leave();};
protected:
	IGxObject* m_pParent;
	IGxCatalog* m_pCatalog;
protected:
	long m_nID;
	wxCriticalSection m_DestructCritSect;
};

static void GxObjectDeleter(IGxObject *ptr)
{
    if(ptr)
        ptr->Unlock();
}
/** \class IGxObjectEdit catalog.h
    \brief A GxObject edit interface.
*/
class IGxObjectEdit
{
public:
	virtual ~IGxObjectEdit(void){};
	virtual bool Delete(void){return false;};
	virtual bool CanDelete(void){return false;};
	virtual bool Rename(wxString NewName){return false;};
	virtual bool CanRename(void){return false;};
	virtual bool Copy(CPLString szDestPath, ITrackCancel* pTrackCancel){return false;};
	virtual bool CanCopy(CPLString szDestPath){return false;};
	virtual bool Move(CPLString szDestPath, ITrackCancel* pTrackCancel){return false;};
	virtual bool CanMove(CPLString szDestPath){return false;};
};

class IGxObjectContainer :
	public IGxObject
{
public:
	virtual ~IGxObjectContainer(void){};
	virtual void Detach(void)
	{
		for(size_t i = 0; i < m_Children.size(); ++i)
		{
			m_Children[i]->Detach();
			delete m_Children[i];
		}
		m_Children.clear();
		IGxObject::Detach();
	};
	virtual bool AddChild(IGxObject* pChild)
	{
		if(pChild == NULL)
			return false;
		if(!pChild->Attach(this, m_pCatalog))
			return false;
		m_Children.push_back(pChild);
		return true;
	};
	virtual bool DeleteChild(IGxObject* pChild)
	{
		if(pChild == NULL)
			return false;
		GxObjectArray::iterator pos = std::find(m_Children.begin(), m_Children.end(), pChild);
		if(pos != m_Children.end())
		{
			pChild->Detach();
            delete pChild;
			m_Children.erase(pos);
		}
		return true;
	};
	virtual bool AreChildrenViewable(void) = 0;
	virtual bool HasChildren(void) = 0;
	virtual GxObjectArray* const GetChildren(void){return &m_Children;};
	virtual IGxObject* SearchChild(wxString sPath)
	{
		wxString sTestPath = sPath;
		if(GetFullName().CmpNoCase(sPath) == 0)//GetName
			return this;
		for(size_t i = 0; i < m_Children.size(); ++i)
		{
			wxString sTestedPath = m_Children[i]->GetFullName();
			if(sTestedPath.CmpNoCase(sPath) == 0)
				return m_Children[i];
            bool bHavePart = sPath.MakeLower().Find(sTestedPath.MakeLower()) != wxNOT_FOUND;
			if(bHavePart )
			{
				IGxObjectContainer* pContainer = dynamic_cast<IGxObjectContainer*>(m_Children[i]);
				if(pContainer && pContainer->HasChildren())
				{
					IGxObject* pFoundChild = pContainer->SearchChild(sPath);
					if(pFoundChild)
						return pFoundChild;
				}
			}
		}
		return NULL;
	}
    virtual bool CanCreate(long nDataType, long DataSubtype){return false;}; 
protected:
	GxObjectArray m_Children;
};

class IGxObjectFactory
{
public:
	IGxObjectFactory(void) : m_bIsEnabled(true){};
	virtual ~IGxObjectFactory(void){};
    //pure virtual
	virtual bool GetChildren(CPLString sParentDir, char** &pFileNames, GxObjectArray &ObjArray) = 0;
    virtual void Serialize(wxXmlNode* const pConfig, bool bStore) = 0;
    virtual wxString GetClassName(void) = 0;
    virtual wxString GetName(void) = 0;
    //
    virtual bool GetEnabled(void){return m_bIsEnabled;};
    virtual void SetEnabled(bool bIsEnabled){m_bIsEnabled = bIsEnabled;};
	virtual void PutCatalogRef(IGxCatalog* pCatalog){m_pCatalog = pCatalog;};
protected:
	IGxCatalog* m_pCatalog;
    bool m_bIsEnabled;
};

class IGxDataset
{
public:
	virtual ~IGxDataset(void){};
	virtual wxGISDatasetSPtr GetDataset(bool bCached = true, ITrackCancel* pTrackCancel = NULL) = 0;
	virtual wxGISEnumDatasetType GetType(void) = 0;
	virtual int GetSubType(void) = 0;
};

class IGxFile
{
public:
	virtual ~IGxFile(void){};
	//virtual bool Open(void) = 0;
	//virtual bool Close(bool bSaveEdits) = 0;
	//virtual void Edit(void) = 0;
	//virtual void New(void) = 0;
	//virtual bool Save(void) = 0;
};

class IGxRootObjectProperties
{
public:
	virtual ~IGxRootObjectProperties(void){};
	virtual void Init(wxXmlNode* const pConfigNode) = 0;
	virtual void Serialize(wxXmlNode* pConfigNode) = 0;
    virtual bool GetEnabled(void){return m_bEnabled;};
    virtual void SetEnabled(bool bEnabled){m_bEnabled = bEnabled;};
protected:
    bool m_bEnabled;
};

class IGxObjectSort
{
public:
	virtual ~IGxObjectSort(void){};
	virtual bool IsAlwaysTop(void) = 0;
	virtual bool IsSortEnabled(void) = 0;
};

class IGxObjectFilter
{
public:
	virtual ~IGxObjectFilter(void){};
	virtual bool CanChooseObject( IGxObject* pObject ) = 0;//, esriDoubleClickResult* result
	virtual bool CanDisplayObject( IGxObject* pObject ) = 0;
	virtual wxGISEnumSaveObjectResults CanSaveObject( IGxObject* pLocation, wxString sName ) = 0;
	virtual wxString GetName(void) = 0;
    virtual wxString GetExt(void) = 0;
    virtual wxString GetDriver(void) = 0;
    virtual int GetSubType(void) = 0;
};

typedef std::vector<IGxObjectFilter*> OBJECTFILTERS, *LPOBJECTFILTERS;

static wxString GetConvName(CPLString szPath)
{
    //name conv cp866 if zip
    wxString name;
    if( EQUALN(szPath,"/vsizip/",8) )
	{
		wxString sCharset(wxT("cp-866"));
		wxGISAppConfigSPtr pConfig = GetConfig();
		if(pConfig)
			sCharset = pConfig->Read(enumGISHKCU, wxString(wxT("wxGISCommon/zip/charset")), sCharset);
        name = wxString(CPLGetFilename(szPath), wxCSConv(sCharset));
	}
    else
        name = wxString(CPLGetFilename(szPath), wxConvUTF8);
	return name;
}



