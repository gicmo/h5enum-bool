#!/usr/bin/env python
from __future__ import print_function
import h5py

print("H5 version: " + str(h5py.__version__))

fd = h5py.File("nixenum.h5")
p = fd["metadata/s/properties/boolProperty"]
print(p)
print(p[:])
