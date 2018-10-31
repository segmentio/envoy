#include "common/buffer/buffer_impl.h"
#include "common/http/header_map_impl.h"
#include "common/protobuf/protobuf.h"

#include "extensions/filters/http/grpcerrors/grpcerrors_filter.h"


#include "test/mocks/http/mocks.h"
#include "test/test_common/utility.h"

#include "gmock/gmock.h"
#include "gtest/gtest.h"

using testing::_;
using testing::Invoke;
using testing::NiceMock;

namespace Envoy {
namespace Extensions {
namespace HttpFilters {
namespace GrpcErrors {

class GrpcErrorsFilterTest : public testing::Test {
public:
  GrpcErrorsFilterTest() {}

  const std::string config_yaml = R"EOF(
error_key: "message"
show_grpc_error_code: true
)EOF";

  void initializeFilter(const std::string& yaml) {
    envoy::config::filter::http::grpcerrors::v2::Config config;
    MessageUtil::loadFromYaml(yaml, config);
    config_.reset(new Config(config));
    filter_.reset(new GrpcErrorsFilter(config_));
    filter_->setEncoderFilterCallbacks(encoder_callbacks_);
  }

  ConfigSharedPtr config_;
  std::shared_ptr<GrpcErrorsFilter> filter_;
  NiceMock<Http::MockStreamEncoderFilterCallbacks> encoder_callbacks_;
};

// Test that a normal response is not modified by this filter
TEST_F(GrpcErrorsFilterTest, IgnoreRequestTest) {
  initializeFilter(config_yaml);

  Http::TestHeaderMapImpl response_headers{{"content-type", "application/grpc"},
                                           {":status", "200"}};

  Http::TestHeaderMapImpl expected_response_headers{{"content-type", "application/grpc"},
                                                    {":status", "200"}};

  EXPECT_EQ(Http::FilterHeadersStatus::Continue, filter_->encodeHeaders(response_headers, false));
  EXPECT_EQ(expected_response_headers, response_headers);

  Buffer::OwnedImpl response_data{"{}"};
  EXPECT_EQ(Http::FilterDataStatus::Continue, filter_->encodeData(response_data, false));
  EXPECT_EQ(2, response_data.length());

  Http::TestHeaderMapImpl response_trailers{{"grpc-status", "0"}};
  Http::TestHeaderMapImpl expected_response_trailers{{"grpc-status", "0"}};
  EXPECT_EQ(Http::FilterTrailersStatus::Continue, filter_->encodeTrailers(response_trailers));
  EXPECT_EQ(expected_response_trailers, response_trailers);
}

TEST_F(GrpcErrorsFilterTest, ErrorInBody) {
  initializeFilter(config_yaml);

  Http::TestHeaderMapImpl response_headers{{"content-type", "application/grpc"},
                                           {"grpc-status", "3"},
                                           {"grpc-message", "failed validation"},
                                           {":status", "400"}};

  Http::TestHeaderMapImpl expected_response_headers{{"content-type", "application/json"},
                                                    {":status", "400"}};

  Buffer::OwnedImpl response_data;
  EXPECT_CALL(encoder_callbacks_, addEncodedData(_, false))
      .WillOnce(Invoke([&](Buffer::Instance& data, bool) { response_data.move(data); }));

  EXPECT_EQ(Http::FilterHeadersStatus::Continue, filter_->encodeHeaders(response_headers, true));
  EXPECT_EQ(expected_response_headers, response_headers);

  Buffer::OwnedImpl expected_response_data{
      "{\n  \"message\": \"failed validation\",\n  \"code\": 3\n}"};

  EXPECT_EQ(expected_response_data.toString(), response_data.toString());
}

TEST_F(GrpcErrorsFilterTest, DefaultConfig) {
  const std::string& emptyConfig{"{}"};
  initializeFilter(emptyConfig);

  Http::TestHeaderMapImpl response_headers{{"content-type", "application/grpc"},
                                           {"grpc-status", "3"},
                                           {"grpc-message", "failed validation"},
                                           {":status", "400"}};

  Http::TestHeaderMapImpl expected_response_headers{{"content-type", "application/json"},
                                                    {":status", "400"}};

  Buffer::OwnedImpl response_data;
  EXPECT_CALL(encoder_callbacks_, addEncodedData(_, false))
      .WillOnce(Invoke([&](Buffer::Instance& data, bool) { response_data.move(data); }));

  EXPECT_EQ(Http::FilterHeadersStatus::Continue, filter_->encodeHeaders(response_headers, true));
  EXPECT_EQ(expected_response_headers, response_headers);

  Buffer::OwnedImpl expected_response_data{"{\n  \"error\": \"failed validation\"\n}"};

  EXPECT_EQ(expected_response_data.toString(), response_data.toString());
}
} // namespace GrpcErrors
} // namespace HttpFilters
} // namespace Extensions
} // namespace Envoy
