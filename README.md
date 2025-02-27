# rpc-assignment

RPC demonstration usin Unix sockets

# Running

This was written on Linux, so goodluck trying to get it to run on windows

You will need CMake to compile this. Run

```
mkdir build
cmake -B build
cmake --build build
```

Or you can use the CMake GUI

After this, you will have two executables in the build directory: `server` and `client`

Run `server` in the terminal, this will output the port address.

Then run `client` in a another terminal with the server port address

```
client <port>
```

This will send a test message to the server, at the moment.
