#include "JavaCodecGenerator.h"

#include <algorithm>
#include <iostream>
#include <iterator>
#include <map>
#include <vector>
#include <google/protobuf/compiler/java/java_names.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/io/printer.h>
#include <google/protobuf/io/zero_copy_stream.h>

// Stringify helpers used solely to cast GRPC_VERSION
#ifndef STR
#define STR(s) #s
#endif

#ifndef XSTR
#define XSTR(s) STR(s)
#endif

#ifndef FALLTHROUGH_INTENDED
#define FALLTHROUGH_INTENDED
#endif

namespace JavaTarsCodecGenerator
{

    using google::protobuf::FileDescriptor;
    using google::protobuf::ServiceDescriptor;
    using google::protobuf::MethodDescriptor;
    using google::protobuf::Descriptor;
    using google::protobuf::io::Printer;
    using google::protobuf::SourceLocation;
    using std::to_string;

    /**
     * Adjust a method name prefix identifier to follow the JavaBean spec:
     * decapitalize the first letter
     * remove embedded underscores & capitalize the following letter
     */
    static string MixedLower(const string &word)
    {
        string w;
        w += tolower(word[0]);
        bool after_underscore = false;
        for (size_t i = 1; i < word.length(); ++i)
        {
            if (word[i] == '_')
            {
                after_underscore = true;
            }
            else
            {
                w += after_underscore ? toupper(word[i]) : word[i];
                after_underscore = false;
            }
        }
        return w;
    }

    /**
     * Converts to the identifier to the ALL_UPPER_CASE format.
     * An underscore is inserted where a lower case letter is followed by an
     * upper case letter.
     * All letters are converted to upper case
     */
    static string ToAllUpperCase(const string &word)
    {
        string w;
        for (size_t i = 0; i < word.length(); ++i)
        {
            w += toupper(word[i]);
            if ((i < word.length() - 1) && islower(word[i]) && isupper(word[i + 1]))
            {
                w += '_';
            }
        }
        return w;
    }

    static inline string LowerMethodName(const MethodDescriptor *method)
    {
        return MixedLower(method->name());
    }

    static inline string MethodPropertiesFieldName(const MethodDescriptor *method)
    {
        return "METHOD_" + ToAllUpperCase(method->name());
    }

    static inline string MethodIdFieldName(const MethodDescriptor *method)
    {
        return "METHODID_" + ToAllUpperCase(method->name());
    }

    static inline string MessageFullJavaName(bool nano, const Descriptor *desc)
    {
        string name = google::protobuf::compiler::java::ClassName(desc);
        if (nano)
        {
            if (isupper(name[0]))
            {
                // No java package specified.
                return "nano." + name;
            }
            for (size_t i = 0; i < name.size(); ++i)
            {
                if ((name[i] == '.') && (i < (name.size() - 1)) && isupper(name[i + 1]))
                {
                    return name.substr(0, i + 1) + "nano." + name.substr(i + 1);
                }
            }
        }
        return name;
    }

    static void TarsWriteFileDocComment(Printer *printer,
                                        const ServiceDescriptor *service)
    {
        printer->Print("/**\n");
        printer->Print(" * Tencent is pleased to support the open source community by making Tars available.\n"
                               " * Copyright (C) 2016 THL A29 Limited, a Tencent company. All rights reserved.\n"
                               " * Licensed under the BSD 3-Clause License (the \"License\"); you may not use this file\n"
                               " * except in compliance with the License. You may obtain a copy of the License at\n"
                               " * https://opensource.org/licenses/BSD-3-Clause\n"
                               " * Unless required by applicable law or agreed to in writing, software distributed\n"
                               " * under the License is distributed on an \"AS IS\" BASIS, WITHOUT WARRANTIES OR\n"
                               " * CONDITIONS OF ANY KIND, either express or implied. See the License for the\n"
                               " * specific language governing permissions and limitations under the License.\n");
        printer->Print(" */\n");
    }

    static void TarsWriteServiceDocComment(Printer *printer,
                                           const ServiceDescriptor *service)
    {
        printer->Print("/**\n");
        printer->Print(" */\n");
    }

    static void PrintService(const ServiceDescriptor *service,
                             std::map<string, string> *vars,
                             Printer *p,
                             ProtoFlavor flavor,
                             bool enable_deprecated)
    {
        (*vars)["service_name"] = service->name();
        (*vars)["file_name"] = service->file()->name();
        (*vars)["service_class_name"] = ServiceClassName(service);
        TarsWriteServiceDocComment(p, service);
        p->Print(*vars, "public class $service_class_name$ extends TarsCodec {\n");
        p->Indent();

        p->Print(*vars, "public $service_class_name$(String charsetName) {\n");
        p->Indent();
        p->Print("super(charsetName);\n");
        p->Outdent();
        p->Print("}\n\n");

        p->Print("@Override\n");
        p->Print("protected Object decodeParam(TarsInputStream inputStream, TarsMethodParameterInfo info, boolean require) throws ProtocolException {\n");
        p->Indent();
        p->Print("byte[] bytes = inputStream.read(TarsHelper.STAMP_BYTE_ARRAY, info.getOrder(), false);\n");
        p->Print("try {\n");
        p->Indent();
        p->Print("return ((Class) info.getStamp()).getMethod(\"parseFrom\", ByteString.class).invoke(null, ByteString.copyFrom(bytes));\n");
        p->Outdent();
        p->Print("} catch (Exception e) {\n");
        p->Indent();
        p->Print("throw new ProtocolException();\n");
        p->Outdent();
        p->Print("}\n");
        p->Outdent();
        p->Print("}\n\n");

        p->Print("public String getProtocol() {\n");
        p->Indent();
        p->Print("return Constants.PROTO_PROTOCOL;\n");
        p->Outdent();
        p->Print("}\n");

        p->Outdent();
        p->Print("}\n");
    }

    void PrintImports(Printer *p, bool generate_nano)
    {
        p->Print("import com.google.protobuf.ByteString;\n"
                         "import com.google.protobuf.GeneratedMessage;\n"
                         "import com.qq.tars.common.util.Constants;\n"
                         "import com.qq.tars.net.protocol.ProtocolException;\n"
                         "import com.qq.tars.protocol.tars.TarsInputStream;\n"
                         "import com.qq.tars.protocol.tars.TarsOutputStream;\n"
                         "import com.qq.tars.protocol.tars.support.TarsMethodParameterInfo;\n"
                         "import com.qq.tars.protocol.util.TarsHelper;\n"
                         "import com.qq.tars.rpc.protocol.tars.TarsCodec;\n\n");
        if (generate_nano)
        {
            p->Print("import java.io.IOException;\n\n");
        }
    }

    void GenerateService(const ServiceDescriptor *service,
                         google::protobuf::io::ZeroCopyOutputStream *out,
                         ProtoFlavor flavor,
                         bool enable_deprecated)
    {
        // All non-generated classes must be referred by fully qualified names to
        // avoid collision with generated classes.
        std::map<string, string> vars;
        vars["String"] = "java.lang.String";
        vars["Override"] = "java.lang.Override";

        Printer printer(out, '$');
        string package_name = ServiceJavaPackage(service->file(),
                                                 flavor == ProtoFlavor::NANO);
        TarsWriteFileDocComment(&printer, service);
        if (!package_name.empty())
        {
            printer.Print(
                    "package $package_name$;\n\n",
                    "package_name", package_name);
        }
        PrintImports(&printer, flavor == ProtoFlavor::NANO);

        // Package string is used to fully qualify method names.
        vars["Package"] = service->file()->package();
        if (!vars["Package"].empty())
        {
            vars["Package"].append(".");
        }
        PrintService(service, &vars, &printer, flavor, enable_deprecated);
    }

    string ServiceJavaPackage(const FileDescriptor *file, bool nano)
    {
        string result = google::protobuf::compiler::java::ClassName(file);
        size_t last_dot_pos = result.find_last_of('.');
        if (last_dot_pos != string::npos)
        {
            result.resize(last_dot_pos);
        }
        else
        {
            result = "";
        }
        if (nano)
        {
            if (!result.empty())
            {
                result += ".";
            }
            result += "nano";
        }
        return result;
    }

    string ServiceClassName(const google::protobuf::ServiceDescriptor *service)
    {
        return "ProtoCodec";
    }

}  // namespace JavaTarsCodecGenerator
