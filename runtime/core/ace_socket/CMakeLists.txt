
add_library(ace_socket STATIC
  participant_acceptor.cc
  participant.cc
  protocol_hub.cc
  recorder.cc
  first_time_connect.cc
  on_tcp_ready.cc
  on_pcm_data.cc
  on_websocket.cc
  on_http_request.cc
  group.cc
  base64.cc
  wait_end_thread.cc
)

target_link_libraries(ace_socket PUBLIC decoder ACE crypto)
