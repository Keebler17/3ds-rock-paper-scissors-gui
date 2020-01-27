make
:: cxitool rps.3dsx rps.cxi
:: makerom -f cia -icon icon.png -banner epic.cgfx -target t -i rps.cxi:0:0 -ignoresign
mv "rps.3dsx" "E:\3ds\graphics.3dsx"
mv "rps.cia" "E:\cias\graphics.cia"
3dslink -a 192.168.1.29 graphics.3dsx
pause