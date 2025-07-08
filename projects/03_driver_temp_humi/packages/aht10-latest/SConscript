
from building import *

cwd     = GetCurrentDir()
src     = ['src/aht10.c']
path    = [cwd + '/inc']

if GetDepend('PKG_AHT10_USING_SENSOR_V2'):
    src += ['src/sensor_asair_aht10.c']

if GetDepend('PKG_USING_AHT10_SAMPLE'):
    src += ['samples/aht10_sample.c']

group = DefineGroup('aht10', src, depend = ['PKG_USING_AHT10'], CPPPATH = path)

Return('group')
