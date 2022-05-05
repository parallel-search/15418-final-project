import sys
from tkinter import *

UP, DOWN, LEFT, RIGHT = [str(i) for i in range(1, 5)]

if len(sys.argv) != 3:
  raise Exception("expected: init_board_file sol_file")

board = []
init_board_file = open(sys.argv[1], "r")
for line in init_board_file:
  board.append(line.split())
init_board_file.close()

print(board)

sol_file = open(sys.argv[2], "r")
sol = sol_file.readline().split()
sol_file.close()

DIM_X, DIM_Y = 46, 31
width=len(board[0])
height=len(board)
zero = [-1, -1]
for r in range(height):
  for c in range(width):
    if board[r][c] == '0':
      zero = [r, c]
      break

root = Tk()

try:
  root.attributes('-type', 'dialog')
except:
  pass

root.title(0)
root.geometry(f"{width*DIM_X}x{height*DIM_Y}")

labels = []
for r in range(height):
  labels.append([])
  for c in range(width):
    l = Label(root, width=3, height=1, text=f"{board[r][c]}", relief='solid',bg='white')
    l.grid(row=r,column=c)
    labels[-1].append(l)

L_ARROW, R_ARROW = 113, 114

on = 0
def key_press(e):
  global on, zero, board, labels
  if e.keycode == R_ARROW:
    if on >= len(sol):
      return
    move = sol[on]
    new_zero = zero.copy()
    if move == UP:
      new_zero[0] += 1
    elif move == DOWN:
      new_zero[0] -= 1
    elif move == RIGHT:
      new_zero[1] -= 1
    elif move == LEFT:
      new_zero[1] += 1
    board[zero[0]][zero[1]] = board[new_zero[0]][new_zero[1]]
    board[new_zero[0]][new_zero[1]] = '0'

    labels[zero[0]][zero[1]]['text'] = board[zero[0]][zero[1]]
    labels[new_zero[0]][new_zero[1]]['text'] = board[new_zero[0]][new_zero[1]]

    zero = new_zero
    on += 1
  root.title(on)

root.bind("<KeyRelease>", key_press)

root.mainloop()
