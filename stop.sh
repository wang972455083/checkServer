#!/bin/sh


echo "stop CenterServer"
pid=`ps -ef|grep XLCenterServer|grep -v grep|awk '{print $2}'`
if [ "$pid" == "0" ]; then
	echo "CenterServer not start"
else
	kill -9 $pid
fi

sleep 1

echo "stop LogicDB" 
pid=`ps -ef|grep XLLogicDB|grep -v grep|awk '{print $2}'`
if [ "$pid" == "0" ]; then
	echo "LogicDB not start"
else
	kill -9 $pid
fi



sleep 1

echo "stop LogicManager" 
pid=`ps -ef|grep XLLogicManager|grep -v grep|awk '{print $2}'`
if [ "$pid" == "0" ]; then
	echo "LogicManager not start"
else
	kill -9 $pid
fi



sleep 1

echo "stop LogicServer" 
pid=`ps -ef|grep XLLogicServer|grep -v grep|awk '{print $2}'`
if [ "$pid" == "0" ]; then
	echo "LoigcServer not start"
else
	kill -9 $pid
fi



sleep 1

echo "stop GateServer" 
pid=`ps -ef|grep XLGateServer|grep -v grep|awk '{print $2}'`
if [ "$pid" == "0" ]; then
	echo "GateServer not start"
else
	kill -9 $pid
fi



sleep 1

echo "stop LoginGate" 
pid=`ps -ef|grep XLLoginGate|grep -v grep|awk '{print $2}'`
if [ "$pid" == "0" ]; then
	echo "LoginGate not start"
else
	kill -9 $pid
fi



sleep 1


echo "kill all server OK"
