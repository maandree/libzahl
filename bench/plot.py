#!/usr/bin/env python3
# See LICENSE file for copyright and license details.


# Invoke using `env XKCD_STYLE=` to for more a comical plot style.

# Invoke using `env PER_BIT=` to divide all time values by the number
# of bits that where processed. This applies to 2-dimensional data only.

# Invoke using `env VIOLIN_STYLE=` to draw violin plots rather than
# box plots. This applies to multisample 1-dimensional data only.
# If used, used `env SHOW_MEAN=` will show that mean value in place
# of the median value.

# For multisample 1-dimensional, if `env VIOLIN_STYLE=` is not used
# `env NOTCH_STYLE=`, `env PATCH_ARTIST`, and `env SHOW_MEAN` may be
# applied.


import sys, os
import matplotlib.pyplot as plot

xkcdstyle = 'XKCD_STYLE' in os.environ
if xkcdstyle:
    plot.xkcd()
fig = plot.figure()

multiple = 1
smultiple = ''

labels_1d = []
values_1d = []

xint = lambda x : (float(x) if '.' in x else int(x))

for path in sys.argv[1:]:
    if path.startswith('*'):
        multiple = float(path[1:])
        smultiple = ' * ' + path[1:]
        continue
    with open(path, 'rb') as file:
        lines = file.read()
    lines = lines.decode('utf-8', 'strict').split('\n')
    label, dim, values = lines[0] + smultiple, int(lines[1]), lines[2:]
    if dim > 1:
        xpoints, values = values[0], values[1:]
        xpoints = [int(x) for x in xpoints.split(' ')]
        xpoints[1] += 1
        xpoints = list(range(*xpoints))
    if dim > 2:
        ypoints, values = values[0], values[1:]
        ypoints = [int(x) for x in ypoints.split(' ')]
        ypoints[1] += 1
        ypoints = list(range(*ypoints))
    values = [xint(v) * multiple for v in values if v != '']
    if dim == 1:
        labels_1d.append(label)
        values_1d.append(values)
    elif dim == 2:
        if 'PER_BIT' in os.environ:
            values = [y / x for y, x in zip(values, xpoints)]
        plot.plot(xpoints, values, label = label)
    elif dim == 3:
        pass
    multiple = 1
    smultiple = ''

if dim == 1:
    plot.ylabel('time')
    if len(values_1d[0]) == 1:
        plot.bar(range(len(values_1d)),
                 [vs[0] for vs in values_1d],
                 align = 'center',
                 orientation = 'vertical',
                 tick_label = labels_1d)
        labels_1d = None
    elif 'VIOLIN_STYLE' in os.environ:
        plot.violinplot(values_1d,
                        vert = True,
                        showmeans = 'SHOW_MEAN' in os.environ,
                        showmedians = 'SHOW_MEAN' not in os.environ,
                        showextrema = True)
    else:
        plot.boxplot(values_1d,
                    vert = True,
                     notch = 'NOTCH_STYLE' in os.environ,
                     patch_artist = 'PATCH_ARTIST' in os.environ)
        if 'SHOW_MEAN' in os.environ:
            for i in range(len(values_1d)):
                mean = sum(values_1d[i]) / len(values_1d[i])
                plot.plot([i + 0.75, i + 1.25], [mean, mean]);
    if labels_1d is not None:
        plot.setp(fig.axes,
                  xticks = [x + 1 for x in range(len(values_1d))],
                  xticklabels = labels_1d)
elif dim == 2:
    plot.legend(loc = 'best')
    plot.xlabel('bits')
    plot.ylabel('time')
elif dim == 3:
    pass

if not xkcdstyle:
    plot.grid(True)
plot.show()
