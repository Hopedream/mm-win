#pragma once
#include "../Common/UserInfoDef.h"
#include <set>
using namespace std;

typedef  map<tstring, tstring> t_mapStr2Str;
typedef  map<tstring, tstring>::iterator t_mapStr2StrIter;
typedef  pair<tstring, tstring> t_strstrPair;  
class CSortHelper
{
public:
	enum eFoundField
	{
		eFoundAccount,
		eFoundGroup,
	};
	CSortHelper(tstring _field, eFoundField _fieldType)		{m_strFoundField = _field;	m_eFieldType = _fieldType;}
	bool operator()(sFriendListItem item)
	{
		if (eFoundAccount == m_eFieldType) return item.strAccount.compare(m_strFoundField) == 0;
		if (eFoundGroup == m_eFieldType) return item.strOrgID.compare(m_strFoundField) == 0;
	}		
	bool operator()(const  pair<tstring, tstring>& lhs, const  pair<tstring, tstring>& rhs);
private:	
	tstring m_strFoundField;
	eFoundField m_eFieldType;	
public:
	static bool CompareFuncIntraGroup(const sFriendListItem left, const sFriendListItem right);
	static bool CompareFunc( const sFriendListItem left, const sFriendListItem  right);
	static bool CompareGroupFunc(const t_strstrPair& lhs, const t_strstrPair& rhs);
	CRITICAL_SECTION m_cs;
	template<class _InIt,
	class _Pr> /*inline*/
	static	int FindIf(_InIt _First, _InIt _Last, _InIt& _FirstFound, _InIt& _LastFound, _Pr _Pred)
	{	// find first satisfying _Pred
		bool bFoundFirst = false;
		_FirstFound = _Last;
		_LastFound = _Last;
		int count = 0;
		for (; _First != _Last; ++_First)
		{
			if (_Pred(*_First))
			{
				++count;
				if (!bFoundFirst)
				{
					_FirstFound = _First;
					bFoundFirst = true;
				}
			}

			if (bFoundFirst /*&& !bFoundLast */&& !_Pred(*_First))
			{
				_LastFound = _First;
				break;
			}
		}

		return count;
	}
};


class CDelaySortMrg{
public:
	static CDelaySortMrg* Instance();
	void AddRefreshGroup(const tstring strOrgId, const tstring strGroup);
	void CopyOrgId2GroupMap(t_mapStr2Str& _map); 
	bool WaitDelaySort();
private:
	CDelaySortMrg();
	~CDelaySortMrg();
	t_mapStr2Str m_mapOrgId2Group;
	static CDelaySortMrg* m_pInstance;
	CRITICAL_SECTION m_cs;
};

