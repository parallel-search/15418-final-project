import sys

if len(sys.argv) != 3:
  raise Exception("expected: width height")

width = int(sys.argv[1])
height = int(sys.argv[2])

board = [[i + j*width for i in range(width)] for j in range(height)]
zero = [0, 0]

import random
randomness = width**2 * height**2

for _ in range(randomness):
  seed = random.randint(1, width-1)
  d = 1
  if seed <= zero[1]:
    seed -= 1
    d = -1
  for i in range(zero[1], seed, d):
    board[zero[0]][i] = board[zero[0]][i+d]
  board[zero[0]][seed] = 0
  zero[1] = seed

  seed = random.randint(1, height-1)
  d = 1
  if seed <= zero[0]:
    seed -= 1
    d = -1
  for i in range(zero[0], seed, d):
    board[i][zero[1]] = board[i+d][zero[1]]
  board[seed][zero[1]] = 0
  zero[0] = seed

[print(" ".join(str(tile) for tile in row)) for row in board]
