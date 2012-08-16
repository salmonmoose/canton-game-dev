#/usr/bin/python
import numpy
import marchingcubes
import timeit

density = numpy.random.rand(16,16,16)
material = numpy.random.random_integers(0,3,[16,16,16])

def dotime():
	t = timeit.Timer(
		"marchingcubes.generate(density, material)",
	)

	time = t.timeit(300)

	print "took %fs\n" % (time,)

import __builtin__
__builtin__.__dict__.update(locals())

marchingcubes.generate(density, material)

dotime()

