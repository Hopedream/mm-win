#pragma once

#include "..\gloox-1.0.14\src\mucinvitationhandler.h"

using namespace gloox;
namespace std{
#ifdef _UNICODE
	typedef wstring tstring;
#else
	typedef string tstring;
#endif
}
class CMUCInvitationListener : public gloox::MUCInvitationHandler
{
public:
	CMUCInvitationListener(gloox::ClientBase* parent);
	virtual ~CMUCInvitationListener();


	 /**
       * This function is called for incoming invitations to MUC rooms.
       * @param room The JID of the room you're being invited to.
       * @param from The JID of the inviter.
       * @param reason A reason for the invitation.
       * @param body The body of the message. May contain a MUC-service generated invitation message.
       * @param password Optionally, a password for the room.
       * @param cont Indicates whether or not the multi-user chat is a continuation of a private chat.
       * @param thread An optional thread identifier in case this is a
       * continued chat.
       */
      virtual void handleMUCInvitation( const JID& room, const JID& from, const std::tstring& reason,
                                        const std::tstring& body, const std::tstring& password,
                                        bool cont, const std::tstring& thread );

};