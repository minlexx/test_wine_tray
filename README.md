# test_wine_tray
Simple test application to test tray icon in Wine

To build, you must have wine installed, and all its tools (winebuild, winemaker, winegcc)

### Checkout repo:
```
https://github.com/minlexx/test_wine_tray.git
```

### Makefile
If you want to generate Makefile yourself, use:
```
cd test_wine_tray
winemaker --wine32 .
```
or just skip this step.

### Compile:
```
make
```

### Run:
```
./winelib_test_tray.exe
```
