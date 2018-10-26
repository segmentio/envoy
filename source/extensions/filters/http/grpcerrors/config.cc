#include "extensions/filters/http/grpcerrors/config.h"

#include <string>

#include "envoy/config/filter/http/grpcerrors/v2/grpcerrors.pb.validate.h"
#include "envoy/registry/registry.h"
#include "common/protobuf/utility.h"
#include "extensions/filters/http/grpcerrors/grpcerrors_filter.h"

namespace Envoy {
namespace Extensions {
namespace HttpFilters {
namespace GrpcErrors {

Http::FilterFactoryCb GrpcErrorsFilterConfig::createFilterFactoryFromProtoTyped(
    const envoy::config::filter::http::grpcerrors::v2::Config& proto_config,
    const std::string&, Server::Configuration::FactoryContext&) {
  ConfigSharedPtr filter_config(std::make_shared<Config>(proto_config));

  return [filter_config](Http::FilterChainFactoryCallbacks& callbacks) -> void {
    callbacks.addStreamFilter(
        Http::StreamFilterSharedPtr{new GrpcErrorsFilter(filter_config)});
  };
}

/**
 * Static registration for the header-to-metadata filter. @see RegisterFactory.
 */
static Registry::RegisterFactory<GrpcErrorsFilterConfig,
                                 Server::Configuration::NamedHttpFilterConfigFactory>
    register_;

} // namespace GrpcErrors
} // namespace HttpFilters
} // namespace Extensions
} // namespace Envoy
