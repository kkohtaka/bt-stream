#include "stream_client.h"

StreamClient::StreamClient(
    uv_loop_t *loop,
    Stream &stream
) :
    Client(loop),
    stream_(stream),
    running_(true),
    fragment_sequence_(0) {
}

StreamClient::~StreamClient(void) {
}

void StreamClient::run(void) {

  // [TODO] stream.postEvent(CLIENT_START);

  auto header = stream_.header();
}

