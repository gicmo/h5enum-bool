#!/usr/bin/env python
from __future__ import print_function
import h5py

print("H5 version: " + str(h5py.__version__))

fd = h5py.File("h5enum.h5")

for d in ['ds1', 'ds2']:
    p = fd[d]
    print(p)
    d = p[()]
    print(d, type(d))
