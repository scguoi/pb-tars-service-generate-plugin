tars Java Codegen Plugin for Protobuf
==============================================

插件可以和`protoc`一起生成`.proto`文件定义的接口；

需要自己编译；

**插件编译需要依赖Protobuf**

```shell
export PROTOBUF_HOME="protobuf安装目录"
export CXXFLAGS="-I$PROTOBUF_HOME/src" LDFLAGS="-L$PROTOBUF_HOME/src/.libs"
```

## System requirement

* Linux、 Mac OS X with Clang、 Windows with MSYS2
* Java 7 or up
* [Protobuf](https://github.com/google/protobuf) 3.0.0-beta-3 or up

## Compiling and testing the codegen
protobuf的编译[参见](https://github.com/google/protobuf);

```

```



插件的编译

```shell
git clone git@github.com:scguoi/pb-tars-service-generate-plugin.git
cd pb-tars-service-generate-plugin
export CXXFLAGS="-I$PROTOBUF_HOME/src" LDFLAGS="-L$PROTOBUF_HOME/src/.libs"
./gradlew java_pluginExecutable
```

## Useage

mac

```shell
export DYLD_LIBRARY_PATH=$DYLD_LIBRARY_PATH:$PROTOBUF_HOME/src/.libs
cd test
$PROTOBUF_HOME/src/.libs/protoc --plugin=protoc-gen-tars-java=../build/exe/java_plugin/protoc-gen-tars-java --tars-java_out=./ --java_out=./ Flight.proto
```

linux

```

```

