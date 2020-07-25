# G'MIC for Android

## Building
NOTE: May be incomplete
Tested on Ubuntu GNU/Linux 20.04 amd64 and Google Pixel 4 arm64-v8a

### Requests
- GNU/Linux
- OpenJDK 8
- Git
- Android SDK
- Android NDK r17c(because GCC isn't supported by r18b)
- Qt5 and Qt Creator
  NOTE: If you're using Ubuntu, don't use Qt from apt (because the build fails).
  For your reference: I used the official open source version of Qt5, used offline installer. ([It's recommended to change the installer settings](https://superuser.com/questions/1524977/cant-skip-login-in-qt-installer).)

### How to build
1. Setup environment variables: `PATH, ANDROID_HOME, ANDROID_SDK_ROOT, NDK_ROOT`
2. Clone repository: `git clone https://github.com/nnn1590/gmic-qt.git -b android-port --recursive`
3. Run `(gmic-qt)/external/android/download.sh`
4. Build `(gmic-qt)/external/android/*`  (See [#Buiding-external-libraries](#Buiding-external-libraries))
5. Open `gmic-qt` with Qt Creator
6. Setup Qt Creator Android settings  (See [#Setup Qt Creator](#Setup-Qt-Creator))
7. Setup kit
   Select/click Projects(left tab) -> Build & Run -> Android for * (with plus icon)
8. Build/Run
9. Finish
```bash
# Like this

export ANDROID_HOME="/path/to/android-sdk"
export ANDROID_SDK_ROOT="${ANDROID_HOME}"
export NDK_ROOT="/path/to/android-ndk-r17c"
export PATH="${ANDROID_HOME}/tools:${ANDROID_HOME}/platform-tools:${NDK_ROOT}:${PATH}"

git clone https://github.com/nnn1590/gmic-qt.git -b android-port --recursive
cd gmic-qt
./external/android/download.sh

# Build external libraries at this point, then open Qt Creator.
```

## Buiding external libraries
### fftw-3.8.8
```bash
cd external/android/fftw-3.8.8
./bootstrap.sh; APP_PLATFORM=android-19 NDK_ARCH=arch-amd64 ANDROID_NDK_HOME="${NDK_ROOT}" bash jni/configure.sh  # Binaries for arm64-v8a, armeabi-v7a, x86, x86_64 seem to be built regardless of the value of ${NDK_ARCH}
```

### curl-android-ios
```bash
cd external/android/curl-android-ios/curl-compile-scripts
./build_Android.sh  # If failed, run `git submodule update --init --recursive` and retry.
```

### libpng-android
```bash
cd external/android/libpng-android
./build.sh
```

## Setup Qt Creator
1. Open Tools(menu bar) -> Options -> Device -> Android
2. Fill JDK, SDK, NDK, OpenSSL paths
   NOTE: OpenSSL can use [this](https://github.com/KDAB/android_openssl). Just clone it and set path to it in Qt Creator.
3. Press OK

## Known issues
- Most filters crash
- GUI doesn't fit on small display
- Does not work except `HOST=none` (so you can't run from Krita for Android. Probably should be native binary instead of APK.)
