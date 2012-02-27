
from moosecore import Game
import map

def main():
        game = Game()
        game.world.add_child(map.Map(game))
        print('starting game')
        game.start()

if __name__ == '__main__':
        main()
