#!/usr/bin/env bash
make clean

make clean build

./imageRotation ./images/001.jpg invert ./rotated_images/invert_001.jpg >output/output.log
./imageRotation ./images/001.jpg rotate90 ./rotated_images/rotate_001.jpg >output/output.log 