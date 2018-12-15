echo "start to build"

echo "building XLCenterServer"

svn up

cd ./CenterServer
sh make.sh
make clean
make

cd ..
echo "building XLLoginGate"
cd ./LoginGate
sh make.sh
make clean
make

cd ..
echo "building XLLogicDB"
cd ./LogicDB
sh make.sh
make clean
make

cd ..
echo "building XLLogicManager"
cd ./LogicManager
sh make.sh
make clean
make

cd ..
echo "building XLLogicServer"
cd ./LogicServer
sh make.sh
make clean
make

cd ..
echo "building XLGateServer"
cd ./GateServer
sh make.sh
make clean
make

cd ..
echo "building XLMatchServer"
cd ./MatchServer
sh make.sh
make clean
make

cd ..
echo "building TeamServer"
cd ./TeamServer
sh make.sh
make clean
make

echo "building finished!!!"

cd ..

echo "please check creation date of all executable files"

ls -Rl|grep XL

