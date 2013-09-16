// Copyright (c) 2013 Kazumasa Kohtaka. All rights reserved.
// This file is available under the MIT license.

#include "./stream_client.h"

#include <cstdio>

#include "./stream.h"

const char StreamClient::http_header[] =
    "HTTP/1.1 200 OK\nContent-type: video/webm\nServer: bt-stream\n\n";

void StreamClient::write_header_static(::uv_timer_t *timer, int status) {
  StreamClient *self = reinterpret_cast<StreamClient *>(timer->data);
  self->write_header();
}

void StreamClient::write_fragment_static(::uv_timer_t *timer, int status) {
  StreamClient *self = reinterpret_cast<StreamClient *>(timer->data);
  self->write_fragment();
}

void StreamClient::close_static(::uv_timer_t *timer, int status) {
  StreamClient *self = reinterpret_cast<StreamClient *>(timer->data);
  self->close(
      [] (uv_handle_t *handle) {
        StreamClient *self = reinterpret_cast<StreamClient *>(handle->data);
        self->on_close();
      });
}

StreamClient::StreamClient(
    uv_loop_t *loop,
    std::shared_ptr<Stream> stream)
  : Client(loop),
    timer_(),
    stream_(stream),
    running_(true),
    fragment_sequence_(0),
    age_(0),
    stream_age_(0),
    fragment_offset_(0),
    fragment_length_(0),
    write_req_(nullptr) {
  ::uv_timer_init(loop, &timer_);

  timer_.data = this;
}

StreamClient::~StreamClient(void) {
  if (write_req_) {
    ::uv_cancel(reinterpret_cast< ::uv_req_t *>(write_req_));
  }

  ::uv_timer_stop(&timer_);

  std::printf("StreamClient deleted. %p\n", this);
}

void StreamClient::write_header(void) {
#if DEBUG_CONSUMER
  std::printf("StreamClient::write_header() %p\n", this);
#endif

  auto header_length = stream_.get()->header_length();

  if (header_length == 0) {
    // Wait for a header prepared.

    ::uv_timer_start(&timer_, write_header_static, 200, 0);

  } else {
    // Write the header on a stream.

    auto header = stream_.get()->header();

    write_req_ = new ::uv_write_t();
    ::uv_buf_t buf = ::uv_buf_init(header.get(), header_length);

    write_req_->data = this;

#if DEBUG_CONSUMER
    std::printf("===== HEADER (%d): ", header_length);
    for (unsigned int i = 0; i < header_length; ++i) {
      std::printf("%d ", static_cast<int>(*(header.get() + i)));
    }
    std::printf("=====\n");
#endif

    write(write_req_, &buf, 1, [] (::uv_write_t *req, int status) {
      StreamClient *client = reinterpret_cast<StreamClient *>(req->data);

      if (status == 0) {
#if DEBUG_CONSUMER
        std::cout << "StreamClient::write_header() -> WRITE OK" << std::endl;
#endif

        // Start writing fragments.

        ::uv_timer_start(&client->timer_, write_fragment_static, 1000, 0);

      } else {
#if DEBUG_CONSUMER
        std::cout << "StreamClient::write_header() -> WRITE NG" << std::endl;
#endif

        // Close the output stream.

        ::uv_timer_start(&client->timer_, close_static, 0, 0);
      }

      delete req;
      client->write_req_ = nullptr;
    });
  }
}

void StreamClient::write_fragment(void) {
#if DEBUG_CONSUMER
  std::printf("StreamClient::write_fragment() %p\n", this);
#endif

  if (!running_) {
    // Since the output stream was closed, do nothing.

  } else if (!stream_.get()->is_running()) {
    // Since an input stream was closed, close the output stream.

    ::uv_timer_start(&timer_, close_static, 0, 0);

  } else {
    int stream_age = stream_.get()->fragment_age();
    if (age_ >= stream_age) {
      // Wait for a next fragment prepared.

      ::uv_timer_start(&timer_, write_fragment_static, 200, 0);

    } else {
      if (age_ > 0 && stream_age > age_ + 1) {
        // [TODO] stream.postEvent(ServerEvent.CLIET_FRAGMENT_SKIP);
      }

      auto fragment = stream_.get()->fragment();

      const unsigned int PACKET_SIZE = 24 * 1024;
      unsigned int fragment_length = fragment.get()->data_length();

      if (fragment_offset_ >= fragment_length) {
        // Wait and process Next Fragment.

        age_ = stream_age;
        fragment_offset_ = 0;

        ::uv_timer_start(&timer_, write_fragment_static, 200, 0);

      } else {
        // Process Current Fragment.

        fragment_length_ = fragment_length - fragment_offset_;
        if (fragment_length_ >= PACKET_SIZE + (PACKET_SIZE >> 1)) {
          fragment_length_ = PACKET_SIZE;
        }

        write_req_ = new ::uv_write_t();
        ::uv_buf_t buf = ::uv_buf_init(
            fragment.get()->data().get() + fragment_offset_,
            fragment_length_);

        write_req_->data = this;

#if DEBUG_CONSUMER
        std::printf(
            "===== FRAGMENT (%d, %d): ",
            fragment_offset_,
            fragment_length_);
        for (unsigned int i = 0; i < 128 && i < fragment_length_; ++i) {
          std::printf(
              "%d ",
              static_cast<int>(
                  *(fragment.get()->data().get() + fragment_offset_ + i)));
        }
        std::printf("\n");
#endif

        write(write_req_, &buf, 1, [] (::uv_write_t *req, int status) {
          StreamClient *client = reinterpret_cast<StreamClient *>(req->data);

          if (status == 0) {
#if DEBUG_CONSUMER
            std::printf("StreamClient::write_fragment() -> WRITE OK\n");
#endif

            // [TODO] stream.postEvent(TransferEvent.STREAM_OUTPUT);

            client->fragment_offset_ += client->fragment_length_;
            client->write_fragment();

          } else {
#if DEBUG_CONSUMER
            std::printf("StreamClient::write_fragment() -> WRITE NG\n");
#endif

            // Close the output stream.

            ::uv_timer_start(&client->timer_, close_static, 0, 0);
          }

          delete req;
          client->write_req_ = nullptr;
        });
      }
    }
  }
}

void StreamClient::run(void) {
#if DEBUG_CONSUMER
  std::printf("StreamClient::run() %p\n", this);
#endif

  write_req_ = new ::uv_write_t();

  ::uv_buf_t buf =
      ::uv_buf_init(
          const_cast<char *>(http_header),
          sizeof(http_header) - 1);

  write_req_->data = this;

  write(write_req_, &buf, 1, [] (::uv_write_t *req, int status) {
    StreamClient *client = reinterpret_cast<StreamClient *>(req->data);

    if (status == 0) {
#if DEBUG_CONSUMER
      std::cout << "StreamClient::run() -> WRITE OK" << std::endl;
#endif

      // [TODO] stream.postEvent(CLIENT_START);

      // Wait for a header prepared.

      ::uv_timer_start(&client->timer_, write_header_static, 200, 0);

    } else {
#if DEBUG_CONSUMER
      std::cout << "StreamClient::run() -> WRITE NG" << std::endl;
#endif

      // Close the output stream.

      ::uv_timer_start(&client->timer_, close_static, 0, 0);
    }

    delete req;
    client->write_req_ = nullptr;
  });
}

void StreamClient::on_close(void) {
#if DEBUG_CONSUMER
  std::printf("StreamClient::on_close() %p\n", this);
#endif

  // [TODO] stream.postEvent(ServerEvent.CLIET_STOP);

  std::printf("===== CONNECTION WAS CLOSED. =====\n");

  running_ = false;
}
