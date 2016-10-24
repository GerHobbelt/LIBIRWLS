# -*- coding: UTF-8 -*-
from distutils.core import setup
from distutils.extension import Extension
from Cython.Distutils import build_ext
import numpy as np

ext_modules = [ 
        Extension('LIBIRWLS',
            include_dirs=['.','../include/',np.get_include()],
            sources = ['pythonmodule.c'],
            library_dirs = ['/usr/lib/atlas-base/atlas',"../build/"],
            extra_objects = ['../build/LIBIRWLS-predict.o','../build/PIRWLS-train.o','../build/PSIRWLS-train.o','../build/IOStructures.o','../build/ParallelAlgorithms.o','../build/kernels.o'],
            extra_compile_args=["-llapack", "-lf77blas", "-lcblas", "-latlas", "-lgfortran",'-fopenmp'],
            extra_link_args=[ "-llapack", "-lf77blas", "-lcblas", "-latlas", "-lgfortran",'-fopenmp']
        )
        ]

setup(
        name = 'LIBIRWLS',
        version = '2.0',
        include_dirs = [np.get_include()], #Add Include path of numpy
        ext_modules = ext_modules
      )