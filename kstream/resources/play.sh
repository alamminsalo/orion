#!/bin/bash
#
#Default launch script for mplayer,
#change as needed
#
#Takes full channel url as parameter

livestreamer $1 best --player 'mplayer -cache 2048' --hls-segment-threads 2
