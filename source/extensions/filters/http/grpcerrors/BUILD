licenses(["notice"])  # Apache 2

load(
    "//bazel:envoy_build_system.bzl",
    "envoy_cc_library",
    "envoy_package",
)

envoy_package()

envoy_cc_library(
    name = "grpcerrors_filter_lib",
    srcs = ["grpcerrors_filter.cc"],
    hdrs = ["grpcerrors_filter.h"],
    deps = [
        "//include/envoy/server:filter_config_interface",
        "//source/common/grpc:common_lib",
        "//source/common/http:headers_lib",
        "//source/extensions/filters/http:well_known_names",
        "@envoy_api//envoy/config/filter/http/grpcerrors/v2:grpcerrors_cc",
    ],
)

envoy_cc_library(
    name = "config",
    srcs = ["config.cc"],
    hdrs = ["config.h"],
    deps = [
        "//include/envoy/registry",
        "//source/common/protobuf:utility_lib",
        "//source/extensions/filters/http:well_known_names",
        "//source/extensions/filters/http/common:factory_base_lib",
        "//source/extensions/filters/http/grpcerrors:grpcerrors_filter_lib",
    ],
)
