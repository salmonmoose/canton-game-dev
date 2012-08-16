import numpy

class Water:
    def __init__(self):
        self.mapSize = (16,16,16)
        self.waterMap = numpy.zeros(self.mapSize, int)

    def automata(self):
        for z in range(self.mapSize[2]):
            for y in range(self.mapSize[1]):
                for x in range(self.mapSize[0]):
                    if(z > 0):
                        self.fill(
                            (x,y,z),
                            (x,y,z-1)
                        )
                    if(x < 15):
                        self.spill(
                            (x,y,z),
                            (x+1,y,z)
                            )

                    if(y < 15):
                        self.spill(
                            (x,y,z),
                            (x,y+1,z)
                            )
                    if(x > 0):
                        self.spill(
                            (x,y,z),
                            (x-1,y,z)
                            )

                    if(y > 0):
                        self.spill(
                            (x,y,z),
                            (x,y-1,z)
                            )
                    
    def fill(self, sourceCell, destinationCell):
        drop = min(self.waterMap[sourceCell], 1 - self.waterMap[destinationCell])
        self.waterMap[destinationCell] += drop
        self.waterMap[sourceCell] -= drop


    def spill(self, sourceCell, destinationCell):
        drop = min(self.waterMap[sourceCell] / 5, self.waterMap[sourceCell] - self.waterMap[destinationCell])
        if(drop > 0):
            self.waterMap[destinationCell] += drop
            self.waterMap[sourceCell] -= drop

