#ifndef NET_TARS_PRX_CALLBACK_COMPILER_JAVA_GENERATOR_H_
#define NET_TARS_PRX_CALLBACK_COMPILER_JAVA_GENERATOR_H_

#include <stdlib.h>  // for abort()
#include <iostream>
#include <string>

#include <google/protobuf/io/zero_copy_stream.h>
#include <google/protobuf/descriptor.h>

#ifndef NET_TARS_COMPILER_JAVA_GENERATOR_LOG_

class LogHelper {
    std::ostream *os;

public:
    LogHelper(std::ostream *os) : os(os) {}

    ~LogHelper() {
        *os << std::endl;
        ::abort();
    }

    std::ostream &get_os() {
        return *os;
    }
};

// Abort the program after logging the mesage if the given condition is not
// true. Otherwise, do nothing.
#define TARS_CODEGEN_CHECK(x) !(x) && LogHelper(&std::cerr).get_os() \
                             << "CHECK FAILED: " << __FILE__ << ":" \
                             << __LINE__ << ": "

// Abort the program after logging the mesage.
#define TARS_CODEGEN_FAIL TARS_CODEGEN_CHECK(false)
#define NET_TARS_COMPILER_JAVA_GENERATOR_LOG_
#endif //NET_TARS_COMPILER_JAVA_GENERATOR_LOG_

using namespace std;

namespace java_tars_prx_callback_generator {

    enum ProtoFlavor {
        NORMAL, LITE, NANO
    };

// Returns the package name of the tars prx services defined in the given file.
    string ServiceJavaPackage(const google::protobuf::FileDescriptor *file, bool nano);

// Returns the name of the outer class that wraps in all the generated code for
// the given service.
    string ServiceClassName(const google::protobuf::ServiceDescriptor *service);

// Writes the generated service interface into the given ZeroCopyOutputStream
    void GenerateService(const google::protobuf::ServiceDescriptor *service,
                         google::protobuf::io::ZeroCopyOutputStream *out,
                         ProtoFlavor flavor,
                         bool enable_deprecated);

}  // namespace java_tars_prx_generator

#endif  // NET_TARS_PRX_CALLBACK_COMPILER_JAVA_GENERATOR_H_
