# Case-Loan

## For init på MacOS:
- Installer både crow og asio med brew:
```
brew install crow asio
```

- Init prosjektet (oppdateringer også):
```
g++ private/main.cpp -o main -I/opt/homebrew/opt/crow/include -I/opt/homebrew/include -std=c++17 -lsqlite3
```

## For å kjøre programmet på MacOS:
```
./main
```

## For init og kjøring på Windows:
[Using GCC with MinGW](https://code.visualstudio.com/docs/cpp/config-mingw)
