import socket
from networktables import NetworkTable 
import logging

port = 5000
s = socket.socket(socket.AF_INET,socket.SOCK_DGRAM)
s.bind(('',port))
logging.basicConfig(level=logging.DEBUG)
NetworkTable.setIPAddress("roborio-5431-frc.local")
NetworkTable.setClientMode()
NetworkTable.initialize()
sd = NetworkTable.getTable("SmartDashboard")
while 1:
	data,addr = s.recvfrom(1024)
	print ord(data)
	sd.putNumber('x',ord(data))
