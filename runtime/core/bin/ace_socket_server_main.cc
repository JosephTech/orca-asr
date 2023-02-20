#include<thread>
#include<ace/Reactor.h>
#include<ace/Log_Msg.h>

#include "decoder/params.h"
// #include "utils/log.h"
#include "ace_socket/participant_acceptor.h"
#include "ace_socket/wait_end_thread.h"

DEFINE_int32(port, 10010, "socket listening port");

int main(int argc, char *argv[]) 
{
    gflags::ParseCommandLineFlags(&argc, &argv, false);
    google::InitGoogleLogging(argv[0]);

    ACE_INET_Addr addr(FLAGS_port);

    auto decode_config = wenet::InitDecodeOptionsFromFlags();
    auto feature_config = wenet::InitFeaturePipelineConfigFromFlags();    
    auto decode_resource = wenet::InitDecodeResourceFromFlags();

    std::thread waiter(&wenet::WaitEndThread::Update, &wenet::WaitEndThread::Instance());

    wenet::ParticipantAcceptor socket_server(feature_config, decode_config, decode_resource);
    // socket_server.pass_configs(feature_config, decode_config, decode_resource);

    socket_server.reactor(ACE_Reactor::instance());
    if(-1 == socket_server.open(addr))
    {
        ACE_DEBUG((LM_ERROR, ACE_TEXT("ERROR: The port is occupied.\n")));
    }

    
    ACE_DEBUG((LM_INFO, ACE_TEXT("INFO: run reactor event loop.\n")));
    ACE_DEBUG((LM_INFO, ACE_TEXT("INFO: Listening at port %d.\n"), FLAGS_port));
    ACE_Reactor::instance()->run_reactor_event_loop();
    waiter.join();

    return 0; 
} 
 