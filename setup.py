from distutils.core import setup, Extension
setup(name = "synctest", version = "1.0", ext_modules = [Extension('synctest', ['test.c', 'sync.c'])])
