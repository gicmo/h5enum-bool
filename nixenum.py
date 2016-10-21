#!/usr/bin/env python
from __future__ import print_function
import h5py

print("H5PY version: " + str(h5py.__version__))
print("HDF5 version: " + str(h5py.version.api_version))

fd = h5py.File("nixenum.h5")
p = fd["metadata/s/properties/boolProperty"]
print(p)
print(p[:])
