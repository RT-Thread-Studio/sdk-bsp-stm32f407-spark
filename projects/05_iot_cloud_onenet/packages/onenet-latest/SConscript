from building import *

cwd = GetCurrentDir()
src = []

if GetDepend(['PKG_USING_ONENET_SAMPLE']):
    src += Glob('samples/onenet_sample.c')

if GetDepend(['ONENET_USING_MQTT']):
    src += Glob('src/onenet_mqtt.c')

if GetDepend(['ONENET_USING_AUTO_REGISTER'] or ['ONENET_USING_HTTP']):
    src += Glob('src/onenet_http.c')

path = [cwd + '/inc']

group = DefineGroup('onenet', src, depend = ['PKG_USING_ONENET'], CPPPATH = path)

Return('group')
