'''
.d8888. d8888b. d8888b. d888888b d888888b d88888b   d88888b d8888b. d888888b d888888b 
88'  YP 88  `8D 88  `8D   `88'   `~~88~~' 88'       88'     88  `8D   `88'   `~~88~~' 
`8bo.   88oodD' 88oobY'    88       88    88ooooo   88ooooo 88   88    88       88    
  `Y8b. 88~~~   88`8b      88       88    88~~~~~   88~~~~~ 88   88    88       88    
db   8D 88      88 `88.   .88.      88    88.       88.     88  .8D   .88.      88    
`8888Y' 88      88   YD Y888888P    YP    Y88888P   Y88888P Y8888D' Y888888P    YP    
'''
from bin.moosecore.moosecore import Game
import bin.moosecore.mooselog
import bin.moosecore.gamemap

def main():

    game = Game()

    sprite = bin.moosecore.Sprite()

    game.world.add_child(sprite)

    bin.moosecore.mooselog.log.info('starting game')
    game.start()

    gameMap.pool.close()
    gameMap.pool.join()


if __name__ == '__main__':
        main()