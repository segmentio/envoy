#pragma once

#include <string>

#include "envoy/buffer/buffer.h"
#include "envoy/config/filter/http/grpcerrors/v2/grpcerrors.pb.h"
#include "envoy/server/filter_config.h"

#include "common/common/logger.h"

namespace Envoy {
  namespace Extensions {
    namespace HttpFilters {
      namespace GrpcErrors {

        class Config : public Logger::Loggable<Logger::Id::config> {
        public:
          Config(const envoy::config::filter::http::grpcerrors::v2::Config config);

          std::string error_key() const { return error_key_; }
          bool show_grpc_error_code() const { return show_grpc_error_code_; }

        private:
          std::string error_key_{"error"};
          bool show_grpc_error_code_{false};
        };

        typedef std::shared_ptr<Config> ConfigSharedPtr;

        class GrpcErrorsFilter : public Http::StreamEncoderFilter,
                                 public Logger::Loggable<Logger::Id::filter> {
        public:
          GrpcErrorsFilter(const ConfigSharedPtr config);
          ~GrpcErrorsFilter();

          void onDestroy() override;

          // StreamEncoderFilter
          Http::FilterHeadersStatus encode100ContinueHeaders(Http::HeaderMap&) override {
            return Http::FilterHeadersStatus::Continue;
          }
          Http::FilterDataStatus encodeData(Buffer::Instance&, bool) override {
            return Http::FilterDataStatus::Continue;
          }
          Http::FilterTrailersStatus encodeTrailers(Http::HeaderMap&) override {
            return Http::FilterTrailersStatus::Continue;
          }
          void setEncoderFilterCallbacks(Http::StreamEncoderFilterCallbacks& callbacks) override {
            encoder_callbacks_ = &callbacks;
          }

          Http::FilterHeadersStatus encodeHeaders(Http::HeaderMap& headers, bool) override;
        private:
          const ConfigSharedPtr config_;
          Http::StreamEncoderFilterCallbacks* encoder_callbacks_{};
        };

      } // namespace GrpcErrors
    } // namespace HttpFilters
  } // namespace Extensions
} // namespace Envoy
