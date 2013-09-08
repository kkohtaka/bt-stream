#include "stream_input.h"

#include "header_detection_state.h"
#include <sstream>
#include <iostream>
#include <cstring>
#include <algorithm>

static int on_headers_complete(http_parser *parser) {

  std::cout << "on_headers_complete" << std::endl;

  //Client *client = static_cast<Client *>(parser->data);

  return 1;
}

StreamInput::StreamInput(
    std::shared_ptr<Stream> stream,
    std::shared_ptr<Client> client
) :
    stream_(stream),
    client_(client),
    running_(false),
    header_parsed_(false),
    header_(),
    header_length_(0),
    chunk_size_(0),
    buffer_() {

  client_.get()->set_data(this);
}

StreamInput::~StreamInput(void) {
  std::cout << "StreamInput deleted." << std::endl;
}

bool StreamInput::is_running(void) {

  return running_;
}

unsigned char *StreamInput::header(void) {

  return header_;
}

unsigned int StreamInput::header_length(void) {

  return header_length_;
}

void StreamInput::set_header(unsigned char *header, unsigned int header_length) {

  header_ = header;
  header_length_ = header_length;
}

void StreamInput::run(void) {

  running_ = true;

  // [TODO] postEvent stream_.publish(Stream::INPUT_START);

  change_state(
      std::shared_ptr<StreamInputState>(
          new HeaderDetectionState(std::shared_ptr<StreamInput>(this), stream_)
      )
  );

  if (running_ && stream_.get()->is_running()) {

    client_.get()->start_reading(
        [] (uv_handle_s *handle, size_t suggested_size) -> uv_buf_t {

          Client *client = reinterpret_cast<Client *>(handle->data);
          StreamInput *self = reinterpret_cast<StreamInput *>(client->data());

          return self->on_alloc(suggested_size);
        },
        [] (uv_stream_t *stream, ssize_t nread, uv_buf_t buf) {

          Client *client = reinterpret_cast<Client *>(stream->data);
          StreamInput *self = reinterpret_cast<StreamInput *>(client->data());

          self->on_read(nread, buf);
        }
    );
  }
}

void StreamInput::change_state(std::shared_ptr<StreamInputState> state) {

  state_ = state;
}

uv_buf_t StreamInput::on_alloc(size_t suggested_size) {

  std::cout << "===== on_alloc: ofset: " << buffer_.offset << "=====" << std::endl;

  return ::uv_buf_init(
      buffer_.buffer.get() + buffer_.offset,
      buffer_.buffer_length - buffer_.offset
  );
}

void StreamInput::on_read(ssize_t nread, uv_buf_t buf) {

  // [TODO] stream.postEvent(new TransferEvent(this, stream, TransferEvent.STREAM_INPUT, numBytes, new Date().getTime() - transferStart));

  std::cout << "===== on_read: nread: " << nread << " =====" << std::endl;

  if (nread < 0) {

    running_ = false;

    client_.get()->close(
        [] (uv_handle_t *handle) {

          Client *client = reinterpret_cast<Client *>(handle->data);
          StreamInput *self = reinterpret_cast<StreamInput *>(client->data());

          self->on_close();
        }
    );

    return;
  }

  buffer_.length += nread;

  if (!header_parsed_) {

    static http_parser_settings parser_settings;
    parser_settings.on_headers_complete = on_headers_complete;
    int parsed = client_.get()->parse_request(
        &parser_settings,
        nread,
        buf
    );
    std::cout << "parsed: " << parsed << std::endl;

    for (unsigned int i = 0; (int)i < parsed; ++i) {
      std::cout << *(buffer_.buffer.get() + i );
    }
    std::cout << std::endl;

    buffer_.length -= parsed;
    ::memmove(
        buffer_.buffer.get(),
        buffer_.buffer.get() + parsed,
        buffer_.length
    );

    header_parsed_ = true;
  }

  while (buffer_.length != 0) {

    if (chunk_size_ == 0) {
      int head_size = refresh_chunk_size();
      std::cout << "head_size: " << head_size << std::endl;
      if (head_size == -1) {
        break;
      }
      //std::cout << "buffer_.length: " << buffer_.length << std::endl;
      //std::cout << "head_size: " << head_size << std::endl;
      buffer_.length -= head_size;
      //std::cout << "buffer_.length: " << buffer_.length << std::endl;
#if 0
      std::cout << "=== BEFORE === ";
      for (unsigned int i = 0; i < 64; ++i) {
        std::cout << (int)*(buffer_.buffer.get() + i) << ' ';
      }
      std::cout << std::endl;
#endif
      ::memmove(
          buffer_.buffer.get(),
          buffer_.buffer.get() + head_size,
          buffer_.length
      );
#if 0
      std::cout << "=== AFTER  === ";
      for (unsigned int i = 0; i < 64; ++i) {
        std::cout << (int)*(buffer_.buffer.get() + i) << ' ';
      }
      std::cout << std::endl;
#endif
    }
    std::cout << "chunk_size: " << chunk_size_ << std::endl;
#if 0
    std::cout << "=== HEAD === ";
    for (unsigned int i = 0; i < 32; ++i) {
      std::cout << (int)*(buffer_.buffer.get() + i) << ' ';
    }
    std::cout << std::endl;

    std::cout << "=== TAIL === ";
    for (unsigned int i = buffer_.length - 32; i < buffer_.length; ++i) {
      std::cout << (int)*(buffer_.buffer.get() + i) << ' ';
    }
    std::cout << std::endl;
#endif

    if (buffer_.length == 0) {
      break;
    }

    std::cout << "buffer_.length: " << buffer_.length << std::endl;
    std::cout << "chunk_size_: " << chunk_size_ << std::endl;
    unsigned int new_offset = state_.get()->process_data(
        buffer_.buffer.get(),
        0,
        std::min<int>(buffer_.length, chunk_size_)
    );

    chunk_size_ -= new_offset;

    std::cout
        << "new_offset: " << new_offset     << ", "
        << "chunk_size: " << chunk_size_    << ", "
        << "offset: "     << buffer_.offset << ", "
        << "length: "     << buffer_.length << std::endl;

    if (new_offset < buffer_.offset + buffer_.length) {

      buffer_.length -= new_offset;
      ::memmove(
          buffer_.buffer.get(),
          buffer_.buffer.get() + new_offset,
          buffer_.length
      );

    } else {

      std::cout << "=== RESET BUFFER ===" << std::endl;
      buffer_.length = 0;
      chunk_size_ = 0;
    }

    std::cout << "offset: " << buffer_.offset << ", length: " << buffer_.length << std::endl;
  }

  buffer_.offset = buffer_.length;
}

void StreamInput::on_close(void) {

  // [TODO] stream.postEvent(new ServerEvent(this, stream, ServerEvent.INPUT_STOP));
  std::cout << "===== CONNECTION WAS CLOSED. =====" << std::endl;
}

int StreamInput::refresh_chunk_size(void) {

  char *buffer = buffer_.buffer.get();
  int length = buffer_.length;
  for (unsigned int i = 0, len = length - 1; i < len; ++i) {
    if (*(buffer + i) == '\r' && *(buffer + i + 1) == '\n') {
      std::stringstream ss;
      std::cout << "=== CHUNK SIZE: ";
      for (unsigned int j = 0; j < i; ++j) {
        ss << *(buffer + j);
        std::cout << *(buffer + j);
      }
      std::cout << " ===" << std::endl;
      ss >> std::hex >> chunk_size_;
      std::cout << "chunk_size: " << chunk_size_ << std::endl;
      return i + 2;
    }
  }
  return -1;
}

