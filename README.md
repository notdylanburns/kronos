# Kronos
Kronos is a simple socket HTTP server written in C.

## Installing `libkronos`

1) Download the source code, e.g. using Git like so: 
`git clone https://github.com/notdylanburns/kronos.git`.
2) Change into the root directory of the repository.
3) Run `make` followed by `sudo make install`.
4) Now you can symlink `/usr/local/lib/libkronos.so` to `/usr/lib/libkronos.so` by running the following as root:
`ln -s /usr/local/lib/libkronos.so /usr/lib/libkronos.so`
Alternatively, you can append `/usr/local/lib/libkronos.so` to your `$LD_LIBRARY_PATH` environment variable if you prefer.
5) `libkronos` is now installed globally.

## Removing `libkronos`

From the root directory of the code repository, run `make uninstall` as root. Then you can remove any symlinks you created by running the following:`unlink [symlink_path]`. Alternatively, you can remove the path to `libkronos.so` from your `$LD_LIBRARY_PATH` environment variable. After this, you can remove the downloaded source code if you wish.

## Using `libkronos`

Annotated example programs using the library can be found in the `examples` directory. They can be compiled and ran using `make [filename]` inside the folder (todo). When compiling your own files, be sure to link the required libraries using the compiler arguments:
 `-lm -lpthread -lkronos`
 i.e. Compiling `main.c`with `gcc` would be: 
 `gcc -o main -lm -lpthread -lkronos main.c`.
If you receive any errors pertaining to including headers, you can use the `-I` flag to specify the directory containing the `kronos` folder and its headers. 
