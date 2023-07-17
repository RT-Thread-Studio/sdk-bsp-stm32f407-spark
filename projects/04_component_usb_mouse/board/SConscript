import os
import rtconfig
from building import *

Import('SDK_LIB')

cwd = GetCurrentDir()

# add general drivers
src = Split('''
board.c
CubeMX_Config/Src/stm32f4xx_hal_msp.c
''')
path =  [cwd]
path += [cwd + '/CubeMX_Config/Inc']

CPPDEFINES = ['STM32F407xx']
group = DefineGroup('Drivers', src, depend = [''], CPPPATH = path, CPPDEFINES = CPPDEFINES)

Return('group')
