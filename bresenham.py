def bresenham(startPoint, endPoint):
   startPoint = [int(startPoint[0]),int(startPoint[1]),int(startPoint[2])]

   endPoint = [int(endPoint[0]),int(endPoint[1]),int(endPoint[2])]

   steepXY = (abs(endPoint[1] - startPoint[1]) > abs(endPoint[0] - startPoint[0]))
   if(steepXY):   
      startPoint[0], startPoint[1] = startPoint[1], startPoint[0]
      endPoint[0], endPoint[1] = endPoint[1], endPoint[0]

   steepXZ = (abs(endPoint[2] - startPoint[2]) > abs(endPoint[0] - startPoint[0]))
   if(steepXZ):
      startPoint[0], startPoint[2] = startPoint[2], startPoint[0]
      endPoint[0], endPoint[2] = endPoint[2], endPoint[0]

   delta = [abs(endPoint[0] - startPoint[0]), abs(endPoint[1] - startPoint[1]), abs(endPoint[2] - startPoint[2])]

   errorXY = delta[0] / 2
   errorXZ = delta[0] / 2

   step = [
      -1 if startPoint[0] > endPoint[0] else 1,
      -1 if startPoint[1] > endPoint[1] else 1,
      -1 if startPoint[2] > endPoint[2] else 1
   ]

   y = startPoint[1]
   z = startPoint[2]

   for x in range(startPoint[0], endPoint[0], step[0]):
      point = [x, y, z]

      if(steepXZ):
          point[0], point[2] = point[2], point[0]
      if(steepXY):
          point[0], point[1] = point[1], point[0]

      print (point)

      errorXY -= delta[1]
      errorXZ -= delta[2]

      if(errorXY < 0):
          y += step[1]
          errorXY += delta[0]

      if(errorXZ < 0):
          z += step[2]
          errorXZ += delta[0]