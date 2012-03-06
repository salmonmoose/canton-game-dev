def myCallback(args):
    for arg in args:
        print arg


def useCallback(callback, args):
    callback(args)

if __name__ == '__main__':
    useCallback(myCallback, ['fish', 'dog', 'cat'])    
