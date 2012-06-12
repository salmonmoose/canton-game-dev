from distutils.core import setup, Extension

ext = Extension('marchingcubes', sources=['marchingcubesmodule.c'])

setup(name='marchingcubes', version='1.0', description='Test description', ext_modules=[ext])
