
## parun 
Parallel thread running (similar `xargs`)

### Build 
- git clone 
```
git clone https://github.com/cdecl/parun.git
```

- linux  
```
# Custom Makefile
make 
```
or  
```
mkdir build 
cd build
cmake .. 
make 
```
```
cat ../input.txt | ./parun -p 4 -v echo 
./parun -f ../input.txt -p 4 -v echo 
```

- Windows : Visual Studio
```
mkdir msbuild 
cd msbuild
cmake -G "Visual Studio 15 2017" ..
msbuild parun.sln /p:Configuration=Release /p:Platform=win32
```

- Windows : Mingw(Msys)
```
mkdir build 
cd build
cmake -G "MSYS Makefiles" ..
make	
```

```
type ..\input.txt | parun.exe -p 4 -v echo 
parun.exe -f ..\input.txt -p 4 -v echo 
```


