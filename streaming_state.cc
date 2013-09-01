#include "streaming_state.h"

#include "ebml.h"
#include <iostream>

StreamingState::StreamingState(
    std::shared_ptr<StreamInput> input,
    std::shared_ptr<Stream> stream,
    int video_track_number
) :
    StreamInputState(),
    input_(input),
    stream_(stream),
    video_track_number_(video_track_number),
    cluster_time_code_(0),
    fragment_(new MovieFragment()) {
}

StreamingState::~StreamingState(void) {
}

int StreamingState::process_data(
    char *buffer,
    unsigned int offset,
    unsigned int length
) {

  const unsigned int end_offset = offset + length;

  while (offset < end_offset - 12) {

    EBML ebml(buffer, offset, length);

    /* Note: cluster check was moved to be the first because of the
     * possibility of infinite clusters (gstreamer's curlsink?).
     */
    if (ebml.id() != ID_CLUSTER && static_cast<unsigned int>(ebml.end_offset()) > end_offset) {

      /* The element is not fully loaded: we need more data, so we end
       * this processing cycle. The StreamInput will fill the buffer
       * and take care of the yet unprocessed element. We signal this
       * by sending the element's offset back. The StreamInput will
       * keep the data beyound this offset and the next processing cycle
       * will begin with this element.
       */
      return ebml.element_offset();
    }

    /* Timecode for this cluster. We use a flat processing model (we do
     * not care about the nesting of elements), so this timecode will
     * live until we get the next one. It will be the Timecode element
     * of the next Cluster (according to standard).
     */
    switch (ebml.id()) {

    case ID_TIMECODE: {

      // we have the timecode, so open a new cluster in our movie fragment
      cluster_time_code_ = EBML::load_unsigned(buffer, ebml.data_offset(), ebml.data_size());

      // cluster opened
      fragment_.get()->open_cluster(cluster_time_code_);

      break;
    }
    case ID_SIMPLEBLOCK: {

      int track_num = buffer[ebml.data_offset()] & 0xff;
      if ((track_num & 0x80) == 0) {
        std::cerr << "Track numbers > 127 are not implemented." << std::endl;
      }
      track_num ^= 0x80;

      int video_key_offset = -1;

      if (track_num == video_track_number_) {

        int flags = buffer[ebml.data_offset() + 3] & 0xff;
        if ((flags & 0x80) != 0) {

          if (fragment_.get()->data_length() >= MINIMAL_FRAGMENT_LENGTH) {

            fragment_.get()->close_cluster();
            stream_.get()->push_fragment(fragment_);

            fragment_ = std::shared_ptr<MovieFragment>(new MovieFragment());
            fragment_.get()->open_cluster(cluster_time_code_);

            // [TODO] stream_.post_event(new ServerEvent(input, stream, ServerEvent.INPUT_FRAGMENT_START));

            if ((flags & 0x60) == 0) {
              video_key_offset = ebml.data_offset() + 4;
            } else {
              std::cerr << "Lacyng is not supported." << std::endl;
            }
          }
        }
      }

      fragment_.get()->append_key_block(
          buffer,
          ebml.element_offset(),
          ebml.element_size(),
          video_key_offset
      );

      break;
    }
    case ID_BLOCKGROUP: {

      fragment_.get()->append_block(
          buffer,
          ebml.element_offset(),
          ebml.element_size()
      );

      break;
    }
    default:

      break;
    }

    if (ebml.id() == ID_CLUSTER || ebml.data_size() >= 0x100000000L) {
      offset = ebml.data_offset();
    } else {
      offset = ebml.end_offset();
    }
  }

  return offset;
}
