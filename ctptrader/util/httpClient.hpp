#pragma once

#include <optional>
#include <string>
#include <unordered_map>

#include <curl/curl.h>
#include <fmt/format.h>
#include <rapidjson/document.h>

namespace ctptrader::util {

struct HttpResponse {
  CURLcode code_;
  std::string buffer_;

  std::optional<std::string> Text() {
    return code_ == CURLE_OK ? std::optional<std::string>(buffer_)
                             : std::nullopt;
  }

  std::optional<rapidjson::Document> Json() {
    if (code_ != CURLE_OK) {
      return std::nullopt;
    }
    rapidjson::Document doc;
    doc.Parse(buffer_.c_str());
    if (doc.HasParseError()) {
      return std::nullopt;
    }
    return doc;
  }
  static size_t Callback(void *ptr, size_t size, size_t nmemb,
                         std::string *data) {
    data->append((char *)ptr, size * nmemb);
    return size * nmemb;
  }
};

struct HttpRequest {
  std::unordered_map<std::string, std::string> headers_;
  std::optional<std::string> body_;
  CURL *curl_;

  HttpRequest &Header(const std::string &key, const std::string &value) {
    headers_[key] = value;
    return *this;
  }

  HttpRequest &Body(const std::string &body) {
    body_ = body;
    return *this;
  }

  HttpResponse Get(const std::string &url) {
    HttpResponse res;
    return res;
  }

  HttpResponse Post(const std::string &url) {
    HttpResponse res;

    curl_easy_setopt(curl_, CURLOPT_POST, 1L);
    curl_easy_setopt(curl_, CURLOPT_URL, url.c_str());
    struct curl_slist *headers = NULL;
    for (auto &[key, value] : headers_) {
      auto header = fmt::format("{}: {}", key, value);
      headers = curl_slist_append(headers, header.c_str());
    }
    curl_easy_setopt(curl_, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl_, CURLOPT_POSTFIELDS, body_.value().c_str());
    curl_easy_setopt(curl_, CURLOPT_WRITEFUNCTION, HttpResponse::Callback);
    curl_easy_setopt(curl_, CURLOPT_WRITEDATA, (void *)&res.buffer_);
    res.code_ = curl_easy_perform(curl_);
    curl_slist_free_all(headers);
    return res;
  }
};

class HttpClient {
public:
  HttpClient() : curl_(curl_easy_init()) {}
  ~HttpClient() { curl_easy_cleanup(curl_); }

  HttpRequest Request() {
    HttpRequest req;
    req.curl_ = this->curl_;
    return req;
  }

public:
  CURL *curl_;
};

} // namespace ctptrader::util