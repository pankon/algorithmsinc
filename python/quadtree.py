"""
quadtree - really a ktree
"""

import numpy
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D
import random
import itertools
import math
import string
import threading

def default_zero(idim):
	return 0

def default_random(idim):
	return random.random()

def default_compare(self, other, idim):
	return self.coordinates[idim] > other.coordinates[idim]

class Coordinate(object):
	def __init__(self, ndim, vector, zero_func = None, random_func = None, compare_func = None):
		self.ndim = ndim
		self.coordinates = vector
		self.zero_func = zero_func if zero_func else default_zero
		self.random_func = random_func if random_func else default_random
		self.compare_func = compare_func if compare_func else default_compare

	def zero(self, ndim):
		zero_vector =  [self.zero_func(idim) for idim in range(ndim)]
		return Coordinate(ndim, zero_vector)

	def random(self, ndim):
		random_vector = [self.random_func(idim) for idim in range(ndim)]
		return Coordinate(ndim, random_vector)

	def get_bucket(self, other):
		bucket = 0
		for idim in range(ndim):
			bucket += int(self.compare_func(self, other, idim)) * (ndim - idim)
			
		return bucket

	def __add__(self, other):
		if other.ndim != self.ndim:
			raise ArithmeticError("cannot add vectors of different dimensions")

		result_coordinates = [x + y for x, y in zip(self.coordinates, other.coordinates)]

		return Coordinate(self.ndim, result_coordinates)

	def __div__(self, factor):
		# not sure how to make this generic
		if factor is Coordinate:
			# divide accordingly, unsupported right now
			raise ArithmeticError("cannot divide by vector yet")
		else:
			#divide whole vector by factor
			return Coordinate(self.ndim, [x / factor for x in self.coordinates])

	def __str__(self):
		coordinate_view = []
		for x in self.coordinates:
			coordinate_view.append("{0}".format(x))

		return "({})".format(', '.join(coordinate_view))

	def average(self, points):
		total = self.zero(ndim)
		n = len(points)
		if n == 0:
			return 0
		for current in points:
			total += current
		return Coordinate(ndim, (total / n).coordinates)

	def prepare_plot(self, arrays):
		for i, x in enumerate(self.coordinates):
			arrays[i].append(x)

class KTree(object):
	def __init__(self, ndim, points, depth):
		base = Coordinate(ndim, None)
		self.ndim = ndim
		self.children = None
		self.points = points

		if depth > ndim * ndim or len(points) < ndim + 3:
			self.average = base.average(points)
			return

		self.average = base.average(points)

		self.children = [None for _ in range((ndim + 1) * (ndim + 1))]
		self.buckets = [[] for _ in range((ndim + 1) * (ndim + 1))]
		
		while len(points) != 0:
			# greater in x dimension
			current = points.pop()
			bucket = self.average.get_bucket(current)
			#print current, average, hex(bucket)
			self.buckets[bucket].append(current)

		#print [[str(item) for item in bucket] for bucket in self.buckets]

		threads = []
		for i, bucket in enumerate(self.buckets):
			#buckets print depth, bucket
			if depth > 10000: # not fast
				t = threading.Thread(target=self.__setup_child, args=(i, bucket, depth))
				t.daemon = True
				threads.append(t)
				t.start()
			else:
				self.__setup_child(i, bucket, depth)

		if depth > 10000:
			for t in threads:
				t.join()

		self.buckets = None

	def __setup_child(self, i, bucket, depth):
		if len(bucket) > 0:
			child = KTree(ndim, bucket, depth + 1)
			self.children[i] = child
		self.buckets[i] = None

	def __str__(self):
		res = []
		if self.children:
			for child in self.children:
				res.append(str(child))
		else:
			for current in self.points:
				res.append(str(current))
		return "<{0} : [{1}]>".format(self.average, ', '.join(res))

	def prepare_plot(self, arrays, meta_arrays):
		if self.children:
			for child in self.children:
				if child:
					self.average.prepare_plot(arrays)
					self.average.prepare_plot(meta_arrays)
					child.prepare_plot(arrays, meta_arrays)
		else:
			for current in self.points:
				self.average.prepare_plot(arrays)
				current.prepare_plot(arrays)
	
	def plot(self):
		plot_pts = [[] for _ in range(self.ndim)]
		meta_pts = [[] for _ in range(self.ndim)]
		self.prepare_plot(plot_pts, meta_pts)

		plt.close('all')
		if ndim == 3:
			fig = plt.figure()
			ax = fig.add_subplot(111, projection='3d')
			ax.scatter(plot_pts[0], plot_pts[1], plot_pts[2], c="blue", marker='+')
			ax.scatter(meta_pts[0], meta_pts[1], meta_pts[2], c="red", marker='o')
			plt.show()
		else:
			all_subsets = list(itertools.combinations(range(self.ndim), 2))
			names = ['x', 'y', 'z'] + list(string.ascii_lowercase[:-3])
			n_subsets = len(all_subsets)
			nrows = int(math.sqrt(n_subsets))
			ncols = int(n_subsets / nrows + 1)
			print n_subsets, nrows, ncols
			f, axlist = plt.subplots(nrows, ncols)
			plt.ioff()
			axflat = axlist.flatten()
			for i, subset in enumerate(all_subsets):
				ax = axflat[i]
				ax.set_xlabel(names[subset[0]])
				ax.set_ylabel(names[subset[1]])
				ax.scatter(plot_pts[subset[0]], plot_pts[subset[1]], c="blue", marker = '+')
				ax.scatter(meta_pts[subset[0]], meta_pts[subset[1]], c="red", marker = 'o')
			plt.show()
			

if __name__ == "__main__":
	ndim = 2
	base = Coordinate(ndim, None)
	points = [base.random(ndim) for _ in range(10000)]
	# 1,000,000: 18.4000000954
	# 100,000:   1.56399989128s
	# 10,000:    0.140000104904
	# 1,000:     0.0149998664856
	# bad? probably

	import time
	t0 = time.time()
	tree = KTree(ndim, points, 0)
	t1 = time.time()

	print t1 - t0
	tree.plot()
	


