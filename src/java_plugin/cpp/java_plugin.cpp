// Generates Java tars service interface out of Protobuf IDL.
//
// This is a Proto2 compiler plugin.  See net/proto2/compiler/proto/plugin.proto
// and net/proto2/compiler/public/plugin.h for more information on plugins.

#include <memory>

#include "java_servant_generator.h"
#include "java_prx_generator.h"
#include "java_prx_callback_generator.h"
#include "java_codec_generator.h"
#include <google/protobuf/compiler/code_generator.h>
#include <google/protobuf/compiler/plugin.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/io/zero_copy_stream.h>

static string JavaPackageToDir(const string &package_name) {
    string package_dir = package_name;
    for (size_t i = 0; i < package_dir.size(); ++i) {
        if (package_dir[i] == '.') {
            package_dir[i] = '/';
        }
    }
    if (!package_dir.empty()) package_dir += "/";
    return package_dir;
}

class JavaTarsGenerator : public google::protobuf::compiler::CodeGenerator {
public:
    JavaTarsGenerator() {}

    virtual ~JavaTarsGenerator() {}

    virtual bool Generate(const google::protobuf::FileDescriptor *file,
                          const string &parameter,
                          google::protobuf::compiler::GeneratorContext *context,
                          string *error) const {
        std::vector<std::pair<string, string> > options;
        google::protobuf::compiler::ParseGeneratorParameter(parameter, &options);

        java_tars_servant_generator::ProtoFlavor flavor =
                java_tars_servant_generator::ProtoFlavor::NORMAL;

        java_tars_prx_generator::ProtoFlavor flavor_prx =
                java_tars_prx_generator::ProtoFlavor::NORMAL;

        java_tars_prx_callback_generator::ProtoFlavor flavor_prx_callback =
                java_tars_prx_callback_generator::ProtoFlavor::NORMAL;

        java_tars_codec_generator::ProtoFlavor flavor_codec =
                java_tars_codec_generator::ProtoFlavor::NORMAL;

        bool enable_deprecated = false;
        for (size_t i = 0; i < options.size(); i++) {
            std::cout << options[i].first << std::endl;
            std::cout << options[i].second << std::endl;
            if (options[i].first == "nano") {
                flavor = java_tars_servant_generator::ProtoFlavor::NANO;
                flavor_prx = java_tars_prx_generator::ProtoFlavor::NANO;
                flavor_prx_callback = java_tars_prx_callback_generator::ProtoFlavor::NANO;
                flavor_codec = java_tars_codec_generator::ProtoFlavor::NANO;
            } else if (options[i].first == "lite") {
                flavor = java_tars_servant_generator::ProtoFlavor::LITE;
                flavor_prx = java_tars_prx_generator::ProtoFlavor::LITE;
                flavor_prx_callback = java_tars_prx_callback_generator::ProtoFlavor::LITE;
                flavor_codec = java_tars_codec_generator::ProtoFlavor::LITE;
            } else if (options[i].first == "enable_deprecated") {
                enable_deprecated = options[i].second == "true";
            }
        }
        // servant
        {
            string package_name = java_tars_servant_generator::ServiceJavaPackage(
                    file, flavor == java_tars_servant_generator::ProtoFlavor::NANO);
            string package_filename = JavaPackageToDir(package_name);
            for (int i = 0; i < file->service_count(); ++i) {
                const google::protobuf::ServiceDescriptor *service = file->service(i);
                string filename = package_filename
                                  + java_tars_servant_generator::ServiceClassName(service) + ".java";
                std::unique_ptr<google::protobuf::io::ZeroCopyOutputStream> output(
                        context->Open(filename));
                java_tars_servant_generator::GenerateService(service, output.get(), flavor, enable_deprecated);
            }
        }
        // prx
        {
            string package_name = java_tars_prx_generator::ServiceJavaPackage(
                    file, flavor_prx == java_tars_prx_generator::ProtoFlavor::NANO);
            string package_filename = JavaPackageToDir(package_name);
            for (int i = 0; i < file->service_count(); ++i) {
                const google::protobuf::ServiceDescriptor *service = file->service(i);
                string filename = package_filename
                                  + java_tars_prx_generator::ServiceClassName(service) + ".java";
                std::unique_ptr<google::protobuf::io::ZeroCopyOutputStream> output(
                        context->Open(filename));
                java_tars_prx_generator::GenerateService(service, output.get(), flavor_prx, enable_deprecated);
            }
        }
        // callback
        {
            string package_name = java_tars_prx_callback_generator::ServiceJavaPackage(
                    file, flavor_prx_callback == java_tars_prx_callback_generator::ProtoFlavor::NANO);
            string package_filename = JavaPackageToDir(package_name);
            for (int i = 0; i < file->service_count(); ++i) {
                const google::protobuf::ServiceDescriptor *service = file->service(i);
                string filename = package_filename
                                  + java_tars_prx_callback_generator::ServiceClassName(service) + ".java";
                std::unique_ptr<google::protobuf::io::ZeroCopyOutputStream> output(
                        context->Open(filename));
                java_tars_prx_callback_generator::GenerateService(service, output.get(), flavor_prx_callback,
                                                                  enable_deprecated);
            }
        }
        // codec
        {
            string package_name = java_tars_codec_generator::ServiceJavaPackage(
                    file, flavor_prx == java_tars_codec_generator::ProtoFlavor::NANO);
            string package_filename = JavaPackageToDir(package_name);
            for (int i = 0; i < file->service_count(); ++i) {
                const google::protobuf::ServiceDescriptor *service = file->service(i);
                string filename = package_filename
                                  + java_tars_codec_generator::ServiceClassName(service) + ".java";
                std::unique_ptr<google::protobuf::io::ZeroCopyOutputStream> output(
                        context->Open(filename));
                java_tars_codec_generator::GenerateService(service, output.get(), flavor_codec, enable_deprecated);
            }
        }
        return true;
    }
};

int main(int argc, char *argv[]) {
    JavaTarsGenerator generator;
    return google::protobuf::compiler::PluginMain(argc, argv, &generator);
}
