#!/bin/bash

export FFMPEG=$HOME/repos/FFmpeg/ffmpeg

#./bt-stream &

#ffmpeg -re -i data/univac.webm -vcodec copy -acodec copy -f webm http://localhost:8080/publish/first?password=secret -v quiet #&
#${FFMPEG} -s 320x240 -r 25 -f video4linux2 -i /dev/video0 -f webm http://localhost:8080/publish/first?password=secret #&

#${FFMPEG} -s 320x240 -r 5 -f video4linux2 -i /dev/video0 -f rawvideo http://localhost:8090/webcam.ffm

${FFMPEG} -re -i data/univac.webm -vcodec copy -acodec copy -f webm http://localhost:8081/publish/first?password=secret
