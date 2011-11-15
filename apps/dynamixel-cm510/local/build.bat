cd ..\..\..\libASE\
make dummy

cd lib\dummy\
ranlib libASE.a

cd ..\..\..\apps\dynamixel-cm510\controllers\LuiTrainer
make all

cd ..\..\local
make all
