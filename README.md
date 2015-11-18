# test_wine_tray
Simple test application to test tray icon in Wine

To build, you must have wine installed, and all its tools (winebuild, winemaker, winegcc)

### Checkout repo:
```
git clone https://github.com/minlexx/test_wine_tray.git
cd test_wine_tray
```

### Generate Makefile (optional)
If you want to generate Makefile yourself, use:
```
winemaker --wine32 .
```

### Compile:
```
make
```

### Run:
```
WINEDEBUG=+systray ./winelib_test_tray.exe
```
