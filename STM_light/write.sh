make clean
if
    make all 
then
    cd bin/stm32f4ve/;
    st-flash write pwmtest.bin 0x08000000
fi
