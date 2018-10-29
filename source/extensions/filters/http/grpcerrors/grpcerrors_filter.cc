#include "extensions/filters/http/grpcerrors/grpcerrors_filter.h"

#include "common/config/well_known_names.h"
#include "common/grpc/common.h"
#include "common/http/headers.h"
#include "common/buffer/buffer_impl.h"

#include "extensions/filters/http/well_known_names.h"

namespace Envoy {
  namespace Extensions {
    namespace HttpFilters {
      namespace GrpcErrors {

        Config::Config(const envoy::config::filter::http::grpcerrors::v2::Config config) {
          if (config.error_key().length()) {
            error_key_ = config.error_key();
          }

          show_grpc_error_code_ = config.show_grpc_error_code();
        }

        GrpcErrorsFilter::GrpcErrorsFilter(const ConfigSharedPtr config) : config_(config) {}

        GrpcErrorsFilter::~GrpcErrorsFilter() {}

        void GrpcErrorsFilter::onDestroy() {}


        Http::FilterHeadersStatus GrpcErrorsFilter::encodeHeaders(Http::HeaderMap& headers, bool end_stream) {
          if (end_stream) {
            const Http::HeaderEntry* grpc_message_header = headers.GrpcMessage();
            const Http::HeaderEntry* grpc_status_code = headers.GrpcStatus();
            if (headers.Status()) {

              uint64_t http_status_code{};
              StringUtil::atoul(headers.Status()->value().c_str(), http_status_code);

              if (http_status_code != 200 &&
                  grpc_message_header) {
                ENVOY_LOG(trace, "found non-200 grpc response, copying message to body");

                std::string response = "{\n";
                response += "  \"";
                response += config_->error_key();
                response += "\": \"";
                response += grpc_message_header->value().c_str();
                response += "\"";
                if (config_->show_grpc_error_code() && grpc_status_code) {
                  response += ",\n  \"code\": ";
                  response += grpc_status_code->value().c_str();
                  response += "\n";

                } else {
                  response += "\n";
                }
                response += "}";

                Buffer::OwnedImpl buf;
                buf.add(response);
                encoder_callbacks_->addEncodedData(buf, false);

                headers.removeContentLength();
                headers.insertContentType().value().setReference(Http::Headers::get().ContentTypeValues.Json);
                headers.removeGrpcMessage();
                headers.removeGrpcStatus();

                return Http::FilterHeadersStatus::Continue;
              }
            }
            return Http::FilterHeadersStatus::Continue;
          }

          return Http::FilterHeadersStatus::Continue;
        }
      } // namespace GrpcErrors
    } // namespace HttpFilters
  } // namespace Extensions
} // namespace Envoy
