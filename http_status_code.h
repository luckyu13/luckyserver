#pragma once

enum StatusCode {
    // Information responses
    Continue = 100,
    SwitchingProtocol = 101,
    Processing = 102,
    EarlyHints = 103,

    // Successful responses
    OK = 200,
    Created = 201,
    Accepted = 202,
    NonAuthoritativeInformation = 203,
    NoContent = 204,
    ResetContent = 205,
    PartialContent = 206,
    MultiStatus = 207,
    AlreadyReported = 208,
    IMUsed = 226,

    // Redirection messages
    MultipleChoices = 300,
    MovedPermanently = 301,
    Found = 302,
    SeeOther = 303,
    NotModified = 304,
    UseProxy = 305,
    unused = 306,
    TemporaryRedirect = 307,
    PermanentRedirect = 308,

    // Client error responses
    BadRequest = 400,
    Unauthorized = 401,
    PaymentRequired = 402,
    Forbidden = 403,
    NotFound = 404,
    MethodNotAllowed = 405,
    NotAcceptable = 406,
    ProxyAuthenticationRequired = 407,
    RequestTimeout = 408,
    Conflict = 409,
    Gone = 410,
    LengthRequired = 411,
    PreconditionFailed = 412,
    PayloadTooLarge = 413,
    UriTooLong = 414,
    UnsupportedMediaType = 415,
    RangeNotSatisfiable = 416,
    ExpectationFailed = 417,
    ImATeapot = 418,
    MisdirectedRequest = 421,
    UnprocessableContent = 422,
    Locked = 423,
    FailedDependency = 424,
    TooEarly = 425,
    UpgradeRequired = 426,
    PreconditionRequired = 428,
    TooManyRequests = 429,
    RequestHeaderFieldsTooLarge = 431,
    UnavailableForLegalReasons = 451,

    // Server error responses
    InternalServerError = 500,
    NotImplemented = 501,
    BadGateway = 502,
    ServiceUnavailable = 503,
    GatewayTimeout = 504,
    HttpVersionNotSupported = 505,
    VariantAlsoNegotiates = 506,
    InsufficientStorage = 507,
    LoopDetected = 508,
    NotExtended = 510,
    NetworkAuthenticationRequired = 511,
};

static const char* status_message(int status) {
    switch (status) {
    case StatusCode::Continue: return "Continue";
    case StatusCode::SwitchingProtocol: return "Switching Protocol";
    case StatusCode::Processing: return "Processing";
    case StatusCode::EarlyHints: return "Early Hints";
    case StatusCode::OK: return "OK";
    case StatusCode::Created: return "Created";
    case StatusCode::Accepted: return "Accepted";
    case StatusCode::NonAuthoritativeInformation:
    return "Non-Authoritative Information";
    case StatusCode::NoContent: return "No Content";
    case StatusCode::ResetContent: return "Reset Content";
    case StatusCode::PartialContent: return "Partial Content";
    case StatusCode::MultiStatus: return "Multi-Status";
    case StatusCode::AlreadyReported: return "Already Reported";
    case StatusCode::IMUsed: return "IM Used";
    case StatusCode::MultipleChoices: return "Multiple Choices";
    case StatusCode::MovedPermanently: return "Moved Permanently";
    case StatusCode::Found: return "Found";
    case StatusCode::SeeOther: return "See Other";
    case StatusCode::NotModified: return "Not Modified";
    case StatusCode::UseProxy: return "Use Proxy";
    case StatusCode::unused: return "unused";
    case StatusCode::TemporaryRedirect: return "Temporary Redirect";
    case StatusCode::PermanentRedirect: return "Permanent Redirect";
    case StatusCode::BadRequest: return "Bad Request";
    case StatusCode::Unauthorized: return "Unauthorized";
    case StatusCode::PaymentRequired: return "Payment Required";
    case StatusCode::Forbidden: return "Forbidden";
    case StatusCode::NotFound: return "Not Found";
    case StatusCode::MethodNotAllowed: return "Method Not Allowed";
    case StatusCode::NotAcceptable: return "Not Acceptable";
    case StatusCode::ProxyAuthenticationRequired:
    return "Proxy Authentication Required";
    case StatusCode::RequestTimeout: return "Request Timeout";
    case StatusCode::Conflict: return "Conflict";
    case StatusCode::Gone: return "Gone";
    case StatusCode::LengthRequired: return "Length Required";
    case StatusCode::PreconditionFailed: return "Precondition Failed";
    case StatusCode::PayloadTooLarge: return "Payload Too Large";
    case StatusCode::UriTooLong: return "URI Too Long";
    case StatusCode::UnsupportedMediaType: return "Unsupported Media Type";
    case StatusCode::RangeNotSatisfiable: return "Range Not Satisfiable";
    case StatusCode::ExpectationFailed: return "Expectation Failed";
    case StatusCode::ImATeapot: return "I'm a teapot";
    case StatusCode::MisdirectedRequest: return "Misdirected Request";
    case StatusCode::UnprocessableContent: return "Unprocessable Content";
    case StatusCode::Locked: return "Locked";
    case StatusCode::FailedDependency: return "Failed Dependency";
    case StatusCode::TooEarly: return "Too Early";
    case StatusCode::UpgradeRequired: return "Upgrade Required";
    case StatusCode::PreconditionRequired: return "Precondition Required";
    case StatusCode::TooManyRequests: return "Too Many Requests";
    case StatusCode::RequestHeaderFieldsTooLarge:
    return "Request Header Fields Too Large";
    case StatusCode::UnavailableForLegalReasons:
    return "Unavailable For Legal Reasons";
    case StatusCode::NotImplemented: return "Not Implemented";
    case StatusCode::BadGateway: return "Bad Gateway";
    case StatusCode::ServiceUnavailable: return "Service Unavailable";
    case StatusCode::GatewayTimeout: return "Gateway Timeout";
    case StatusCode::HttpVersionNotSupported:
    return "HTTP Version Not Supported";
    case StatusCode::VariantAlsoNegotiates: return "Variant Also Negotiates";
    case StatusCode::InsufficientStorage: return "Insufficient Storage";
    case StatusCode::LoopDetected: return "Loop Detected";
    case StatusCode::NotExtended: return "Not Extended";
    case StatusCode::NetworkAuthenticationRequired:
    return "Network Authentication Required";

    default:
    case StatusCode::InternalServerError: return "Internal Server Error";
    }
}