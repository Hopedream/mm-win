#pragma once

#include "..\gloox-1.0.14\src\mucroomhandler.h"

using namespace gloox;
namespace std{
#ifdef _UNICODE
	typedef wstring tstring;
#else
	typedef string tstring;
#endif
}
class CMUCRoomProcessor : public MUCRoomHandler
{
public:
	CMUCRoomProcessor();
	virtual ~CMUCRoomProcessor();

    virtual void handleMUCParticipantPresence( MUCRoom* room, const MUCRoomParticipant participant,
        const Presence& presence );

    virtual void handleMUCMessage( MUCRoom* room, const Message& msg, bool priv );
      
    virtual bool handleMUCRoomCreation( MUCRoom* room );

    virtual void handleMUCSubject( MUCRoom* room, const std::tstring& nick,
                                    const std::tstring& subject );

    virtual void handleMUCInviteDecline( MUCRoom* room, const JID& invitee,
                                        const std::tstring& reason );

    virtual void handleMUCError( MUCRoom* room, StanzaError error );

    virtual void handleMUCInfo( MUCRoom* room, int features, const std::tstring& name,
                                const DataForm* infoForm );

    virtual void handleMUCItems( MUCRoom* room, const Disco::ItemList& items );

	virtual void HandleRoomList(MUCRoom* room, const Disco::ItemList& items);

	virtual void HandleRoomMemberList(MUCRoom* room, const Disco::ItemList& items);

};