import signal
import zmq
import labview

signal.signal(signal.SIGINT, signal.SIG_DFL);

ctx=zmq.Context()
sock = ctx.socket(zmq.SUB)
sock.connect('tcp://127.0.0.1:5000')
sock.setsockopt(zmq.SUBSCRIBE, 'Test')
while True:
    topic,msg = sock.recv().split()
    print msg