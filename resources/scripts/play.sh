#!/bin/bash
#
#Default launch script for livestreamer + mpv
#
#Takes full channel url as parameter
#
#Adjust as needed

livestreamer $1 best --player mpv
