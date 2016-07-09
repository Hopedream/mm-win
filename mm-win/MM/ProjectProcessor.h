#pragma once
#include <string>
#include <vector>
#include <map>


using namespace std;
namespace std{
#ifdef _UNICODE
	typedef wstring tstring;
#else
	typedef string tstring;
#endif
}
struct sProjectItem
{
	tstring strProjName;
	tstring strProjID;
	tstring strDestOrgName;
	tstring strDestOrgID;
	tstring strCreateTime;
	tstring strRoomID;
	vector<tstring> vecDestParticipants;
};

class CProjectProcessor
{
public:
	CProjectProcessor();
	~CProjectProcessor();

	//如果strTime为空，表示取全部项目。
	bool GetProject(const std::tstring& strTime, std::vector<sProjectItem>& vecProjs,const std::tstring& strSearch=(_T("")));

	bool GetProjectsByOrgId(const std::tstring& strOrgId, std::vector<sProjectItem>& vecProjs);

	bool GetProjectsByCellPhone(const std::tstring& strCellPhone, std::vector<sProjectItem>& vecProjs);

protected:
	//获取没有拉取的room成员的成员列表：eg：我在登陆过程中被拉进群了，这个时候获取的room列表和登陆初始的room列表相比是不全的。
	void GetRoomMemList(std::vector<sProjectItem>& vecProjs);
protected:

	map<tstring, sProjectItem> m_mapID2Projects;
};