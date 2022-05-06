import numpy as np
import pylab as pl
import sys

from matplotlib import collections  as mc

if __name__ == "__main__":

  def printit(f):
    def inner():
      x = f()
      print(x)
      return x
    return inner

  def read():
    return int(input())

  def read_pair():
    return tuple(map(int, input().split()))

  width, height = read_pair()

  num_pts = read()
  pts = []
  for _ in range(num_pts):
    x, y = read_pair()
    pts.append((x, y)) 

  start, end = read_pair()
  print(pts)
  print(start, end)

  edges = []
  for _ in range(num_pts):
    edges.append([])
    num_edges = read()
    for _ in range(num_edges):
      to, weight = read_pair()
      edges[-1].append((to, weight))

  seen = set()
  lines = []
  weights = []
  for i in range(num_pts):
    for edge in edges[i]:
      to, weight = edge
      if (i, to) not in seen:
        seen.add((i, to))
        seen.add((to, i))
        lines.append([pts[i], pts[to]])
        weights.append(weight)

  lc = mc.LineCollection(lines)
  fig, ax = pl.subplots()
  ax.add_collection(lc)
  ax.autoscale()
  ax.margins(0.1)

  for i, [(x1,y1),(x2,y2)] in enumerate(lines):
    ax.text((x1+x2)/2,(y1+y2)/2,str(weights[i]),weight="bold")

  print(start, end, pts)
  ax.text(*pts[start],"S",weight="bold",color="red")
  ax.text(*pts[end],"T",weight="bold",color="red")

  pl.xticks(np.arange(0, width+1, 1.0))
  pl.yticks(np.arange(0, height+1, 1.0))

  for pt in pts:
    pl.plot(*pt,marker="o",markersize=4,color="black")

  pl.show()
