setlocal
cd bin\

pushd release\
SET PATH=%PATH%;%CD%;
popd

cd ..\server
start SHO_LS.exe