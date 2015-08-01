#!/bin/bash
#
#Default launch script for mplayer,
#change as needed
#
#Takes full channel url as parameter

livestreamer $1 best --player 'mpv' --hls-segment-threads 2
