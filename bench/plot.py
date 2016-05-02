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

xint = lambda x : (float(x) if '.' in x else int(x))

multiple = 1
smultiple = ''

multiples  = [1]    * len(sys.argv[1:])
smultiples = ['']   * len(sys.argv[1:])
paths      = [None] * len(sys.argv[1:])

i = 0
for arg in sys.argv[1:]:
    if arg.startswith('*'):
        multiples[i] = float(arg[1:])
        smultiples[i] = ' * ' + arg[1:]
    else:
        paths[i] = arg
    i += 1

multiples  = multiples[:i]
smultiples = smultiples[:i]
paths      = paths[:i]

xpoints = [None] * i
ypoints = [None] * i
values  = [None] * i
labels  = [None] * i

for i, path in enumerate(paths):
    with open(path, 'rb') as file:
        lines = file.read()
    lines = lines.decode('utf-8', 'strict').split('\n')
    labels[i], dim, values[i] = lines[0] + smultiples[i], int(lines[1]), lines[2:]
    if dim > 1:
        xpoints[i], values[i] = values[i][0], values[i][1:]
        xpoints[i] = [int(x) for x in xpoints[i].split(' ')]
        xpoints[i][1] += 1
        xpoints[i] = list(range(*xpoints[i]))
    if dim > 2:
        ypoints[i], values[i] = values[i][0], values[i][1:]
        ypoints[i] = [int(x) for x in ypoints[i].split(' ')]
        ypoints[i][1] += 1
        ypoints[i] = list(range(*ypoints[i]))
    values[i] = [xint(v) * multiples[i] for v in values[i] if v != '']
    if dim == 2:
        if 'PER_BIT' in os.environ:
            values[i] = [y / x for y, x in zip(values[i], xpoints[i])]

data = [[[i], (values[i], xpoints[i], ypoints[i])] for i in range(len(values))]
data.sort(key = lambda x : x[1])
merged, data = [data[0]], data[1:]
for ([i], d) in data:
    if d == merged[-1][1]:
        merged[-1][0].append(i)
    else:
        merged.append(([i], d))

xpoints = [xpoints[i[0]] for (i, _) in merged]
ypoints = [ypoints[i[0]] for (i, _) in merged]
values  = [values[i[0]]  for (i, _) in merged]
labels  = [' & '.join(labels[j] for j in i)  for (i, _) in merged]

vmin = min(min(min(v) for v in values), 0)
vmax = max(max(max(v) for v in values), 0)

if dim == 1:
    plot.ylabel('time')
    if len(values[0]) == 1:
        plot.bar(range(len(values)),
                 [vs[0] for vs in values],
                 align = 'center',
                 orientation = 'vertical',
                 tick_label = labels)
        labels = None
    elif 'VIOLIN_STYLE' in os.environ:
        plot.violinplot(values,
                        vert = True,
                        showmeans = 'SHOW_MEAN' in os.environ,
                        showmedians = 'SHOW_MEAN' not in os.environ,
                        showextrema = True)
    else:
        plot.boxplot(values,
                    vert = True,
                     notch = 'NOTCH_STYLE' in os.environ,
                     patch_artist = 'PATCH_ARTIST' in os.environ)
        if 'SHOW_MEAN' in os.environ:
            for i in range(len(values)):
                mean = sum(values[i]) / len(values[i])
                plot.plot([i + 0.75, i + 1.25], [mean, mean]);
    if labels is not None:
        plot.setp(fig.axes,
                  xticks = [x + 1 for x in range(len(values))],
                  xticklabels = labels)
elif dim == 2:
    for i in range(len(values)):
        plot.plot(xpoints[i], values[i], label = labels[i])
    plot.legend(loc = 'best')
    plot.xlabel('bits')
    plot.ylabel('time')
elif dim == 3:
    pass

plot.ylim((vmin * 1.1, vmax * 1.1))

if not xkcdstyle:
    plot.grid(True)
plot.show()
