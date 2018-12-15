#!/bin/sh
cd /opt/CardServer/
ulimit -c unlimited

echo "start CenterServer"
cd /opt/CardServer/CenterServer
./XLCenterServer


echo "start LogicDB"
sleep 1
cd /opt/Cardserver/LogicDB
./XLLogicDB



echo "start LogicManager"
cd /opt/CardServer/LogicManager
./XLLogicManager
sleep 1



echo "start LogicServer"
cd /opt/CardServer/LogicServer
./XLLogicServer
sleep 1


echo "start GateServer"
cd /opt/CardServer/GateServer
./XLGateServer
sleep 1

echo "start GateServer"
cd /opt/CardServer/LoginGate
./XLLoginGate
sleep 1


ps -ef|grep XL|grep -v grep

