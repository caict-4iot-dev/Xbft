#!/usr/bin/env python
# -*- coding: utf-8 -*-

import os
import os.path
path =  os.path.dirname(os.getcwd())
for root, dirs, files in os.walk(path):
    for name in files:
        if (name.endswith(".h") or name.endswith(".cpp")):
            localpath = root + '/' + name
            print("clang-format -i %s -style=File" %(localpath))
            os.system("clang-format -i %s -style=File" %(localpath))
