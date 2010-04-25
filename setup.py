#!/usr/bin/env python

import os, sys

from distutils.core import setup
from distutils.extension import Extension

import numpy
from Cython.Distutils import build_ext

torus = Extension(
    '_torus',
    sources = ["torus.c", "_torus.pyx"],
    include_dirs=[numpy.get_include() + "/numpy"],
)

setup(
    name = "torus",
    version = "0.1",
    packages = ['torus'],
    ext_modules = [torus],
    cmdclass = {'build_ext': build_ext}
)
