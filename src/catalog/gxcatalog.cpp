/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxCatalog class.
 * Author:   Baryshnikov Dmitriy (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2009-2013 Bishop
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
#include "wxgis/catalog/gxcatalog.h"
#include "wxgis/catalog/gxevent.h"
#include "wxgis/catalog/gxobjectfactory.h"
#include "wxgis/core/format.h"

// ----------------------------------------------------------------------------
// wxGxCatalog
// ----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxGxCatalog, wxGxCatalogBase); 

wxGxCatalog::wxGxCatalog(wxGxObject *oParent, const wxString &soName, const CPLString &soPath) : wxGxCatalogBase(oParent, soName, soPath)
{
}

wxGxCatalog::~wxGxCatalog(void)
{
}

void wxGxCatalog::ObjectDeleted(long nObjectID)
{
	wxGxCatalogEvent event(wxGXOBJECT_DELETED, nObjectID);
	PostEvent(event);
}

void  wxGxCatalog::ObjectAdded(long nObjectID)
{
	wxGxCatalogEvent event(wxGXOBJECT_ADDED, nObjectID);
	PostEvent(event);
}

void  wxGxCatalog::ObjectChanged(long nObjectID)
{
	wxGxCatalogEvent event(wxGXOBJECT_CHANGED, nObjectID);
	PostEvent(event);
}

void  wxGxCatalog::ObjectRefreshed(long nObjectID)
{
	wxGxCatalogEvent event(wxGXOBJECT_REFRESHED, nObjectID);
	PostEvent(event);
}

void wxGxCatalog::LoadChildren(void)
{
	wxGISAppConfig oConfig = GetConfig();
	if(!oConfig.IsOk())
		return;
	//loads current user and when local machine items
	wxXmlNode* pRootItemsNode = oConfig.GetConfigNode(enumGISHKCU, GetConfigName() + wxString(wxT("/catalog/rootitems")));
	LoadChildren(pRootItemsNode);
	pRootItemsNode = oConfig.GetConfigNode(enumGISHKLM, GetConfigName() + wxString(wxT("/catalog/rootitems")));
	LoadChildren(pRootItemsNode);
}

void wxGxCatalog::LoadChildren(wxXmlNode* const pNode)
{
    wxCHECK_RET(pNode, wxT("Intput config xml node is null"));

	wxXmlNode* pChildren = pNode->GetChildren();
	while(pChildren)
	{
		wxString sCatalogRootItemName = pChildren->GetAttribute(wxT("name"), NONAME);
        bool bIsEnabled = GetBoolValue(pChildren, wxT("is_enabled"), true);

        if(m_CatalogRootItemArray.Index(sCatalogRootItemName, false) != wxNOT_FOUND)
        {
            pChildren = pChildren->GetNext();
            continue;
        }

		//init plugin and add it
        wxObject *obj = wxCreateDynamicObject(sCatalogRootItemName);
		wxGxObject *pGxObject = dynamic_cast<wxGxObject*>(obj);

        if(pGxObject)
        {
            ROOTITEM rt = {sCatalogRootItemName, pGxObject->GetName(), bIsEnabled, pChildren};
            m_staRootitems.push_back(rt);
        }

        IGxRootObjectProperties* pGxRootObjectProperties = dynamic_cast<IGxRootObjectProperties*>(pGxObject);
        if(bIsEnabled && pGxObject && pGxRootObjectProperties && pGxObject->Create(this) )
		{
            pGxRootObjectProperties->Init(pChildren);
    		wxLogMessage(_("wxGxCatalog: Root Object %s initialized"), sCatalogRootItemName.c_str());
            //prevent duplicates
            m_CatalogRootItemArray.Add(sCatalogRootItemName);
		}
		else
        {
            wxDELETE(obj);
			wxLogError(_("wxGxCatalog: Root Object %s disabled"), sCatalogRootItemName.c_str());
        }

		pChildren = pChildren->GetNext();
	}
}

void wxGxCatalog::LoadObjectFactories(void)
{
	wxGISAppConfig oConfig = GetConfig();
	if(!oConfig.IsOk())
		return;
	//loads current user and when local machine items
	wxXmlNode* pObjectFactoriesNode = oConfig.GetConfigNode(enumGISHKCU, GetConfigName() + wxString(wxT("/catalog/objectfactories")));
	LoadObjectFactories(pObjectFactoriesNode);
	pObjectFactoriesNode = oConfig.GetConfigNode(enumGISHKLM, GetConfigName() + wxString(wxT("/catalog/objectfactories")));
	LoadObjectFactories(pObjectFactoriesNode);
}

void wxGxCatalog::LoadObjectFactories(const wxXmlNode* pNode)
{
	if(pNode == NULL)
		return;

	wxXmlNode* pChildren = pNode->GetChildren();
	while(pChildren)
	{
		wxString sName = pChildren->GetAttribute(wxT("factory_name"), wxT(""));

		for(size_t i = 0; i < m_ObjectFactoriesArray.size(); ++i)
		{
            if(m_ObjectFactoriesArray[i]->GetClassName() == sName)
			{
				sName.Empty();
				break;
			}
		}

		if(!sName.IsEmpty())
		{
			wxObject *obj = wxCreateDynamicObject(sName);
			wxGxObjectFactory *pFactory = dynamic_cast<wxGxObjectFactory*>(obj);
			if(pFactory != NULL)
			{				
				pFactory->Serialize(pChildren, false);
				m_ObjectFactoriesArray.push_back( pFactory );
				wxLogMessage(_("wxGxCatalog: ObjectFactory %s initialize"), sName.c_str());
			}
			else
				wxLogError(_("wxGxCatalog: Error initializing ObjectFactory %s"), sName.c_str());
		}
		pChildren = pChildren->GetNext();
	}
}

bool wxGxCatalog::CreateChildren(wxGxObject* pParent, char** &pFileNames, wxArrayLong & pChildrenIds)
{
	for(size_t i = 0; i < m_ObjectFactoriesArray.size(); ++i)
	{
        if(m_ObjectFactoriesArray[i]->GetEnabled())
			if(!m_ObjectFactoriesArray[i]->GetChildren(pParent, pFileNames, pChildrenIds))
				return false;
	}
	return true;
}

void wxGxCatalog::SerializePlugins(wxXmlNode* const pNode, bool bStore)
{
	if(bStore)
	{
        wxXmlNode* pChildNode = pNode->GetChildren();
        while(pChildNode)
        {
            for(size_t i = 0; i < m_staRootitems.size(); ++i)
            {
                wxString sClassName = pChildNode->GetAttribute(wxT("name"));
                if(m_staRootitems[i].sClassName == sClassName)
                {
                    pChildNode->DeleteAttribute(wxT("is_enabled"));
                    SetBoolValue(pChildNode, wxT("is_enabled"), m_staRootitems[i].bEnabled);
                    break;
                }
            }
            pChildNode = pChildNode->GetNext();
        }
    }
 
}

void wxGxCatalog::EmptyObjectFactories(void)
{
	for(size_t i = 0; i < m_ObjectFactoriesArray.size(); ++i)
	{
		wxDELETE(m_ObjectFactoriesArray[i]);
	}
	m_ObjectFactoriesArray.clear();
}

void wxGxCatalog::EnableRootItem(size_t nItemId, bool bEnable)
{
    m_staRootitems[nItemId].bEnabled = bEnable;

    if(bEnable) //enable
    {

		//init plugin and add it
        wxObject *obj = wxCreateDynamicObject(m_staRootitems[nItemId].sClassName);
		wxGxObject *pGxObject = dynamic_cast<wxGxObject*>(obj);

        IGxRootObjectProperties* pGxRootObjectProperties = dynamic_cast<IGxRootObjectProperties*>(pGxObject);
        if(pGxObject && pGxRootObjectProperties && pGxObject->Create(this) )
		{
            pGxRootObjectProperties->Init(m_staRootitems[nItemId].pConfig);
    		wxLogMessage(_("wxGxCatalog: Root Object %s initialized"), m_staRootitems[nItemId].sName.c_str());
            ObjectAdded(pGxObject->GetId());
		}
		else
        {
            wxDELETE(obj);
			wxLogError(_("wxGxCatalog: Root Object %s disabled"), m_staRootitems[nItemId].sName.c_str());
        }        
    }
    else        //disable
    {
        for(size_t i = 0; i < m_Children.size(); ++i)
        {
            if(m_Children[i]->GetName().IsSameAs(m_staRootitems[nItemId].sName, false))
            {    
                DestroyChild(m_Children[i]);
                break;
            }
        }
    }
}

bool wxGxCatalog::Destroy(void)
{
    m_pPointsArray.clear();

    //store to config values
	wxGISAppConfig oConfig = GetConfig();
	if(oConfig.IsOk())
	{
		oConfig.Write(enumGISHKCU, GetConfigName() + wxString(wxT("/catalog/show_hidden")), m_bShowHidden);
		oConfig.Write(enumGISHKCU, GetConfigName() + wxString(wxT("/catalog/show_ext")), m_bShowExt);

		wxXmlNode* pNode = oConfig.GetConfigNode(enumGISHKCU, GetConfigName() + wxString(wxT("/catalog/rootitems")));
		if(!pNode)
			pNode = oConfig.CreateConfigNode(enumGISHKCU, GetConfigName() + wxString(wxT("/catalog/rootitems")));
		if(pNode)
		{
			SerializePlugins(pNode, true);
		}

		pNode = oConfig.GetConfigNode(enumGISHKCU, GetConfigName() + wxString(wxT("/catalog/objectfactories")));
		if(!pNode)
			pNode = oConfig.CreateConfigNode(enumGISHKCU, GetConfigName() + wxString(wxT("/catalog/objectfactories")));
		if(pNode)
		{
			oConfig.DeleteNodeChildren(pNode);
			for(int i = m_ObjectFactoriesArray.size() - 1; i >= 0; --i)
			{
				wxXmlNode* pFactoryNode = new wxXmlNode(pNode, wxXML_ELEMENT_NODE, wxT("objectfactory"));
				m_ObjectFactoriesArray[i]->Serialize(pFactoryNode, true);
			}
		}
	}

	EmptyObjectFactories();

    return wxGxCatalogBase::Destroy();
}

wxGxObject* const wxGxCatalog::GetRootItemByType(const wxClassInfo * info) const
{
    wxGxObjectList::const_iterator iter;
    for(iter = wxGxObjectContainer::GetChildren().begin(); iter != wxGxObjectContainer::GetChildren().end(); ++iter)
    {
        wxGxObject *current = *iter;
        if(current && current->IsKindOf(info))
            return current;
    }
    return NULL;
}

wxGxObjectFactory* const wxGxCatalog::GetObjectFactoryByClassName(const wxString &sClassName)
{
    for(size_t i = 0; i < m_ObjectFactoriesArray.size(); ++i)
    {
        if(m_ObjectFactoriesArray[i]->GetClassName().IsSameAs(sClassName))
            return m_ObjectFactoriesArray[i];
    }
    return NULL;
}

wxGxObjectFactory* const wxGxCatalog::GetObjectFactoryByName(const wxString &sFactoryName)
{
    for(size_t i = 0; i < m_ObjectFactoriesArray.size(); ++i)
    {
        if(m_ObjectFactoriesArray[i]->GetName().IsSameAs(sFactoryName))
            return m_ObjectFactoriesArray[i];
    }
    return NULL;
}

wxVector<wxGxCatalog::ROOTITEM>* const wxGxCatalog::GetRootItems(void)
{
    return &m_staRootitems;
}



/*
#include "wxgis/core/globalfn.h"
#include "wxgis/core/config.h"


	EmptyObjectFactories();
	EmptyChildren();
	EmptyDisabledChildren();

wxGxCatalog::wxGxCatalog(void) : m_bIsChildrenLoaded(false), m_pGxDiscConnections(NULL), m_nGlobalID(0)
{
	m_pCatalog = this;
	m_pParent = NULL;

    m_bShowHidden = false;
    m_bShowExt = true;

	RegisterObject(this); 
}

wxGxCatalog::~wxGxCatalog(void)
{
}

bool wxGxCatalog::Attach(IGxObject* pParent, IGxCatalog* pCatalog)
{
	m_pParent = NULL;	
	m_pCatalog = this; 
	return true;
}

void wxGxCatalog::Detach(void)
{
	m_pPointsArray.clear();

	m_pCatalog->UnRegisterObject(m_nID);

	wxGISAppConfig oConfig = GetConfig();
	if(oConfig.IsOk())
	{
		oConfig.Write(enumGISHKCU, GetConfigName() + wxString(wxT("/catalog/show_hidden")), m_bShowHidden);
		oConfig.Write(enumGISHKCU, GetConfigName() + wxString(wxT("/catalog/show_ext")), m_bShowExt);

		wxXmlNode* pNode = oConfig.GetConfigNode(enumGISHKCU, GetConfigName() + wxString(wxT("/catalog/rootitems")));
		if(!pNode)
			pNode = oConfig.CreateConfigNode(enumGISHKCU, GetConfigName() + wxString(wxT("/catalog/rootitems")));
		if(pNode)
		{
			oConfig.DeleteNodeChildren(pNode);
			SerializePlugins(pNode, true);
		}

		pNode = oConfig.GetConfigNode(enumGISHKCU, GetConfigName() + wxString(wxT("/catalog/objectfactories")));
		if(!pNode)
			pNode = oConfig.CreateConfigNode(enumGISHKCU, GetConfigName() + wxString(wxT("/catalog/objectfactories")));
		if(pNode)
		{
			oConfig.DeleteNodeChildren(pNode);
			for(int i = m_ObjectFactoriesArray.size(); i >= 0; --i)
			{
				wxXmlNode* pFactoryNode = new wxXmlNode(pNode, wxXML_ELEMENT_NODE, wxT("objectfactory"));
				m_ObjectFactoriesArray[i - 1]->Serialize(pFactoryNode, true);
			}
		}
	}

	EmptyObjectFactories();
	EmptyChildren();
	EmptyDisabledChildren();
}

void wxGxCatalog::Refresh(void)
{
	for(size_t i = 0; i < m_Children.size(); ++i)
		m_Children[i]->Refresh();
	if(m_pCatalog)
		m_pCatalog->ObjectRefreshed(GetID());
}

void wxGxCatalog::EmptyChildren(void)
{
	for(size_t i = 0; i < m_Children.size(); ++i)
	{
		m_Children[i]->Detach();
		delete m_Children[i];
	}
	m_Children.clear();
	m_bIsChildrenLoaded = false;
}

void wxGxCatalog::EmptyObjectFactories(void)
{
	for(size_t i = 0; i < m_ObjectFactoriesArray.size(); ++i)
	{
		wxDELETE(m_ObjectFactoriesArray[i]);
	}
	m_ObjectFactoriesArray.clear();
}

void wxGxCatalog::Init(void)
{
	if(m_bIsChildrenLoaded)
		return;

	LoadObjectFactories();
	LoadChildren();
}

void wxGxCatalog::LoadChildren(void)
{
	wxGISAppConfig oConfig = GetConfig();
	if(!oConfig.IsOk())
		return;
	//loads current user and when local machine items
	wxXmlNode* pRootItemsNode = oConfig.GetConfigNode(enumGISHKCU, GetConfigName() + wxString(wxT("/catalog/rootitems")));
	LoadChildren(pRootItemsNode);
	pRootItemsNode = oConfig.GetConfigNode(enumGISHKLM, GetConfigName() + wxString(wxT("/catalog/rootitems")));
	LoadChildren(pRootItemsNode);
}

void wxGxCatalog::LoadChildren(wxXmlNode* const pNode)
{
	if(!pNode)
		return;

	wxXmlNode* pChildren = pNode->GetChildren();
	while(pChildren)
	{
		wxString sCatalogRootItemName = pChildren->GetAttribute(wxT("name"), NONAME);
        bool bIsEnabled = wxAtoi(pChildren->GetAttribute(wxT("is_enabled"), wxT("1"))) != 0;
        bool bContin = false;

		for(size_t i = 0; i < m_CatalogRootItemArray.Count(); ++i)
		{
			if(m_CatalogRootItemArray[i].IsSameAs(sCatalogRootItemName, false))
			{
				pChildren = pChildren->GetNext();
                bContin = true;
				break;
			}
		}

        if(bContin)
            continue;

        m_CatalogRootItemArray.Add(sCatalogRootItemName);

		//init plugin and add it
        wxObject *obj = wxCreateDynamicObject(sCatalogRootItemName);
		IGxObject *pGxObject = dynamic_cast<IGxObject*>(obj);
		if(pGxObject != NULL)
		{
            if(bIsEnabled)
            {
                if(AddChild(pGxObject))
                {
                    //m_CatalogRootItemArray.Add(sCatalogRootItemName);
                    IGxRootObjectProperties* pGxRootObjectProperties = dynamic_cast<IGxRootObjectProperties*>(pGxObject);
                    if(pGxRootObjectProperties)
                    {
                        pGxRootObjectProperties->Init(pChildren);
                        pGxRootObjectProperties->SetEnabled(bIsEnabled);
                    }
                    wxGxDiscConnections* pGxDiscConnections = dynamic_cast<wxGxDiscConnections*>(pGxObject);
                    if(pGxDiscConnections)
                        m_pGxDiscConnections = pGxDiscConnections;

				    wxLogMessage(_("wxGxCatalog: Root Object %s initialize"), sCatalogRootItemName.c_str());
                }
            }
            else
            {
                pGxObject->Attach(this, this);
                IGxRootObjectProperties* pGxRootObjectProperties = dynamic_cast<IGxRootObjectProperties*>(pGxObject);
                if(pGxRootObjectProperties)
                {
                    pGxRootObjectProperties->Init(pChildren);
                    pGxRootObjectProperties->SetEnabled(bIsEnabled);
                }
                m_aRootItems.push_back(pGxObject);
            }
		}
		else
			wxLogError(_("wxGxCatalog: Error initializing Root Object %s"), sCatalogRootItemName.c_str());

		pChildren = pChildren->GetNext();
	}
	m_bIsChildrenLoaded = true;
}

bool wxGxCatalog::GetChildren(CPLString sParentDir, char** &pFileNames, GxObjectArray &ObjArray)
{
	for(size_t i = 0; i < m_ObjectFactoriesArray.size(); ++i)
	{
        if(m_ObjectFactoriesArray[i]->GetEnabled())
			if(!m_ObjectFactoriesArray[i]->GetChildren(sParentDir, pFileNames, ObjArray))
				return false;
	}
	return true;
}

IGxObject* wxGxCatalog::ConnectFolder(wxString sPath, bool bSelect)
{
    if(m_pGxDiscConnections)
    {
        IGxObject* pAddedObj = m_pGxDiscConnections->ConnectFolder(sPath);
        if(pAddedObj)
            return pAddedObj;
    }
    return NULL;
}

IGxObject* wxGxCatalog::SearchChild(wxString sPath)
{
    IGxObject* pOutObj = IGxObjectContainer::SearchChild(sPath);
    if(pOutObj)
        return pOutObj;

    wxFileName oName(sPath);
    //the container is exist but item is absent
    pOutObj = IGxObjectContainer::SearchChild(oName.GetPath());
    if(pOutObj)
        return NULL;
    //try connect path & search child
    IGxObjectContainer* poObjCont = dynamic_cast<IGxObjectContainer*>(ConnectFolder(oName.GetPath(), false));
    if(poObjCont == this)
        return NULL;
    if(!poObjCont)
        return poObjCont;
    return poObjCont->SearchChild(sPath);
}


void wxGxCatalog::DisconnectFolder(CPLString sPath)
{
    if(m_pGxDiscConnections)
        m_pGxDiscConnections->DisconnectFolder(sPath);
}

void wxGxCatalog::RegisterObject(IGxObject* pObj)
{
	wxCriticalSectionLocker locker(m_RegCritSect);
	pObj->SetID(m_nGlobalID);
	GxObjectMap[m_nGlobalID] = pObj;
	m_nGlobalID++;
}

void wxGxCatalog::UnRegisterObject(long nID)
{
	wxCriticalSectionLocker locker(m_RegCritSect);
	GxObjectMap[nID] = NULL;
}

IGxObjectSPtr wxGxCatalog::GetRegisterObject(long nID)
{
	wxCriticalSectionLocker locker(m_RegCritSect);
	if(wxNOT_FOUND == nID)
		return IGxObjectSPtr();

	if(GxObjectMap[nID] != NULL)
	{
		//if(GxObjectMap[nID]->GetID() != nID)
		//	return IGxObjectSPtr();
		GxObjectMap[nID]->Lock();
		return IGxObjectSPtr(GxObjectMap[nID], GxObjectDeleter);
	}
	return IGxObjectSPtr();
}

wxString wxGxCatalog::ConstructFullName(IGxObject* pObject)
{
	wxString sParentPath = pObject->GetParent()->GetFullName();
    wxString sName = pObject->GetName();

	if(sParentPath.IsEmpty())
		return sName;
	else
	{        
		if(sParentPath.EndsWith(wxFileName::GetPathSeparator()))
			return sParentPath + sName;
		else
			return sParentPath + wxFileName::GetPathSeparator() + sName;
	}
}

*/
