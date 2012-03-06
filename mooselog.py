import logging
import multiprocessing

log = multiprocessing.log_to_stderr()
log.setLevel(multiprocessing.SUBDEBUG)