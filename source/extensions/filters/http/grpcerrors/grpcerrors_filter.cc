#include "extensions/filters/http/grpcerrors/grpcerrors_filter.h"

#include "common/config/well_known_names.h"
#include "common/grpc/common.h"
#include "common/http/headers.h"
#include "common/buffer/buffer_impl.h"

// #include "common/protobuf/protobuf.h"

#include "extensions/filters/http/well_known_names.h"

namespace Envoy {
  namespace Extensions {
    namespace HttpFilters {
      namespace GrpcErrors {

        Config::Config(const envoy::config::filter::http::grpcerrors::v2::Config config) {
          append_ = config.append();
        }

        GrpcErrorsFilter::GrpcErrorsFilter(const ConfigSharedPtr config) : config_(config) {}

        GrpcErrorsFilter::~GrpcErrorsFilter() {}

        void GrpcErrorsFilter::onDestroy() {}


        Http::FilterHeadersStatus GrpcErrorsFilter::decodeHeaders(Http::HeaderMap& /*headers*/, bool end_stream) {
          ENVOY_LOG(trace, "decodeHeaders, end_stream = {}", end_stream);



          return Http::FilterHeadersStatus::Continue;
        }

        Http::FilterDataStatus GrpcErrorsFilter::decodeData(Buffer::Instance& data, bool end_stream) {
          ENVOY_LOG(trace, "decodeData, end_stream = {}", end_stream);
          ENVOY_LOG(trace, "buffer data: {}", data.toString());


          return Http::FilterDataStatus::Continue;
        }

        Http::FilterTrailersStatus GrpcErrorsFilter::decodeTrailers(Http::HeaderMap&) {
          ENVOY_LOG(trace, "decodeTrailers");
          return Http::FilterTrailersStatus::Continue;
        }

        void GrpcErrorsFilter::setDecoderFilterCallbacks(Http::StreamDecoderFilterCallbacks& callbacks) {
          decoder_callbacks_ = &callbacks;
        }

        Http::FilterHeadersStatus GrpcErrorsFilter::encodeHeaders(Http::HeaderMap& headers, bool end_stream) {
          ENVOY_LOG(trace, "encodeHeaders, end_stream = {}", end_stream);
          headers.removeContentLength();
          response_headers_ = &headers;
          if (end_stream) {
            const Http::HeaderEntry* grpc_message_header = response_headers_->GrpcMessage();
            ENVOY_LOG(trace, "1");
            if (response_headers_->Status()) {
              uint64_t status_code{};
              StringUtil::atoul(response_headers_->Status()->value().c_str(), status_code);
              ENVOY_LOG(trace, "http status: {}", response_headers_->Status()->value().c_str());
              ENVOY_LOG(trace, "2");
              if (status_code != 200 &&
                  grpc_message_header) {
                ENVOY_LOG(trace, "3");
                std::string response("{\"error\":\n  \"");
                response += grpc_message_header->value().c_str();
                response += "\"}";

                Buffer::OwnedImpl buf;
                buf.add(response);
                ENVOY_LOG(trace, "json buf: {}", buf.toString());
                ENVOY_LOG(trace, "buf len: {}", buf.length());
                encoder_callbacks_->addEncodedData(buf, false);
                response_headers_->insertContentType().value().setReference(Http::Headers::get().ContentTypeValues.Json);
                response_headers_->removeContentLength();
                response_headers_->removeGrpcMessage();
                response_headers_->removeGrpcStatus();

                response_headers_->addCopy(Http::LowerCaseString{"test"}, std::string{"header"});
                return Http::FilterHeadersStatus::Continue;
              }
            }
            ENVOY_LOG(trace, "4");
            return Http::FilterHeadersStatus::Continue;
          }

          return Http::FilterHeadersStatus::Continue;
        }

        Http::FilterDataStatus GrpcErrorsFilter::encodeData(Buffer::Instance& data, bool end_stream) {
          ENVOY_LOG(trace, "encodeData, end_stream = {}", end_stream);
          ENVOY_LOG(trace, "buffer data: {}",  data.toString());
          return Http::FilterDataStatus::Continue;
        }

        Http::FilterTrailersStatus GrpcErrorsFilter::encodeTrailers(Http::HeaderMap& /*trailers*/) {
          ENVOY_LOG(trace, "encodeTrailers");


          return Http::FilterTrailersStatus::Continue;
        }

        void GrpcErrorsFilter::setEncoderFilterCallbacks(Http::StreamEncoderFilterCallbacks& callbacks){
          encoder_callbacks_ = &callbacks;
        }

      } // namespace GrpcErrors
    } // namespace HttpFilters
  } // namespace Extensions
} // namespace Envoy
