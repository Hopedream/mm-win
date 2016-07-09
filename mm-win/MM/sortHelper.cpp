#include "StdAfx.h"
#include "sortHelper.h"
#include "../Common/regexHelper.h"

bool CSortHelper::CompareFunc( const sFriendListItem left, const sFriendListItem right )
{
		bool bLeftGroupContainChinese = CRegexHelper::IsContainChineseCharacter(left.strOrgnization);
		bool bRightGroupContainChinese = CRegexHelper::IsContainChineseCharacter(right.strOrgnization);

		//sort regular: group > online > nickname, 
		//chinese character < english character.
		if ( bLeftGroupContainChinese != bRightGroupContainChinese)  return bLeftGroupContainChinese > bRightGroupContainChinese;

		tstring strLeftGroup = left.strOrgnization;
		tstring strRightGroup = right.strOrgnization;
		if(bLeftGroupContainChinese)
		{
			CRegexHelper::GenFirstPYLetter(left.strOrgnization, strLeftGroup);
			CRegexHelper::GenFirstPYLetter(right.strOrgnization, strRightGroup);				
		}

		int iGroupComp = strLeftGroup.compare(strRightGroup);
		if (0 == iGroupComp && left.strOrgID != right.strOrgID) return left.strOrgID > right.strOrgID;  //处理不同组织ID对应相同组织名称的情况，否则会导致好友显示混论
		if (0 != iGroupComp) return iGroupComp < 0;

		return CompareFuncIntraGroup(left, right);	
}

bool CSortHelper::CompareFuncIntraGroup( const sFriendListItem left, const sFriendListItem right )
{
	if (left.eState != right.eState) return left.eState < right.eState;

	bool bLeftNameContainChinese = CRegexHelper::IsContainChineseCharacter(left.strShowName);
	bool bRightNameContainChinese = CRegexHelper::IsContainChineseCharacter(right.strShowName);
	if (bLeftNameContainChinese != bRightNameContainChinese) return bLeftNameContainChinese > bRightNameContainChinese;

	tstring strLeftNickname = left.strShowName;
	tstring strRightNickname = right.strShowName;
	if (bLeftNameContainChinese)
	{
		CRegexHelper::GenFirstPYLetter(left.strShowName, strLeftNickname);
		CRegexHelper::GenFirstPYLetter(right.strShowName, strRightNickname);				
	}
	return strLeftNickname.compare(strRightNickname) < 0;
}

bool CSortHelper::CompareGroupFunc( const t_strstrPair& lhs, const t_strstrPair& rhs )
{
	sFriendListItem left;
	sFriendListItem right;
	left.strShowName = lhs.second;
	right.strShowName = rhs.second;
	return CompareFuncIntraGroup(left, right);
}



void CDelaySortMrg::AddRefreshGroup( const tstring strOrgId, const tstring strGroup )
{
	::EnterCriticalSection(&m_cs);
	if (m_mapOrgId2Group.end() == m_mapOrgId2Group.find(strOrgId))
	{
		m_mapOrgId2Group.insert(make_pair(strOrgId, strGroup));
	}	
	::LeaveCriticalSection(&m_cs);
}

CDelaySortMrg* CDelaySortMrg::Instance()
{
	if (NULL == m_pInstance)
	{
		m_pInstance = new CDelaySortMrg;
	}
	return m_pInstance;
}

CDelaySortMrg::CDelaySortMrg()
{
	::InitializeCriticalSection(&m_cs);
}

CDelaySortMrg::~CDelaySortMrg()
{
	::DeleteCriticalSection(&m_cs);
}

void CDelaySortMrg::CopyOrgId2GroupMap( t_mapStr2Str& _map )
{
	::EnterCriticalSection(&m_cs);
	_map.insert(m_mapOrgId2Group.begin(), m_mapOrgId2Group.end());
	m_mapOrgId2Group.clear();
	::LeaveCriticalSection(&m_cs);
}

bool CDelaySortMrg::WaitDelaySort()
{
	::EnterCriticalSection(&m_cs);
	bool bRet = m_mapOrgId2Group.size() > 0;
	::LeaveCriticalSection(&m_cs);
	return bRet;
}

CDelaySortMrg* CDelaySortMrg::m_pInstance = NULL;



