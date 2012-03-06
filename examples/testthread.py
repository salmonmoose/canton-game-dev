import multiprocessing
import logging

queue = Queue.Queue()

class Async(threading.Thread):
	def __init__(self, queue):
		threading.Thread.__init__(self)
		self.queue = queue

	def run(self):
		while 1:
			i = self.queue.get()
			if i is None:
				break
			print "sleeping {} sec from thread {}".format(20-i, i)
			time.sleep(20-i)
			print "finished sleeping from thread {}".format(i)
			self.queue.task_done()

def main():

	for i in range(20):
		queue.put(i)

	for i in range(5):
		t = Async(queue)
		t.setDaemon(True)
		t.start()

	queue.join()

main()