#include "tiny_alpaca_server.h"

#include <McuCore.h>

#include "alpaca_response.h"
#include "constants.h"
#include "http_response_header.h"
#include "literals.h"

namespace alpaca {

TinyAlpacaDeviceServer::TinyAlpacaDeviceServer(
    const ServerDescription& server_description,
    mcucore::ArrayView<DeviceInterface*> devices)
    : alpaca_devices_(devices),
      server_description_(server_description),
      server_transaction_id_(0) {}

bool TinyAlpacaDeviceServer::Initialize() {
  return alpaca_devices_.Initialize();
}

void TinyAlpacaDeviceServer::MaintainDevices() {
  alpaca_devices_.MaintainDevices();
}

void TinyAlpacaDeviceServer::OnStartDecoding(AlpacaRequest& request) {
  request.set_server_transaction_id(++server_transaction_id_);
}

bool TinyAlpacaDeviceServer::OnRequestDecoded(AlpacaRequest& request,
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

void TinyAlpacaDeviceServer::OnRequestDecodingError(AlpacaRequest& request,
                                                    EHttpStatusCode status,
                                                    Print& out) {
  MCU_VLOG(3) << MCU_FLASHSTR("OnRequestDecodingError: status=") << status;
  WriteResponse::HttpErrorResponse(status, mcucore::AnyPrintable(), out);
}

bool TinyAlpacaDeviceServer::HandleManagementApiVersions(AlpacaRequest& request,
                                                         Print& out) {
  MCU_VLOG(3) << MCU_FLASHSTR("HandleManagementApiVersions");
  uint32_t versions[] = {1};
  return WriteResponse::ArrayResponse(
      request,
      MakeArrayViewSource(versions, &mcucore::JsonArrayEncoder::AddUIntElement),
      out);
}

bool TinyAlpacaDeviceServer::HandleManagementDescription(AlpacaRequest& request,
                                                         Print& out) {
  MCU_VLOG(3) << MCU_FLASHSTR(
      "TinyAlpacaDeviceServer::HandleManagementDescription");
  mcucore::JsonPropertySourceAdapter<ServerDescription> description(
      server_description_);
  return WriteResponse::ObjectResponse(request, description, out);
}

bool TinyAlpacaDeviceServer::HandleServerSetup(AlpacaRequest& request,
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

TinyAlpacaNetworkServer::TinyAlpacaNetworkServer(
    TinyAlpacaDeviceServer& device_server, uint16_t tcp_port)
    : sockets_(tcp_port, device_server), discovery_server_(tcp_port) {}

bool TinyAlpacaNetworkServer::Initialize() {
  // Give everything a chance to initialize so that logs will contain relevant
  // info about any failures. Choosing to initialize the discovery server before
  // the sockets so that it occupies socket 0.
  bool result = discovery_server_.Initialize();
  if (!sockets_.Initialize()) {
    result = false;
  }
  return result;
}

void TinyAlpacaNetworkServer::PerformIO() {
  discovery_server_.PerformIO();
  sockets_.PerformIO();
}

}  // namespace alpaca
