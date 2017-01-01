#!/usr/bin/env python3
#-*- coding: utf-8 -*-

import os

from serial.serialutil import SerialException

from common.tcptalks    import TCPTalks
from common.serialtalks import SerialTalks

MODULECONNECT_OPCODE = 0x10
MODULEEXECUTE_OPCODE = 0x11


class ModulesRouter(TCPTalks):

	def __init__(self, password=None):
		TCPTalks.__init__(self, password=password)
		self.bind(MODULECONNECT_OPCODE, self.moduleconnect)
		self.bind(MODULEEXECUTE_OPCODE, self.moduleexecute)
		self.modules = dict()

	def moduleconnect(self, uuid, timeout):
		try:
			module = self.modules[uuid]
		except KeyError:
			module = SerialTalks(os.path.join('/dev/arduino', uuid))
			self.modules[uuid] = module
		
		if not module.is_connected:
			module.connect()

	def moduleexecute(self, uuid, methodname, *args, **kwargs):
		try:
			module = self.modules[uuid]
		except KeyError:
			raise RuntimeError('modules router has no module \'{}\''.format(uuid)) from None
			
		if not module.is_connected:
			module.connect()

		return getattr(module, methodname)(*args, **kwargs)


class Module:

	def __init__(self, parent, uuid, timeout = 2):
		self.parent = parent
		self.uuid   = uuid
		self.parent.execute(MODULECONNECT_OPCODE, uuid, timeout)

	def __getattr__(self, methodname):
		def method(*args, **kwargs):
			return self.parent.execute(MODULEEXECUTE_OPCODE, self.uuid, methodname, *args, **kwargs)
		return method