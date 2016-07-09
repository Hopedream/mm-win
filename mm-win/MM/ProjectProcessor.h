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

	//���strTimeΪ�գ���ʾȡȫ����Ŀ��
	bool GetProject(const std::tstring& strTime, std::vector<sProjectItem>& vecProjs,const std::tstring& strSearch=(_T("")));

	bool GetProjectsByOrgId(const std::tstring& strOrgId, std::vector<sProjectItem>& vecProjs);

	bool GetProjectsByCellPhone(const std::tstring& strCellPhone, std::vector<sProjectItem>& vecProjs);

protected:
	//��ȡû����ȡ��room��Ա�ĳ�Ա�б�eg�����ڵ�½�����б�����Ⱥ�ˣ����ʱ���ȡ��room�б�͵�½��ʼ��room�б�����ǲ�ȫ�ġ�
	void GetRoomMemList(std::vector<sProjectItem>& vecProjs);
protected:

	map<tstring, sProjectItem> m_mapID2Projects;
};