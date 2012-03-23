'''
 .o88b.  .d8b.  d8b   db d888888b  .d88b.  d8b   db   
d8P  Y8 d8' `8b 888o  88 `~~88~~' .8P  Y8. 888o  88   
8P      88ooo88 88V8o 88    88    88    88 88V8o 88   
8b      88~~~88 88 V8o88    88    88    88 88 V8o88   
Y8b  d8 88   88 88  V888    88    `8b  d8' 88  V888   
 `Y88P' YP   YP VP   V8P    YP     `Y88P'  VP   V8P   
'''
from bin.moosecore.moosecore import Game
import bin.moosecore.mooselog
import bin.moosecore.gamemap

def main():

    game = Game()

    gameMap = bin.moosecore.gamemap.Map(game)

    game.world.add_child(gameMap)

    bin.moosecore.mooselog.log.info('starting game')
    game.start()

    gameMap.pool.close()
    gameMap.pool.join()


if __name__ == '__main__':
        main()