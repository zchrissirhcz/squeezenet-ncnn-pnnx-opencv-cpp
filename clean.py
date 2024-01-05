# remove files and directories according to .gitignore

import os
import shutil

required_files = [
    "squeezenet_v1.1.ncnn.param",
    "squeezenet_v1.1.ncnn.bin",

    "squeezenet_v1.1.param",
    "squeezenet_v1.1.bin",
]

# remove *.param in root directory
for f in os.listdir('.'):
    if f in required_files:
        continue
    if f.endswith('.param'):
        os.remove(f)

# remove *.bin in root directory
for f in os.listdir('.'):
    if f in required_files:
        continue
    if f.endswith('.bin'):
        os.remove(f)

# remove *.onnx in root directory
for f in os.listdir('.'):
    if f in required_files:
        continue
    if f.endswith('.onnx'):
        os.remove(f)

# remove *.pt in root directory
for f in os.listdir('.'):
    if f in required_files:
        continue
    if f.endswith('.pt'):
        os.remove(f)