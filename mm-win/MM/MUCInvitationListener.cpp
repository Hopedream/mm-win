#include "MUCInvitationListener.h"

using namespace gloox;

CMUCInvitationListener::CMUCInvitationListener(gloox::ClientBase* parent)
	:gloox::MUCInvitationHandler(parent)
{

}

CMUCInvitationListener::~CMUCInvitationListener()
{

}

void CMUCInvitationListener::handleMUCInvitation( const JID& room, const JID& from, const std::tstring& reason,
	const std::tstring& body, const std::tstring& password,
	bool cont, const std::tstring& thread )
{

}