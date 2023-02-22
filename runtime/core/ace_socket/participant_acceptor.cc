#include "ace_socket/participant_acceptor.h"
#include "ace_socket/participant.h"

namespace wenet{

ParticipantAcceptor::~ParticipantAcceptor(){
    handle_close(ACE_INVALID_HANDLE, 0);
}

int ParticipantAcceptor::open (const ACE_INET_Addr &listen_addr)
{
    if (this->acceptor_.open (listen_addr, 1) == -1)
    {
        printf("open port fail\n");
        return -1;
    }
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("register accept event handler...\n")));
    return this->reactor ()->register_handler(this, ACE_Event_Handler::ACCEPT_MASK);
}

int ParticipantAcceptor::handle_input (ACE_HANDLE fd )
{
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("ParticipantAcceptor::handle_input() be called.\n")));
    Participant *client = new Participant(feature_config_, decode_config_, decode_resource_);

    if (this->acceptor_.accept (client->socket ()) == -1)
    {
        printf("accept client fail\n");
        return -1;
    }
    //p.release ();
    client->reactor (this->reactor ());
    if (client->open () == -1)
    {
        client->handle_close (ACE_INVALID_HANDLE, 0);
    }

    return 0;
}


int ParticipantAcceptor::handle_close(ACE_HANDLE handle, ACE_Reactor_Mask m){
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("ParticipantAcceptor::handle_close()被调用..\n")));
    if(acceptor_.get_handle() != ACE_INVALID_HANDLE){
        ACE_Reactor_Mask mask = ACE_Event_Handler::ACCEPT_MASK | ACE_Event_Handler::DONT_CALL;

        reactor()->remove_handler(this, mask);
        acceptor_.close();
    }
    return 0;

}

} // namespace wenet
