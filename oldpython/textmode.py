import curses

screen = curses.initscr()

curses.noecho()
curses.halfdelay(1)
curses.mousemask(1)
screen.keypad(1)

signal_caught = False

while signal_caught == False:
	key = screen.getch()
	mouse = curses.getmouse()

	screen.clear()
	screen.addstr(mouse[1], mouse[2], '*')
	screen.refresh()
curses.nocbreak()
screen.keypad(0)
curses.echo()
curses.endwin()
