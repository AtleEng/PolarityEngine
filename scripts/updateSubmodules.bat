
pushd %~dp0\..\
call git submodule update --init --recursive
popd
PAUSE