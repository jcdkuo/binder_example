# binder_example
This is an example of how to use Android IPC binders from native C++ code.
It is modified with reference to Gabriel Burca's article and source code.
The links are as follows.

# Reference
* [Understanding and implementing native Binders in android using C++](https://stackoverflow.com/questions/33522578/understanding-and-implementing-native-binders-in-android-using-c)
* [The Android IPC system](https://ebixio.com/blog/2011/01/03/the-android-ipc-system/)
* [Using Android IPC binders from native code](https://ebixio.com/blog/2012/07/07/using-android-ipc-binders-from-native-code/)

# How to build
## step 1
`git clone https://github.com/jcdkuo/binder_example.git` to android/externel folder

## step 2
Go to your android root folder, then run
```
source build/envsetup.sh
```

## step 3
```
lunch ${PRODUCT_NAME}

## step 4
Go to binder example folder (binder_demo_server/binder_demo_client), and run
```
make clean
make
```

## step 5
push exec file to rootfs "system"bin" folder, then you can exec it.

use `logcat | grep "binder" ` to see the log
