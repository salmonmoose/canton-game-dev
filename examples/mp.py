from multiprocessing import Pool
import time
def f(x):
    print ("in thread", x)
    return x*x

def cb(r):
    print r

if __name__ == '__main__':
    pool = Pool(processes=4)              # start 4 worker processes

    for i in range(100):
        pool.apply_async(f, (i,), callback=cb)

    print ("pool is closed")
    pool.close()

    pool.join()
    print ("pool is joined")