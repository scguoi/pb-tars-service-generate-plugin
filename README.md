tars Java Codegen Plugin for Protobuf
==============================================

插件可以和`protoc`一起生成`.proto`文件定义的接口；

需要自己编译；

**插件编译需要依赖Protobuf**

```shell
export PROTOBUF_HOME='protobuf安装目录'
export CXXFLAGS='-I$PROTOBUF_HOME/src" LDFLAGS="-L$PROTOBUF_HOME/src/.libs'
```

## System requirement

* Linux、 Mac OS X with Clang
* Java 7 or up
* [Protobuf](https://github.com/google/protobuf) 3.0.0-beta-3 or up

## Compiling and testing the codegen
[protobuf](https://github.com/google/protobuf)的编译

```shell
git clone https://github.com/google/protobuf.git
./autogen.sh
./configure
make
export PROTOBUF_HOME=`pwd`
```

插件的编译

```shell
git clone git@github.com:scguoi/pb-tars-service-generate-plugin.git
cd pb-tars-service-generate-plugin
export CXXFLAGS="-I$PROTOBUF_HOME/src" LDFLAGS="-L$PROTOBUF_HOME/src/.libs"
./gradlew java_pluginExecutable
```

## Useage

### Mac

```shell
export DYLD_LIBRARY_PATH=$DYLD_LIBRARY_PATH:$PROTOBUF_HOME/src/.libs
cd test
$PROTOBUF_HOME/src/.libs/protoc --plugin=protoc-gen-tars-java=../build/exe/java_plugin/protoc-gen-tars-java --tars-java_out=./ --java_out=./ Flight.proto
```

查看生成结果:

```
➜  test git:(master) ll com/iflytek/grpc/flight/
total 92K
-rw-r--r-- 1 scguo staff  79K  9 20 19:28 Flight.java
-rw-r--r-- 1 scguo staff  749  9 20 19:28 FlightServicePrx.java
-rw-r--r-- 1 scguo staff  490  9 20 19:28 FlightServiceServant.java
-rw-r--r-- 1 scguo staff 1.8K  9 20 19:28 ProtoCodec.java
```

### Linux

```shell
export LD_LIBRARY_PATH="$PROTOBUF_HOME/src/.libs"
cd test
$PROTOBUF_HOME/src/.libs/protoc --plugin=protoc-gen-tars-java=../build/exe/java_plugin/protoc-gen-tars-java --tars-java_out=./ --java_out=./ Flight.proto
```

产看生成的结果:

```shell
H0045170 : ➜  test  l com/iflytek/grpc/flight/
总用量 92K
drwxr-xr-x 2 root root  106 9月  21 08:27 .
drwxr-xr-x 3 root root   19 9月  21 08:27 ..
-rw-r--r-- 1 root root  80K 9月  21 08:27 Flight.java
-rw-r--r-- 1 root root  749 9月  21 08:27 FlightServicePrx.java
-rw-r--r-- 1 root root  490 9月  21 08:27 FlightServiceServant.java
-rw-r--r-- 1 root root 1.8K 9月  21 08:27 ProtoCodec.java
```

## 可能的问题

1、编译找不到-lstdc++

```shell
/bin/ld: cannot find -lstdc++
collect2: 错误：ld 返回 1
```

```shell
yum install libstdc++-static.x86_64
```