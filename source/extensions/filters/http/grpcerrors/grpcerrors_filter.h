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

          std::string append() const { return append_; }

        private:
          std::string append_{"default"};
        };

        typedef std::shared_ptr<Config> ConfigSharedPtr;

        class GrpcErrorsFilter : public Http::StreamFilter,
                                 public Logger::Loggable<Logger::Id::filter> {
        public:
          GrpcErrorsFilter(const ConfigSharedPtr config);
          ~GrpcErrorsFilter();

          void onDestroy() override;

          // StreamDecoderFilter
          Http::FilterHeadersStatus decodeHeaders(Http::HeaderMap& headers, bool) override;
          Http::FilterDataStatus decodeData(Buffer::Instance&, bool) override;

          Http::FilterTrailersStatus decodeTrailers(Http::HeaderMap&) override;

          void setDecoderFilterCallbacks(Http::StreamDecoderFilterCallbacks& callbacks) override;

          // StreamEncoderFilter
          Http::FilterHeadersStatus encode100ContinueHeaders(Http::HeaderMap&) override {
            return Http::FilterHeadersStatus::Continue;
          }
          Http::FilterHeadersStatus encodeHeaders(Http::HeaderMap& headers, bool) override;
          Http::FilterDataStatus encodeData(Buffer::Instance&, bool) override;

          Http::FilterTrailersStatus encodeTrailers(Http::HeaderMap&) override;

          void setEncoderFilterCallbacks(Http::StreamEncoderFilterCallbacks& callbacks) override;


        private:
          const ConfigSharedPtr config_;
          Http::StreamDecoderFilterCallbacks* decoder_callbacks_{};
          Http::StreamEncoderFilterCallbacks* encoder_callbacks_{};
          Http::HeaderMap* response_headers_{nullptr};

        };

      } // namespace GrpcErrors
    } // namespace HttpFilters
  } // namespace Extensions
} // namespace Envoy
