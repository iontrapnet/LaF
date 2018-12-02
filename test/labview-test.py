import signal
import zmq
import labview

signal.signal(signal.SIGINT, signal.SIG_DFL);

ctx=zmq.Context()
sock = ctx.socket(zmq.SUB)
sock.connect('tcp://localhost:10000')
sock.setsockopt(zmq.SUBSCRIBE, 'Test')
while True:
    topic,msg = sock.recv().split()
    ctrl = int(msg)
    print(labview.CtrlGetValue(ctrl))