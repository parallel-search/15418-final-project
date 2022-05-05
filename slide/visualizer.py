import sys
from tkinter import *

if len(sys.argv) != 2:
  raise Exception("expected: init_board_file")

board = []
init_board_file = open(sys.argv[1], "r")
for line in init_board_file:
  board.append(line.split())
init_board_file.close()

print(board)

DIM_X, DIM_Y = 46, 31
width=len(board[0])
height=len(board)

root = Tk()

try:
  root.attributes('-type', 'dialog')
except:
  pass

steps = 0
root.title(steps)
root.geometry(f"{width*DIM_X}x{height*DIM_Y}")

for r in range(height):
  for c in range(width):
    Label(root, width=3, height=1, text=f"{board[r][c]}", relief='solid',bg='white').grid(row=r,column=c)

root.mainloop()
