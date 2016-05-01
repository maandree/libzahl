#!/usr/bin/env python3
# See LICENSE file for copyright and license details.

import sys

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

for i in range(line_count):
    best_sec = None
    best_nsec = None
    best_line = None
    for lines in files:
        line = lines[i]
        [sec, nsec] = line.split(':')[1].split(' ')[1].split('.')
        [sec, nsec] = [int(sec), int(nsec)]
        if best_sec is None or sec < best_sec or (sec == best_sec and nsec < best_nsec):
            best_sec, best_nsec, best_line = sec, nsec, line
    print(best_line)
