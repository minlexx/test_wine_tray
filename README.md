# test_wine_tray
Simple test application to test tray icon in Wine

To build, you must have wine installed, and all its tools (winebuild, winemaker, winegcc)

checkout repo:
```
https://github.com/minlexx/test_wine_tray.git
```

To generate Makefile, use:
```
cd test_wine_tray
winemaker --wine32 .
```

Compile:
```
make
```

Run:
```
./winelib_test_tray.exe
```
