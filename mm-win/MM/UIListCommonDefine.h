#ifndef UILISTCOMMONDEFINE_HPP
#define UILISTCOMMONDEFINE_HPP

#include <math.h>
#include "UIDefine.h"

namespace DuiLib
{

struct NodeData
{
	int level_;
	bool folder_;
	bool child_visible_;
	bool has_child_;
	UINT value;
	tstring text_;
	tstring strOrgID;//
	
	tstring strPackIcon;
	tstring strPackID;
	tstring strPackName;
	tstring strPackTime;
	tstring strPackSender;
	tstring strPackCreaterPhone;
	tstring strPackSize;
	tstring strPackProp;
	int	   nPackStatus;
	//tstring strPackStatus;

	tstring strFileIcon;
	tstring strFileName;
	tstring strFileSizeShow;
	tstring strFileSizeAccurate;
	tstring strAliUrl;
	tstring strFileSize;
	tstring strFileList;
	vector<fileInfo> m_vecFiles;
	CListContainerElementUI* list_elment_;
};

double CalculateDelay(double state);

class Node
{
public:
	Node();
	explicit Node(NodeData t);
	Node(NodeData t, Node* parent);
	~Node();
	NodeData& data();
	int num_children() const;
	Node* child(int i);
	Node* parent();
	bool folder() const;
	bool has_children() const;
	void add_child(Node* child);
	void remove_child(Node* child);
	Node* get_last_child();
	void removeAllChild();

private:
	void set_parent(Node* parent);
private:
	typedef std::vector <Node*>	Children;

	Children	children_;
	Node*		parent_;

	NodeData    data_;
};

} // DuiLib

#endif // UILISTCOMMONDEFINE_HPP