/******************************************************************************
 * Project:  wxGIS (GIS Toolbox)
 * Purpose:  tools manager.
 * Author:   Bishop (aka Barishnikov Dmitriy), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2010-2011 Bishop
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

#include "wxgis/geoprocessing/gptoolmngr.h"
#include "wxgis/geoprocessing/gpparam.h"
#include "wxgis/core/config.h"

#include "wx/datetime.h"

///////////////////////////////////////////////////////////////////////////////
/// Class wxGPProcess
///////////////////////////////////////////////////////////////////////////////

wxGPProcess::wxGPProcess(wxString sCommand, IProcessParent* pParent, ITrackCancel* pTrackCancel) : wxGISProcess(sCommand, pParent), m_pProgressor(NULL)
{
    m_pTrackCancel = pTrackCancel;
	if(m_pTrackCancel)
		m_pProgressor = m_pTrackCancel->GetProgressor();
}

wxGPProcess::~wxGPProcess(void)
{
}

void wxGPProcess::OnTerminate(int pid, int status)
{
    wxGISProcess::OnTerminate(pid, status);
	if(m_pProgressor)
		m_pProgressor->SetValue(100);
}

void wxGPProcess::ProcessInput(wxString sInputData)
{
	if(m_nState != enumGISTaskWork)
        return;
//INFO, DONE, ERR, ??
	wxString sRest;
	if( sInputData.StartsWith(wxT("DONE: "), &sRest) )
	{
		int nPercent = wxAtoi(sRest.Trim().Truncate(sRest.Len() - 1));
		wxTimeSpan Elapsed = wxDateTime::Now() - m_dtBeg;//time left
        wxString sTxt;
        if(nPercent)
        {
            double nPercentR = 100 - nPercent;
            //wxTimeSpan Remains = Elapsed * (nPercentR / nPercent);
		    long dMSec = double(Elapsed.GetMilliseconds().ToDouble() * nPercentR) / nPercent;	
		    wxTimeSpan Remains = wxTimeSpan(0,0,0,dMSec);
		    m_dtEstEnd = m_dtBeg + Remains;
		    sTxt = wxString(_("Remains ")) + Remains.Format(_("%H hour(s) %M min. %S sec."));
        }
		if(m_pProgressor)
			m_pProgressor->SetValue(nPercent);

		if(m_pTrackCancel)
			m_pTrackCancel->PutMessage(sTxt, -1, enumGISMessageTitle);
		return;
	}
	if( sInputData.StartsWith(wxT("INFO: "), &sRest) )
	{
		if(m_pTrackCancel)
			m_pTrackCancel->PutMessage(sRest, -1, enumGISMessageNorm);
		return;
	}
	if( sInputData.StartsWith(wxT("ERR: "), &sRest) )
	{
		if(m_pTrackCancel)
			m_pTrackCancel->PutMessage(sRest, -1, enumGISMessageErr);
		return;
	}
	if( sInputData.StartsWith(wxT("WARN: "), &sRest) )
	{
		if(m_pTrackCancel)
			m_pTrackCancel->PutMessage(sRest, -1, enumGISMessageWarning);
		return;
	}
}

///////////////////////////////////////////////////////////////////////////////
/// Class wxGISGPToolManager
///////////////////////////////////////////////////////////////////////////////

wxGISGPToolManager::wxGISGPToolManager(wxXmlNode* pToolsNode) : m_nRunningTasks(0)
{
    m_pToolsNode = pToolsNode;
    m_nMaxTasks = wxAtoi(m_pToolsNode->GetPropVal(wxT("max_tasks"), wxString::Format(wxT("%d"), wxThread::GetCPUCount())));
#ifdef __WXMSW__
    m_sGeoprocessPath = m_pToolsNode->GetPropVal(wxT("gp_exec"), wxString(wxT("wxGISGeoprocess.exe")));
#else
    m_sGeoprocessPath = m_pToolsNode->GetPropVal(wxT("gp_exec"), wxString(wxT("wxGISGeoprocess")));
#endif
    wxXmlNode *pChild = m_pToolsNode->GetChildren();
    while (pChild)
    {
        wxString sName = pChild->GetPropVal(wxT("object"), NONAME);
        if(sName.IsEmpty() || sName.CmpNoCase(NONAME) == 0)
        {
            pChild = pChild->GetNext();
            continue;
        }

        int nCount = wxAtoi(pChild->GetPropVal(wxT("count"), wxT("0")));
        wxString sCmdName = pChild->GetPropVal(wxT("name"), NONAME);

	    wxObject *pObj = wxCreateDynamicObject(sName);
	    IGPTool *pTool = dynamic_cast<IGPTool*>(pObj);
	    if(pTool)
	    {
            sCmdName = pTool->GetName();
			TOOLINFO info = {sName, nCount, pTool};
			m_ToolsMap[sCmdName] = info;
			m_ToolsPopularMap.insert(std::make_pair(nCount, sCmdName));
        }
        pChild = pChild->GetNext();
    }
}

wxGISGPToolManager::~wxGISGPToolManager(void)
{
    if(m_pToolsNode->HasProp(wxT("max_tasks")))
        m_pToolsNode->DeleteProperty(wxT("max_tasks"));
    m_pToolsNode->AddProperty(wxT("max_tasks"), wxString::Format(wxT("%d"), m_nMaxTasks));
    if(m_pToolsNode->HasProp(wxT("gp_exec")))
        m_pToolsNode->DeleteProperty(wxT("gp_exec"));
    m_pToolsNode->AddProperty(wxT("gp_exec"), m_sGeoprocessPath);

    //read tasks
    wxGISConfig::DeleteNodeChildren(m_pToolsNode);
    for(std::map<wxString, TOOLINFO>::const_iterator pos = m_ToolsMap.begin(); pos != m_ToolsMap.end(); ++pos)
    {
        if(pos->second.sClassName.IsEmpty())
            continue;
        wxXmlNode* pNewNode = new wxXmlNode(m_pToolsNode, wxXML_ELEMENT_NODE, wxString(wxT("tool")));
		pNewNode->AddProperty(wxT("object"), pos->second.sClassName);
		pNewNode->AddProperty(wxT("name"), pos->first);
		pNewNode->AddProperty(wxT("count"), wxString::Format(wxT("%d"), pos->second.nCount));
    }

    //kill all processes
    for(size_t i = 0; i < m_ProcessArray.size(); i++)
    {
        CancelProcess(i);
        wxDELETE(m_ProcessArray[i].pProcess)
    }
    //delete all existed tools
    for(std::map<wxString, TOOLINFO>::iterator pos = m_ToolsMap.begin(); pos != m_ToolsMap.end(); ++pos)
        wxDELETE(pos->second.pTool);

}

IGPTool* wxGISGPToolManager::GetTool(wxString sToolName, IGxCatalog* pCatalog)
{
    if(sToolName.IsEmpty())
        return NULL;
    std::map<wxString, TOOLINFO>::const_iterator it = m_ToolsMap.find(sToolName);
    if(it != m_ToolsMap.end() && it->second.pTool)
    {
        if(!it->second.pTool->GetCatalog())
            it->second.pTool->SetCatalog(pCatalog);
	    return it->second.pTool;
    }
	wxObject *pObj = wxCreateDynamicObject(m_ToolsMap[sToolName].sClassName);
    IGPTool* pTool = dynamic_cast<IGPTool*>(pObj);
    if(!pTool)
    {
        TOOLINFO inf = {wxEmptyString, 0, NULL};
        m_ToolsMap[sToolName] = inf;
        return NULL;
    }
    pTool->SetCatalog(pCatalog);
    m_ToolsMap[sToolName].pTool = pTool;
	return pTool;
}

int wxGISGPToolManager::OnExecute(IGPTool* pTool, ITrackCancel* pTrackCancel, IGPCallBack* pCallBack)
{
    if(!pTool)
        return false;
    //get tool name and add it to stat of exec
    wxString sToolName = pTool->GetName();
    m_ToolsMap[sToolName].nCount++;

    wxString sToolParams = pTool->GetAsString();
    wxString sCommand = wxString::Format(wxT("%s -n "), m_sGeoprocessPath.c_str()) + sToolName + wxT(" -p \"") + sToolParams + wxT("\"");

    WXGISEXECDDATA data = {new wxGPProcess(sCommand, static_cast<IProcessParent*>(this), pTrackCancel), pTool, pTrackCancel, pCallBack};
    m_ProcessArray.push_back(data);
    int nTaskID = m_ProcessArray.size() - 1;
    AddPriority(nTaskID, nTaskID);

    if(m_nRunningTasks < m_nMaxTasks)
        ExecTask(m_ProcessArray[nTaskID]);
	else
		m_ProcessArray[nTaskID].pProcess->SetState(enumGISTaskQuered);

    return nTaskID;
}

bool wxGISGPToolManager::ExecTask(WXGISEXECDDATA &data)
{
    wxDateTime begin = wxDateTime::Now();
    if(data.pTrackCancel)
    {
        data.pTrackCancel->PutMessage(wxString::Format(_("Executing (%s)"), data.pTool->GetName().c_str()), -1, enumGISMessageInfo);
        //add some tool info
        GPParameters* pParams = data.pTool->GetParameterInfo();
        if(pParams)
        {
            data.pTrackCancel->PutMessage(wxString(_("Parameters:")), -1, enumGISMessageInfo);
            for(size_t i = 0; i < pParams->size(); i++)
            {
                IGPParameter* pParam = pParams->operator[](i);
                if(!pParam)
                    continue;
                wxString sParamName = pParam->GetName();
                wxString sParamValue = pParam->GetValue();
                data.pTrackCancel->PutMessage(wxString::Format(wxT("%s = %s"), sParamName.c_str(), sParamValue.c_str()), -1, enumGISMessageNorm);
            }
        }
        data.pTrackCancel->PutMessage(wxString::Format(_("Start Time: %s"), begin.Format().c_str()), -1, enumGISMessageInfo);
    }

	long nPID = wxExecute(data.pProcess->GetCommand(), wxEXEC_ASYNC, data.pProcess);
    if(nPID <= 0)
    {
        data.pProcess->SetState( enumGISTaskError );
        if(data.pTrackCancel)
            data.pTrackCancel->PutMessage(_("Error start task!"), -1, enumGISMessageErr);
        //if(data.pCallBack)
        //    data.pCallBack->OnFinish(true, data.pTool);
        return false;
    }

    data.pProcess->OnStart(nPID);
    m_nRunningTasks++;
    return true;
}

void wxGISGPToolManager::OnFinish(IProcess* pProcess, bool bHasErrors)
{
    size_t nIndex;
    for(nIndex = 0; nIndex < m_ProcessArray.size(); nIndex++)
        if(pProcess == m_ProcessArray[nIndex].pProcess)
            break;
    if(m_ProcessArray[nIndex].pCallBack)
        m_ProcessArray[nIndex].pCallBack->OnFinish(bHasErrors, m_ProcessArray[nIndex].pTool);

    wxDateTime end = wxDateTime::Now();
    if(m_ProcessArray[nIndex].pTrackCancel)
    {
        if(!bHasErrors)
        {
            m_ProcessArray[nIndex].pTrackCancel->PutMessage(wxString::Format(_("Executed (%s) successfully"), m_ProcessArray[nIndex].pTool->GetName().c_str()), -1, enumGISMessageInfo);
            m_ProcessArray[nIndex].pTrackCancel->PutMessage(_("Done"), -1, enumGISMessageTitle);
        }
        else
        {
            m_ProcessArray[nIndex].pTrackCancel->PutMessage(wxString::Format(_("An error occured while executing %s. Failed to execute (%s)."), m_ProcessArray[nIndex].pTool->GetName().c_str(), m_ProcessArray[nIndex].pTool->GetName().c_str()), -1, enumGISMessageErr);
            m_ProcessArray[nIndex].pTrackCancel->PutMessage(_("Error!"), -1, enumGISMessageTitle);
        }
        
        wxTimeSpan span = end - m_ProcessArray[nIndex].pProcess->GetStart();
        m_ProcessArray[nIndex].pTrackCancel->PutMessage(wxString::Format(_("End Time: %s (Elapsed Time: %s)"), end.Format().c_str(), span.Format(_("%H hours %M min. %S sec.")).c_str()), -1, enumGISMessageInfo);
    }

    //remove from array
    //m_ProcessArray.erase(m_ProcessArray.begin() + nIndex);
    //reduce counter of runnig threads
    m_nRunningTasks--;

    //run not running tasks
    if(m_nRunningTasks < m_nMaxTasks)
    {
		nIndex = GetPriorityTaskIndex();
		if(nIndex >= 0 && nIndex < m_ProcessArray.size())
			ExecTask(m_ProcessArray[nIndex]);
    }
}

size_t wxGISGPToolManager::GetToolCount()
{
    return m_ToolsMap.size();
}

wxString wxGISGPToolManager::GetPopularTool(size_t nIndex)
{
    std::multimap<int, wxString>::iterator i = m_ToolsPopularMap.begin();
    std::advance(i, nIndex);
    if(i != m_ToolsPopularMap.end())
        return i->second;
    return wxEmptyString;
}

//WXGISEXECDDATA wxGISGPToolManager::GetTask(size_t nIndex)
//{
//    return m_ProcessArray[nIndex];
//}

wxGISEnumTaskStateType wxGISGPToolManager::GetProcessState(size_t nIndex)
{
	wxASSERT(nIndex >= 0);
	wxASSERT(nIndex < m_ProcessArray.size());
	return m_ProcessArray[nIndex].pProcess->GetState();
}

void wxGISGPToolManager::StartProcess(size_t nIndex)
{
	wxASSERT(nIndex >= 0);
	wxASSERT(nIndex < m_ProcessArray.size());
    if(m_nRunningTasks < m_nMaxTasks)
        ExecTask(m_ProcessArray[nIndex]);
	else
		m_ProcessArray[nIndex].pProcess->SetState(enumGISTaskQuered);
}

void wxGISGPToolManager::CancelProcess(size_t nIndex)
{
	wxASSERT(nIndex >= 0);
	wxASSERT(nIndex < m_ProcessArray.size());
	m_ProcessArray[nIndex].pProcess->OnCancel();
}

wxDateTime wxGISGPToolManager::GetProcessStart(size_t nIndex)
{
	wxASSERT(nIndex >= 0);
	wxASSERT(nIndex < m_ProcessArray.size());
	return m_ProcessArray[nIndex].pProcess->GetStart();
}

wxDateTime wxGISGPToolManager::GetProcessFinish(size_t nIndex)
{
	wxASSERT(nIndex >= 0);
	wxASSERT(nIndex < m_ProcessArray.size());
	return m_ProcessArray[nIndex].pProcess->GetFinish();
}

int wxGISGPToolManager::GetProcessPriority(size_t nIndex)
{
    for(size_t i = 0; i < m_aPriorityArray.size(); i++)
        if(m_aPriorityArray[i].nIndex == nIndex)
            return m_aPriorityArray[i].nPriority;
	return m_ProcessArray.size();
}

void wxGISGPToolManager::SetProcessPriority(size_t nIndex, int nPriority)
{
	wxASSERT(nIndex >= 0);
	wxASSERT(nIndex < m_ProcessArray.size());

    for(size_t i = 0; i < m_aPriorityArray.size(); i++)
    {
        if(m_aPriorityArray[i].nIndex == nIndex)
        {
            m_aPriorityArray.erase(m_aPriorityArray.begin() + i);
            return AddPriority(nIndex, nPriority);
        }
    }
}

int wxGISGPToolManager::GetPriorityTaskIndex()
{
	int nPriorityIndex = m_ProcessArray.size(); 
    for(size_t nIndex = 0; nIndex < m_aPriorityArray.size(); nIndex++)
    {
        if(m_ProcessArray[m_aPriorityArray[nIndex].nIndex].pProcess->GetState() == enumGISTaskQuered)
        {
            nPriorityIndex = nIndex;
            break;
        }
    }
	return nPriorityIndex;
}

void wxGISGPToolManager::AddPriority(int nIndex, int nPriority)
{
    TASKPRIOINFO info = {nIndex, nPriority};
    bool bIncrease = false;
	for(size_t j = 0; j < m_aPriorityArray.size(); j++)
	{
		if(m_aPriorityArray[j].nPriority >= nPriority)
        {
            m_aPriorityArray.insert(m_aPriorityArray.begin() + j, info);
            bIncrease = true;
            continue;
        }
        if(bIncrease)
        {
            if(m_aPriorityArray[j].nPriority - m_aPriorityArray[j - 1].nPriority < 2)
                m_aPriorityArray[j].nPriority++;
            else
                return;;
        }
	}
    if(!bIncrease)
        m_aPriorityArray.push_back(info);
}

IGPTool* wxGISGPToolManager::GetProcessTool(size_t nIndex)
{
	wxASSERT(nIndex >= 0);
	wxASSERT(nIndex < m_ProcessArray.size());
	return m_ProcessArray[nIndex].pTool;
}
