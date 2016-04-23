#!/usr/bin/env python3

import sys, os
import matplotlib.pyplot as plot

xkcdstyle = 'XKCD_STYLE' in os.environ
if xkcdstyle:
    plot.xkcd()
plot.figure()

multiple = 1
smultiple = ''

for path in sys.argv[1:]:
    if path.startswith('*'):
        multiple = float(path[1:])
        smultiple = ' * ' + path[1:]
        continue
    with open(path, 'rb') as file:
        lines = file.read()
    lines = lines.decode('utf-8', 'strict').split('\n')
    label, dim, xpoints, values = lines[0], int(lines[1]), lines[2], lines[3:]
    label += smultiple
    xpoints = [int(x) for x in xpoints.split(' ')]
    xpoints[1] += 1
    xpoints = list(range(*xpoints))
    if dim == 3:
        ypoints, values = values[0], values[1:]
        ypoints = [int(x) for x in ypoints.split(' ')]
        ypoints[1] += 1
        ypoints = list(range(*ypoints))
    values = [float(v) * multiple for v in values if v != '']
    if dim == 2:
        if 'PER_BIT' in os.environ:
            values = [y / x for y, x in zip(values, xpoints)]
        plot.plot(xpoints, values, label = label)
    elif dim == 3:
        pass
    multiple = 1
    smultiple = ''

if dim == 2:
    plot.legend(loc = 'best')
    plot.xlabel('bits')
    plot.ylabel('time')
elif dim == 3:
    pass

if not xkcdstyle:
    plot.grid(True)
plot.show()
