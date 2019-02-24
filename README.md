
## parun 
Parallel thread running 

### Build 
- git clone 
```
git clone https://github.com/cdecl/parun.git

# git submodile 
git submodule init
git submodule update
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
cat ../cmd.txt | ./parun -p 4 -v "sleep {}"
./parun -f ../cmd.txt -p 4 -v "sleep {}"
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
type ..\cmd.txt | parun.exe -p 4 -v "sleep {}"
parun.exe -f ..\cmd.txt -p 4 -v "sleep {}"
```


