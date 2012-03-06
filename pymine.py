'''
d8888b. db    db .88b  d88. d888888b d8b   db d88888b 
88  `8D `8b  d8' 88'YbdP`88   `88'   888o  88 88'     
88oodD'  `8bd8'  88  88  88    88    88V8o 88 88ooooo 
88~~~      88    88  88  88    88    88 V8o88 88~~~~~ 
88         88    88  88  88   .88.   88  V888 88.     
88         YP    YP  YP  YP Y888888P VP   V8P Y88888P 
'''
from bin.moosecore.moosecore import Game
import bin.moosecore.mooselog
import bin.moosecore.gamemap

def main():
        game = Game()

        gameMap = gamemap.Map(game)

        game.world.add_child(gameMap)

        mooselog.log.info('starting game')
        game.start()

        gameMap.pool.close()
        gameMap.pool.join()


if __name__ == '__main__':
        main()