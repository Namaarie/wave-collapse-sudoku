import time
import copy
from os import system, name
import sys
import getopt

class Sudoku():
	def __init__(self):
		self.__board = []
		self.__collapses = 0
		self.__showProgress = False
		self.ResetBoard()
		self.__debug = False
		self.__sleep = False

	def ResetBoard(self):
		self.__board = []
		for x in range(9):
			temp = []
			for y in range(9):
				temp.append(Tile(1, y, x))
			self.__board.append(temp)

		for a in range(9):
			for b in range(9):
				related = []
				tile = self.__board[a][b]
				# collapse in row
				for i in self.__board[tile.GetPosition()[1]]:
					if i not in related:
						related.append(i)

				# collaspe in column
				for y in range(9):
					target = self.__board[y][tile.GetPosition()[0]]
					if target not in related:
						related.append(target)

				# collapse in quadrant
				for x in range(9):
					for y in range(9):
						target = self.__board[y][x]
						if target.GetQuadrant() == tile.GetQuadrant():
							if target not in related:
								related.append(target)
				tile.AddRelated(related)
	
	def Sleep(self, value):
		self.__sleep = value

	def PrintBoard(self):
		clear()
		for index, i in enumerate(self.__board):
			if index == 3 or index == 6:
				print("-" * 21)
			for index2, o in enumerate(i):
				if index2 == 3 or index2 == 6:
					print("| " + str(o), end='')
				else:
					print("|" + str(o), end='')
			print("|")
		print("\n")

	def PrintBoardBig(self):
		clear()
		for index, i in enumerate(self.__board):
			for b in range(3):
				for index2, o in enumerate(i):
					print("|", end="")
					for a in range(3):
						print(colored('hello', 'red'), colored('world', 'green'))
						print(str(o), end="")
				print("|")
		print("\n")

	def SetBoard(self, board):
		# self.ResetBoard()
		for i in range(9):
			for j in range(9):
				self.__board[i][j].SetValue(board[i][j])

	def SetBoardValues(self, boardString):
		assert len(boardString) == 81, "NOT ENOUGH VALUES"
		# self.ResetBoard()
		index = 0
		for x in range(9):
			for y in range(9):
				if boardString[index] == '.':
					self.__board[x][y].SetValue(0)
				else:
					self.__board[x][y].SetValue(int(boardString[index]))
				index += 1

	def Collapse(self, tile):
		self.__collapses += 1
		for i in tile.GetRelated():
			i.LosePossibility(tile.GetValue())
			if i.GetEntropy() == 1:
				i.Collapse()
				self.Collapse(i)
				if self.__showProgress:
					self.PrintBoard()
					time.sleep(0.1)
		

	def GetMinEntropy(self):
		minEntropy = -1
		minTile = None
		for i in range(9):
			for j in range(9):
				entropy = self.__board[i][j].GetEntropy()
				if entropy == 1:
					return self.__board[i][j]
				if (entropy < 10) and (entropy != 0):
					minEntropy = self.__board[i][j].GetEntropy()
					minTile = self.__board[i][j]

		return minTile

	def IsSolved(self):
		for i in range(9):
			for j in range(9):
				if self.__board[i][j].GetValue() == 0:
					return False
		return True

	def SizeOfBoard(self):
		size = 0
		for i in self.__board:
			for j in i:
				size += 1
		return size

	def RecursiveSolve(self, board):
		boardCopy = copy.deepcopy(board)
		if(self.__showProgress):
			self.PrintBoard()
		minTile = self.GetMinEntropy()
		if minTile == None:
			return boardCopy
		if minTile.GetEntropy() == 1:
			minTile.Collapse()
			self.Collapse(minTile)
			if self.IsSolved():
				return boardCopy
		else:
			# need to guess
			for i in range(minTile.GetEntropy()):
				minTile.ConditionalCollapse(i)
				self.Collapse(minTile)
				self.RecursiveSolve(self.__board)
				if self.IsSolved():
					return boardCopy

	def Solve(self, showProgress=False, recursive=True):
		self.__showProgress = showProgress
		if self.__showProgress:
			self.PrintBoard()
			time.sleep(0.5)
		for y in range(9):
			for x in range(9):
				target = self.__board[x][y]
				if target.GetEntropy() == 0:
					self.Collapse(target)
		if self.__debug:
			print("TOTAL COLLAPSES: " + str(self.__collapses))
			print("DONE DEFAULT")
		if self.__showProgress:
			time.sleep(0.5)
		if self.IsSolved():
			return True
		else:
			if recursive:
				self.RecursiveSolve(self.__board)
				if(self.IsSolved()):
					return True
				else:
					return False
			else:
				print("UNABLE TO SOLVE WITHOUT RECURSION")
				return False
		
	def Solvable(self):
		for y in range(9):
			for x in range(9):
				target = self.__board[x][y]
				if target.GetEntropy() == 0:
					self.Collapse(target)

		return self.IsSolved()

class Tile():
	def __init__(self, value, x, y):
		self.__possibilities = [1, 2, 3, 4, 5, 6, 7, 8, 9]
		self.__value = value
		self.__x = x
		self.__y = y
		self.__quadrant = self.FindQuadrant()
		self.__relatedTiles = []

	def SetValue(self, value):
		self.__possibilities = [1, 2, 3, 4, 5, 6, 7, 8, 9]
		self.__value = value
		if(value != 0):
			self.__possibilities = []

	def GetValue(self):
		return self.__value

	def AddRelated(self, related):
		self.__relatedTiles += related

	def GetRelated(self):
		return self.__relatedTiles

	def FindQuadrant(self):
		if abs(self.__x - 4) <= 1:
			# in center column
			if abs(self.__y - 4) <= 1:
				# in absolute center
				return 4
			elif self.__y - 4 < 0:
				# top center
				return 1
			else:
				# bottom center
				return 7
		elif self.__x - 4 < 0:
			# left colum
			if abs(self.__y - 4) <= 1:
				# in left center
				return 3
			elif self.__y - 4 < 0:
				# top left
				return 0
			else:
				# bottom left
				return 6
		else:
			# right column
			if abs(self.__y - 4) <= 1:
				# right center
				return 5
			elif self.__y - 4 < 0:
				# top right
				return 2
			else:
				# bottom right
				return 8

	def GetPosition(self):
		return [self.__x, self.__y]

	def GetQuadrant(self):
		return self.__quadrant

	def GetEntropy(self):
		return len(self.__possibilities)

	def GetPossibilities(self):
		return self.__possibilities

	def LosePossibility(self, value):
		try:
			self.__possibilities.remove(value)
		except ValueError:
			pass

	def ConditionalCollapse(self, index):
		self.__value = self.__possibilities[index]
		self.__possibilities = []

	def Collapse(self):
		self.__value = self.__possibilities[0]
		self.__possibilities = []

	def __str__(self):
		if self.__value == 0:
			return "."
		else:
			return str(self.__value)

def clear():
  
    # for windows
    if name == 'nt':
        _ = system('cls')
  
    # for mac and linux(here, os.name is 'posix')
    else:
        _ = system('clear')

def Help():
	print("-h: help\n-b: run benchmark\n-p: show benchmark progress\n-s: screensaver\n-c [board]: run solver on custom board")
	
def main():
	sudoku = Sudoku()
	f = open("hardsudoku.csv", "r")
	try:
		opts, args = getopt.getopt(sys.argv[1:], "hbspc:")
	except getopt.GetoptError:
		Help()
		sys.exit(2)
	for opt, arg in opts:
		if opt == "-h":
			Help()
			sys.exit(0)
		elif opt == "-c":
			assert len(arg) == 81, "INVALID BOARD LENGTH"
			sudoku.SetBoardValues(arg)
			sudoku.Solve(True)
			sys.exit(0)
		elif opt == "-s":
			content = f.readlines()
			for index, i in enumerate(content):
				try:
					sudoku.SetBoardValues(i.replace("\n", "").split(",")[0])
				except Exception:
					print("INVALID STRING AT LINE {}".format(index + 1))
				if not sudoku.Solve(True):
					print("FAILED ON LINE: " + str(index + 1))
			sys.exit(0)
		elif opt == "-b" or opt == "-p":
			content = f.readlines()
			success = 0
			fail = 0
			start = time.time()
			
			for index, i in enumerate(content):
				if index % 100 == 0:
					clear()
					print("RUNNING TEST {}/{}".format(index + 1, len(content)))
				try:
					sudoku.SetBoardValues(i.replace("\n", "").split(",")[0])
				except Exception:
					print("INVALID STRING AT LINE {}".format(index + 1))
				if opt == "-p":
					sudoku.PrintBoard()
					print("RUNNING TEST {}/{}".format(index + 1, len(content)))
					result = sudoku.Solve()
				elif opt == "-b":
					result = sudoku.Solve()
				if result:
					if opt == "-p":
						sudoku.PrintBoard()
					success += 1
				else:
					fail += 1
					# print("FAILED ON LINE: " + str(index + 1))
			print("DONE TESTING {} PUZZLES, SUCCESS: {}, FAILS: {}, PERCENTAGE: {}%, TIME TAKEN: {}s".format(len(content), success, fail, success/len(content) * 100, time.time() - start))
			sys.exit(0)
if __name__ == '__main__':
	main()
