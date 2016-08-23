ICU_DIR=$1
ICU_SRC=${ICU_DIR}/source

echo "DIR: $ICU_DIR"
echo "SRC: $ICU_SRC"

cd $ICU_SRC
chmod +x runConfigureICU configure install-sh 
ls
CXXFLAGS='--std=c++0x -DU_USING_ICU_NAMESPACE=0 -DU_CHARSET_IS_UTF8=1' ./runConfigureICU Linux/gcc --prefix=${ICU_DIR}/build --enable-static
cd -
