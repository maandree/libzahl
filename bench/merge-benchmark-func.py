#!/usr/bin/env python3
# See LICENSE file for copyright and license details.


# Invoke using `env SELECT_MIN=` to select the minimum value,
# rather than concatenate. This applies to 1-dimensional data only.


import sys, os

line_count = None
files = []

for path in sys.argv[1:]:
    with open(path, 'rb') as file:
        data = file.read()
    data = data.decode('utf-8', 'strict')
    if data[-1] == '\n':
        data = data[:-1]
    data = data.split('\n')
    if line_count is None:
        line_count = len(data)
    elif len(data) != line_count:
        print('%s: line count mismatch' % sys.argv[0], file = sys.stderr)
        sys.exit(1)
    files.append(data)

dim = int(files[0][1])
skip = 1 + dim
for i in range(skip):
    print(files[0][i])

if dim > 1:
    for i in range(skip, line_count):
        best_nsec = None
        best_line = None
        for lines in files:
            line = lines[i]
            nsec = int(line)
            if best_nsec is None or nsec < best_nsec:
                best_nsec, best_line = nsec, line
        print(best_line)
elif 'SELECT_MIN' not in os.environ:
    for lines in files:
        for i in range(skip, line_count):
            print(lines[i])
else:
    best_nsec = None
    best_line = None
    for lines in files:
        for i in range(skip, line_count):
            line = lines[i]
            nsec = int(line)
            if best_nsec is None or nsec < best_nsec:
                best_nsec, best_line = nsec, line
    print(best_line)
