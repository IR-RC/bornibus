#!/usr/bin/python3
#-*- coding: utf-8 -*-

import serial
from queue		import Queue, Empty
from threading	import Thread, RLock, Event

from . import serialutils

BAUDRATE = 115200

MASTER_BYTE = b'R'
SLAVE_BYTE  = b'A'

GETUUID_OPCODE = 0x00
SETUUID_OPCODE = 0x01
STDOUT_OPCODE  = 0xFF
STDERR_OPCODE  = 0xFE

BYTEORDER = 'little'
ENCODING  = 'utf-8'

CHAR   = serialutils.IntegerType(1, BYTEORDER, True)
UCHAR  = serialutils.IntegerType(1, BYTEORDER, False)
SHORT  = serialutils.IntegerType(2, BYTEORDER, True)
USHORT = serialutils.IntegerType(2, BYTEORDER, False)
LONG   = serialutils.IntegerType(4, BYTEORDER, True)
ULONG  = serialutils.IntegerType(4, BYTEORDER, False)

FLOAT  = serialutils.FloatType('f')

STRING = serialutils.StringType(ENCODING)

BYTE   = UCHAR
INT    = SHORT
UINT   = USHORT
DOUBLE = FLOAT


class SerialTalks:

	def __init__(self, port):
		Thread.__init__(self)

		# Serial things
		self.port = port
		self.is_connected = False

		# Threading things
		self.queues_dict = dict()
		self.queues_lock = RLock()
		self.stop_event  = Event()

	def __enter__(self):
		self.connect()
		return self

	def __exit__(self, exc_type, exc_value, traceback):
		self.disconnect()

	def connect(self, timeout = 2):
		if not self.is_connected:
			self.stream = serial.Serial(self.port,
				baudrate=BAUDRATE,
				bytesize=serial.EIGHTBITS,
				parity=serial.PARITY_NONE,
				stopbits=serial.STOPBITS_ONE)
			self.stream.timeout	= 1
			self.is_connected = True
		
			# Create a listening thread that will wait for inputs
			self.stop_event.clear()
			self.listener = SerialListener(self)
			self.listener.start()

			# Wait until the Arduino is operational
			if self.getuuid(timeout) is None:
				self.disconnect()
				raise TimeoutError('\'{}\' is mute. It may not be an Arduino or it\'s sketch may not be correctly loaded.'.format(self.stream.port))
		
	def disconnect(self):
		if self.is_connected:
			# Stop the listening thread
			self.stop_event.set()
			self.listener.join()

			# Close the socket
			self.stream.close()
			self.is_connected = False

	def rawsend(self, rawbytes):
		if not self.is_connected:
			raise RuntimeError('\'{}\' is not connected.'.format(self.stream.port))
		
		sentbytes = self.stream.write(rawbytes)
		return sentbytes
	
	def send(self, opcode, *args):
		content = bytes([opcode]) + bytes().join(args)
		prefix  = MASTER_BYTE + bytes([len(content)])
		return self.rawsend(prefix + content)

	def get_queue(self, opcode):
		self.queues_lock.acquire()
		try:
			queue = self.queues_dict[opcode]
		except KeyError:
			queue = self.queues_dict[opcode] = Queue()
		finally:
			self.queues_lock.release()
		return queue

	def process(self, message):
		opcode = message[0]
		queue = self.get_queue(opcode)
		queue.put(message[1:])

	def poll(self, opcode, timeout = 0):
		queue = self.get_queue(opcode)
		block = (timeout is None or timeout > 0)
		try:
			rawbytes = queue.get(block, timeout)
			return serialutils.Deserializer(rawbytes)
		except Empty:
			return None
	
	def flush(self, opcode):
		while self.poll(opcode) is not None:
			pass

	def getuuid(self, timeout = None):
		self.flush(GETUUID_OPCODE)
		self.send(GETUUID_OPCODE)
		output = self.poll(GETUUID_OPCODE, timeout)
		if output is not None:
			return output.read(STRING)
		else:
			return None

	def setuuid(self, uuid):
		return self.send(SETUUID_OPCODE, STRING(uuid))

	def getlog(self, opcode, timeout = 0):
		log = str()
		while True:
			try:
				log += self.poll(opcode, timeout).read(STRING)
			except AttributeError:
				return log

	def getout(self, timeout = 0):
		return self.getlog(STDOUT_OPCODE, timeout)

	def geterr(self, timeout = 0):
		return self.getlog(STDERR_OPCODE, timeout)


class SerialListener(Thread):

	def __init__(self, parent):
		Thread.__init__(self)
		self.parent = parent
		self.daemon = True

	def run(self):
		state  = 'waiting' # ['waiting', 'starting', 'receiving']
		buffer = bytes()
		msglen = 0
		while not self.parent.stop_event.is_set():
			# Wait until new bytes arrive
			inc = self.parent.stream.read()

			# Finite state machine
			if state == 'waiting' and inc == SLAVE_BYTE :
				state = 'starting'
			
			elif state == 'starting':
				msglen = inc[0]
				state  = 'receiving'

			elif state == 'receiving':
				buffer += inc
				if (len(buffer) >= msglen):
					self.parent.process(buffer)
					buffer = bytes()
					state  = 'waiting'

if __name__ == '__main__':
	from pprint import pprint

	ard0 = SerialTalks('/dev/ttyUSB0')
	#ard1 = SerialTalks('/dev/ttyUSB1')
	with ard0:
		print('UUID is:', ard0.getuuid())
		print('stdout:', ard0.getout(), end = '')
		print('stderr:', ard0.geterr(), end = '')
		#print(ard1.getuuid())
