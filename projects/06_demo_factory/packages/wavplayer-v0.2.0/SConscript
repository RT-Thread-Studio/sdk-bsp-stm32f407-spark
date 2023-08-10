Import('rtconfig')
from building import *

cwd = GetCurrentDir()

path = [cwd,
    cwd + '/inc']

src = Split('src/wavhdr.c')

if GetDepend(['PKG_WP_USING_PLAY']):
    src +=  Split('''
        src/wavplayer.c
        src/wavplayer_cmd.c
        ''')

if GetDepend(['PKG_WP_USING_RECORD']):
    src +=  Split('''
        src/wavrecorder.c
        src/wavrecorder_cmd.c
        ''')

group = DefineGroup('wavplayer', src, depend = ['PKG_USING_WAVPLAYER'], CPPPATH = path)

Return('group')
