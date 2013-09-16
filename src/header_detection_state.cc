// Copyright (c) 2013 Kazumasa Kohtaka. All rights reserved.
// This file is available under the MIT license.

#include "./header_detection_state.h"

#include <memory>
#include <cstring>

#include "./ebml.h"
#include "./streaming_state.h"

const unsigned char HeaderDetectionState::infiniteSegment[] = {
  0x18, 0x53, 0x80, 0x67, 0x01, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
};

HeaderDetectionState::HeaderDetectionState(
    std::shared_ptr<StreamInput> input,
    std::shared_ptr<Stream> stream)
  : input_(input),
    stream_(stream) {
}

HeaderDetectionState::~HeaderDetectionState(void) {
  std::printf("HeaderDetectionState deleted.\n");
}

int HeaderDetectionState::process_data(
    char *buffer,
    unsigned int offset,
    unsigned int length
) {
  std::printf("process_data: offset: %d, length: %d\n", offset, length);
  for (unsigned int i = 0; i < 32 && i < length; ++i) {
    std::printf("%d ", static_cast<int>(*(buffer + i)));
  }
  std::printf("\n");

  const unsigned int
      start_offset = offset,
      end_offset = offset + length;

  std::printf("=== start_offset: %d, end_offset: %d ===\n",
      start_offset, end_offset);

  std::shared_ptr<unsigned char> header_buffer(
      new unsigned char[65536],
      [] (unsigned char *p) { delete [] p; });
  uint64_t header_length = 0;

  EBML ebml(buffer, offset, length);

  if (ebml.id() != ID_EBML) {
    fprintf(stderr, "First element is not EBML!\n");
    throw std::exception();
  }

  // Copy EBML header buffer.
  ::memcpy(
      header_buffer.get() + header_length,
      buffer + ebml.element_offset(),
      ebml.element_size());
  header_length += ebml.element_size();

  offset = ebml.end_offset();

  // Copy an infinite segment.
  ::memcpy(
      header_buffer.get() + header_length,
      &infiniteSegment[0],
      sizeof(infiniteSegment));
  header_length += sizeof(infiniteSegment);

  // Search an Segment.
  do {
    ebml = EBML(buffer, offset, length);
    if (ebml.id() == ID_SEGMENT) {
      break;
    }
    offset = ebml.end_offset();
  } while (offset < end_offset);

  // Not found.
  if (offset >= end_offset) {
    return start_offset;
  }

  std::printf("Segment found.\n");

  int segment_data_offset = ebml.data_offset();

  // Search an Info.
  offset = segment_data_offset;
  do {
    ebml = EBML(buffer, offset, length);
    offset = ebml.end_offset();
  } while (offset < end_offset && ebml.id() != ID_INFO);

  // Not found.
  if (offset >= end_offset) {
    return start_offset;
  }

  std::printf("Info found.\n");

  // Copy Info header buffer.
  ::memcpy(
      header_buffer.get() + header_length,
      buffer + ebml.element_offset(),
      ebml.element_size());
  header_length += ebml.element_size();

  // Search Tracks.
  offset = segment_data_offset;
  do {
    ebml = EBML(buffer, offset, length);
    offset = ebml.end_offset();
  } while (offset < end_offset && ebml.id() != ID_TRACKS);

  // Not found.
  if (offset >= end_offset) {
    std::printf("Tracks not found.\n");
    return start_offset;
  }

  std::printf("Tracks found.\n");

  // Copy Tracks header buffer.
  ::memcpy(
      header_buffer.get() + header_length,
      buffer + ebml.element_offset(),
      ebml.element_size());
  header_length += ebml.element_size();

  // Search a video track's id.
  int video_track_number = 0;
  unsigned int end_of_tracks = ebml.end_offset();
  offset = ebml.data_offset();
  while (offset < end_of_tracks) {
    EBML track(buffer, offset, end_of_tracks - offset);
    offset = track.data_offset();
    unsigned int end_of_track = track.end_offset();

    int track_type = 0;
    int track_number = 0;
    while (offset < end_of_track) {
      EBML property(buffer, offset, end_of_track - offset);
      if (property.id() == ID_TRACKTYPE) {
        track_type = buffer[property.data_offset()] & 0xff;
      } else if (property.id() == ID_TRACKNUMBER) {
        track_number = EBML::load_unsigned(
            buffer,
            property.data_offset(),
            property.data_size());
      }
      offset = property.end_offset();
    }
    std::printf("Track no: %d, type: %d\n", track_number, track_type);

    if (track_type == TRACK_TYPE_VIDEO) {
      video_track_number = track_number;
    }

    offset = track.end_offset();
  }

  std::printf("ALL'S WELL.\n");

  std::shared_ptr<char> header(
      new char[header_length],
      [] (char *p) { delete [] p; });
  ::memcpy(header.get(), header_buffer.get(), header_length);
  stream_.get()->set_header(header, header_length);

  input_.get()->change_state(std::shared_ptr<StreamInputState>(
      new StreamingState(input_, stream_, video_track_number)));

  return segment_data_offset;
}
