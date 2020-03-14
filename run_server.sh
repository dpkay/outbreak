cp -f src/web/* gen
cp -f contrib/d3/* gen
cp -f contrib/resetcss/* gen
cd gen
python -m SimpleHTTPServer
