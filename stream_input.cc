#include "stream_input.h"

#include "header_detection_state.h"
#include <sstream>
#include <iostream>
#include <cstring>

StreamInput::StreamInput(
    std::shared_ptr<Stream> stream,
    std::shared_ptr<Client> client
) :
    stream_(stream),
    client_(client),
    running_(false),
    header_(),
    header_length_(0),
    head_size_(-1),
    chunk_size_(0),
    buffer_(),
    timer_() {

  client_.get()->set_data(this);
}

StreamInput::~StreamInput(void) {
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

    ::time(&timer_);

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
  /*
  } else {

    client_.get()->close(
        [] (uv_handle_t *handle) {

          Client *client = reinterpret_cast<Client *>(handle->data);
          StreamInput *self = reinterpret_cast<StreamInput *>(client->data());

          self->on_close();
        }
    );
  */
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

  time_t now;
  time(&now);
  //double elapsed_time = ::difftime(now, timer_);

  //std::cout << "elapsed_time: " << elapsed_time << std::endl;

  // [TODO] stream.postEvent(new TransferEvent(this, stream, TransferEvent.STREAM_INPUT, numBytes, new Date().getTime() - transferStart));

  if (nread < 0) {
    running_ = false;

    client_.get()->close(
        [] (uv_handle_t *handle) {

          Client *client = reinterpret_cast<Client *>(handle->data);
          StreamInput *self = reinterpret_cast<StreamInput *>(client->data());

          self->on_close();
        }
    );
  }
  std::cout << "=== nread: " << nread << " ===" << std::endl;

  if (nread == 6 || nread == 2) {
    return;
  }
  /*
  if (chunk_size_ == 0) {
    if (head_size_ == -1) {
      head_size_ = refresh_chunk_size();
      std::cout << "chunk_size_: " << chunk_size_ << std::endl;
      if (head_size_ == -1) {
        return;
      }
    }
  }
  */

  buffer_.length += nread;

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

  unsigned int new_offset = state_.get()->process_data(
      buffer_.buffer.get(),
      0, //head_size_, // 0,
      buffer_.length
  );

  //std::cout << "new_offset: " << new_offset << ", offset: " << buffer_.offset << ", length: " << buffer_.length << std::endl;
  if (new_offset < buffer_.offset + buffer_.length) {
    buffer_.length -= new_offset;
#if 0
    std::cout << "=== before memmove === ";
    for (unsigned int i = 0; i < 32; ++i) {
      std::cout << (int)*(buffer_.buffer.get() + i) << ' ';
    }
    std::cout << std::endl;
#endif
    ::memmove(
        buffer_.buffer.get(),
        buffer_.buffer.get() + new_offset,
        buffer_.length
    );
#if 0
    std::cout << "=== after memmove  === ";
    for (unsigned int i = 0; i < 32; ++i) {
      std::cout << (int)*(buffer_.buffer.get() + i) << ' ';
    }
    std::cout << std::endl;
#endif
    buffer_.offset = buffer_.length;
  } else {
    buffer_.length = 0;
    buffer_.offset = 0;

    chunk_size_ = 0;
    head_size_ = -1;
  }
  //std::cout << "offset: " << buffer_.offset << ", length: " << buffer_.length << std::endl;
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
      for (unsigned int j = 0; j < i; ++j) {
        ss << *(buffer + j);
      }
      ss >> std::hex >> chunk_size_;
      return i + 2;
    }
  }
  return -1;
}

