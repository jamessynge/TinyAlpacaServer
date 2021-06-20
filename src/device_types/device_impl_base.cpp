#include "device_types/device_impl_base.h"

#include "alpaca_response.h"
#include "ascom_error_codes.h"
#include "constants.h"
#include "device_info.h"
#include "http_response_header.h"
#include "literals.h"
#include "utils/counting_print.h"
#include "utils/json_encoder.h"
#include "utils/o_print_stream.h"
#include "utils/status.h"

namespace alpaca {
namespace {

class DeviceInfoHtml : public Printable {
 public:
  explicit DeviceInfoHtml(const DeviceInfo& info) : info_(info) {}

  size_t printTo(Print& out) const override {
    CountingPrint counter(out);
    OPrintStream strm(counter);
    strm << TASLIT(
                "<html><body>"
                "<h1>Tiny Alpaca Server Device Setup</h1>\n"
                "Type: ")
         << info_.device_type << TASLIT("<br>")  //
         << TASLIT("Number: ") << info_.device_number << TASLIT("<br>")
         << TASLIT("Name: ") << info_.name << TASLIT("<br>")
         << TASLIT("Description: ") << info_.description << TASLIT("<br>")
         << TASLIT("Unique ID: ") << info_.unique_id << TASLIT("<br>")
         << TASLIT("Driver Info: ") << info_.driver_info << TASLIT("<br>")
         << TASLIT("Driver Version: ") << info_.driver_version << TASLIT("<br>")
         << TASLIT("Interface Version: ") << info_.interface_version
         << TASLIT("<br>") << TASLIT("</body></html>");
    return counter.count();
  }

 private:
  const DeviceInfo& info_;
};

}  // namespace

bool DeviceImplBase::HandleDeviceSetupRequest(const AlpacaRequest& request,
                                              Print& out) {
  // Produce a default response indicating that there is no custom setup for
  // this device.
  DeviceInfoHtml html(device_info_);
  return WriteResponse::OkResponse(request, EContentType::kTextHtml, html, out,
                                   /*append_http_newline=*/true);
}

bool DeviceImplBase::HandleDeviceApiRequest(const AlpacaRequest& request,
                                            Print& out) {
  switch (request.http_method) {
    case EHttpMethod::GET:
    case EHttpMethod::HEAD:
      return HandleGetRequest(request, out);

    case EHttpMethod::PUT:
      return HandlePutRequest(request, out);

    case EHttpMethod::kUnknown:
      break;
  }

  // We shouldn't get here because we shouldn't have decoded an http_method not
  // explicitly listed above. So returning kHttpInternalServerError rather than
  // kHttpMethodNotImplemented, but using the HttpMethodNotImplemented reason
  // phrase.
  return WriteResponse::HttpErrorResponse(
      EHttpStatusCode::kHttpInternalServerError,
      Literals::HttpMethodNotImplemented(), out);
}

bool DeviceImplBase::HandleGetRequest(const AlpacaRequest& request,
                                      Print& out) {
  switch (request.device_method) {
    case EDeviceMethod::kConnected:
      return WriteResponse::StatusOrBoolResponse(request, GetConnected(), out);

    case EDeviceMethod::kDescription:
      return WriteResponse::AnyPrintableStringResponse(
          request, device_info_.description, out);

    case EDeviceMethod::kDriverInfo:
      return WriteResponse::AnyPrintableStringResponse(
          request, device_info_.driver_info, out);

    case EDeviceMethod::kDriverVersion:
      return WriteResponse::AnyPrintableStringResponse(
          request, device_info_.driver_version, out);

    case EDeviceMethod::kInterfaceVersion:
      return WriteResponse::IntResponse(request, device_info_.interface_version,
                                        out);

    case EDeviceMethod::kName:
      return WriteResponse::AnyPrintableStringResponse(request,
                                                       device_info_.name, out);

    case EDeviceMethod::kSupportedActions:
      return WriteResponse::LiteralArrayResponse(
          request, device_info_.supported_actions, out);

    default:
      return WriteResponse::AscomNotImplementedResponse(request, out);
  }
}

bool DeviceImplBase::HandlePutRequest(const AlpacaRequest& request,
                                      Print& out) {
  switch (request.device_method) {
    case EDeviceMethod::kAction:
      return HandlePutAction(request, out);

    case EDeviceMethod::kCommandBlind:
      return HandlePutCommandBlind(request, out);

    case EDeviceMethod::kCommandBool:
      return HandlePutCommandBool(request, out);

    case EDeviceMethod::kCommandString:
      return HandlePutCommandString(request, out);

    case EDeviceMethod::kConnected:
      return HandlePutConnected(request, out);

    default:
      return WriteResponse::AscomNotImplementedResponse(request, out);
  }
}

bool DeviceImplBase::HandlePutAction(const AlpacaRequest& request, Print& out) {
  // If there are NO supported actions, then we return MethodNotImplemented,
  // rather than ActionNotImplemented, which we return when there are some valid
  // actions, but the specified action name is not supported.
  if (device_info_.supported_actions.size == 0) {
    return WriteResponse::AscomNotImplementedResponse(request, out);
  }
  return WriteResponse::AscomErrorResponse(
      request, ErrorCodes::ActionNotImplemented(), out);
}

bool DeviceImplBase::HandlePutCommandBlind(const AlpacaRequest& request,
                                           Print& out) {
  return WriteResponse::AscomNotImplementedResponse(request, out);
}

bool DeviceImplBase::HandlePutCommandBool(const AlpacaRequest& request,
                                          Print& out) {
  return WriteResponse::AscomNotImplementedResponse(request, out);
}

bool DeviceImplBase::HandlePutCommandString(const AlpacaRequest& request,
                                            Print& out) {
  return WriteResponse::AscomNotImplementedResponse(request, out);
}

bool DeviceImplBase::HandlePutConnected(const AlpacaRequest& request,
                                        Print& out) {
  if (!request.have_connected) {
    return WriteResponse::AscomParameterMissingErrorResponse(
        request, Literals::Connected(), out);
  }
  return WriteResponse::StatusResponse(request, SetConnected(request.connected),
                                       out);
}

StatusOr<bool> DeviceImplBase::GetConnected() { return true; }

Status DeviceImplBase::SetConnected(bool value) { return OkStatus(); }

}  // namespace alpaca
