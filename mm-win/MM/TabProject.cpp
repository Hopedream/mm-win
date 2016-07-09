#include "StdAfx.h"
#include "TabProject.h"
#include "CDialogBuilderCallbackEx.h"

CTabProject::CTabProject(CPaintManagerUI* ppm)
{
	CDialogBuilder builder;
	CContainerUI* pTabProject = static_cast<CContainerUI*>(builder.Create(_T("tab_project.xml"), 0, NULL, ppm));
	if (NULL != pTabProject)
	{
		this->Add(pTabProject);
	}
	else
	{
		this->RemoveAll();
		return;
	}
}