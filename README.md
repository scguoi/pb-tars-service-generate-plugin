tars Java Codegen Plugin for Protobuf Compiler
==============================================

This generates the Java interfaces out of the service definition from a
`.proto` file. It works with the Protobuf Compiler (``protoc``).

Normally you don't need to compile the codegen by yourself, since pre-compiled
binaries for common platforms are available on Maven Central. However, if the
pre-compiled binaries are not compatible with your system, you may want to
build your own codegen.

## System requirement

* Linux, Mac OS X with Clang, or Windows with MSYS2
* Java 7 or up
* [Protobuf](https://github.com/google/protobuf) 3.0.0-beta-3 or up

## Compiling and testing the codegen
### client

```
cd client
./gradlew java_pluginExecutable
```

### server

```
cd server
./gradlew java_pluginExecutable
```

## Useage

```
protobuf/src/.libs/protoc --plugin=protoc-gen-tars-java=compiler/server/build/exe/java_plugin/protoc-gen-tars-java --plugin=protoc-gen-tars-client-java=compiler/client/build/exe/java_plugin/protoc-gen-tars-client-java --tars-client-java_out=./gcode/ --tars-java_out=./gcode/ --java_out=./gcode/ Flight.proto
```

