#ifndef UI_CALL_FRIENDS_HPP
#define UI_CALL_FRIENDS_HPP

#include "UIFriendListBase.h"

namespace DuiLib
{

class CCallFriendsUI : public DuiLib::CFriendListBaseUI
{
public:
	CCallFriendsUI(CPaintManagerUI& paint_manager);
	~CCallFriendsUI();
	Node* AddNode(const FriendListItemInfo& item, Node* parent = NULL);
	
};

} // DuiLib

#endif // UIFRIENDS_HPP