cwd=$(pwd)
cd sensor/
pio run -t upload
pio run -t monitor
cd $cwd
