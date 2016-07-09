#include "MUCRoomProcessor.h"

CMUCRoomProcessor::CMUCRoomProcessor()
{

}

CMUCRoomProcessor::~CMUCRoomProcessor()
{

}

void CMUCRoomProcessor::handleMUCParticipantPresence( MUCRoom* room, const MUCRoomParticipant participant,
	const Presence& presence )
{

}

void CMUCRoomProcessor::handleMUCMessage( MUCRoom* room, const Message& msg, bool priv )
{

}
bool CMUCRoomProcessor::handleMUCRoomCreation( MUCRoom* room )
{
	return true;
}
void CMUCRoomProcessor::handleMUCSubject( MUCRoom* room, const std::tstring& nick,
	const std::tstring& subject )
{

}

void CMUCRoomProcessor::handleMUCInviteDecline( MUCRoom* room, const JID& invitee,
	const std::tstring& reason )
{

}

void CMUCRoomProcessor::handleMUCError( MUCRoom* room, StanzaError error )
{

}

void CMUCRoomProcessor::handleMUCInfo( MUCRoom* room, int features, const std::tstring& name,
	const DataForm* infoForm )
{

}
void CMUCRoomProcessor::handleMUCItems( MUCRoom* room, const Disco::ItemList& items )
{

}

void CMUCRoomProcessor::HandleRoomList(MUCRoom* room, const Disco::ItemList& items)
{

}
void CMUCRoomProcessor::HandleRoomMemberList(MUCRoom* room, const Disco::ItemList& items)
{

}