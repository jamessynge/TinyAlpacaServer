#include "tiny_alpaca_server.h"

#include "alpaca_response.h"
#include "any_printable.h"
#include "array_view.h"
#include "constants.h"
#include "counting_print.h"
#include "http_response_header.h"
#include "json_encoder.h"
#include "json_encoder_helpers.h"
#include "literals.h"
#include "printable_cat.h"

namespace alpaca {

TinyAlpacaServerBase::TinyAlpacaServerBase(
    const ServerDescription& server_description,
    mcucore::ArrayView<DeviceInterface*> devices)
    : alpaca_devices_(devices),
      server_description_(server_description),
      server_transaction_id_(0) {}

bool TinyAlpacaServerBase::Initialize() { return alpaca_devices_.Initialize(); }

void TinyAlpacaServerBase::MaintainDevices() {
  alpaca_devices_.MaintainDevices();
}

void TinyAlpacaServerBase::OnStartDecoding(AlpacaRequest& request) {
  request.set_server_transaction_id(++server_transaction_id_);
}

bool TinyAlpacaServerBase::OnRequestDecoded(AlpacaRequest& request,
                                            Print& out) {
  switch (request.api) {
    case EAlpacaApi::kUnknown:
      break;

    case EAlpacaApi::kDeviceApi:
      MCU_FALLTHROUGH_INTENDED;
    case EAlpacaApi::kDeviceSetup:
      return alpaca_devices_.DispatchDeviceRequest(request, out);

    case EAlpacaApi::kManagementApiVersions:
      return HandleManagementApiVersions(request, out);

    case EAlpacaApi::kManagementDescription:
      return HandleManagementDescription(request, out);

    case EAlpacaApi::kManagementConfiguredDevices:
      return alpaca_devices_.HandleManagementConfiguredDevices(request, out);

    case EAlpacaApi::kServerSetup:
      return HandleServerSetup(request, out);
  }

  auto msg = MCU_PSV("OnRequestDecoded: unknown request.api=");
  MCU_VLOG(5) << msg << request.api;
  return WriteResponse::HttpErrorResponse(
      EHttpStatusCode::kHttpInternalServerError,
      mcucore::PrintableCat(msg, (request.api)), out);
}

void TinyAlpacaServerBase::OnRequestDecodingError(AlpacaRequest& request,
                                                  EHttpStatusCode status,
                                                  Print& out) {
  MCU_VLOG(3) << MCU_FLASHSTR("OnRequestDecodingError: status=") << status;
  WriteResponse::HttpErrorResponse(status, mcucore::AnyPrintable(), out);
}

bool TinyAlpacaServerBase::HandleManagementApiVersions(AlpacaRequest& request,
                                                       Print& out) {
  MCU_VLOG(3) << MCU_FLASHSTR("HandleManagementApiVersions");
  uint32_t versions[] = {1};
  return WriteResponse::ArrayResponse(
      request,
      MakeArrayViewSource(versions, &mcucore::JsonArrayEncoder::AddUIntElement),
      out);
}

bool TinyAlpacaServerBase::HandleManagementDescription(AlpacaRequest& request,
                                                       Print& out) {
  MCU_VLOG(3) << MCU_FLASHSTR(
      "TinyAlpacaServerBase::HandleManagementDescription");
  mcucore::JsonPropertySourceAdapter<ServerDescription> description(
      server_description_);
  return WriteResponse::ObjectResponse(request, description, out);
}

bool TinyAlpacaServerBase::HandleServerSetup(AlpacaRequest& request,
                                             Print& out) {
  MCU_VLOG(3) << MCU_FLASHSTR("HandleServerSetup");

  auto body = MCU_FLASHSTR(
      "<html><body>"
      "<h1>Tiny Alpaca Server</h1>\n"
      "No setup\n"
      "</body></html>");

  return WriteResponse::OkResponse(request, EContentType::kTextHtml,
                                   mcucore::AnyPrintable(body), out,
                                   /*append_http_newline=*/true);
}

TinyAlpacaServer::TinyAlpacaServer(uint16_t tcp_port,
                                   const ServerDescription& server_description,
                                   mcucore::ArrayView<DeviceInterface*> devices)
    : TinyAlpacaServerBase(server_description, devices),
      sockets_(tcp_port, *this),
      discovery_server_(tcp_port) {}

bool TinyAlpacaServer::Initialize() {
  // Give everything a chance to initialize so that logs will contain relevant
  // info about any failures. Choosing to initialize the discovery server before
  // the sockets so that it occupies socket 0.
  bool result = TinyAlpacaServerBase::Initialize();
  if (!discovery_server_.Initialize()) {
    result = false;
  }
  if (!sockets_.Initialize()) {
    result = false;
  }
  return result;
}

void TinyAlpacaServer::PerformIO() {
  TinyAlpacaServerBase::MaintainDevices();
  discovery_server_.PerformIO();
  sockets_.PerformIO();
}

}  // namespace alpaca
