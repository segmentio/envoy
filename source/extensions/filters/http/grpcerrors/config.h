#pragma once

#include "envoy/config/filter/http/grpcerrors/v2/grpcerrors.pb.h"

#include "extensions/filters/http/common/factory_base.h"
#include "extensions/filters/http/well_known_names.h"

namespace Envoy {
namespace Extensions {
namespace HttpFilters {
namespace GrpcErrors {
class GrpcErrorsFilterConfig
    : public Common::FactoryBase<envoy::config::filter::http::grpcerrors::v2::Config> {
public:
  GrpcErrorsFilterConfig() : FactoryBase(HttpFilterNames::get().GrpcErrors) {}

private:
  Http::FilterFactoryCb createFilterFactoryFromProtoTyped(
      const envoy::config::filter::http::grpcerrors::v2::Config& proto_config,
      const std::string& stats_prefix, Server::Configuration::FactoryContext& context) override;
};
} // namespace GrpcErrors
} // namespace HttpFilters
} // namespace Extensions
} // namespace Envoy
