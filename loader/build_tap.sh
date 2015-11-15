#!/bin/sh
bas2tap -a10 -szxmines2 loader.bas
cat loader.tap zxmines2-loadscr.tap zxmines2-mcblock.tap > zxmines2-loading-screen.tap
