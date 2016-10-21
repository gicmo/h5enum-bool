#!/usr/bin/env python
from __future__ import print_function
import h5py

fd = h5py.File("h5enum.h5")
p = fd["ds1"]
print(p)
d = p[()]
print(d, type(d), d.dtype)
