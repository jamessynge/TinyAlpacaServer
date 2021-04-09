#include "tiny_alpaca_server.h"

#include "alpaca_response.h"
#include "literals.h"

namespace alpaca {

TinyAlpacaServer::TinyAlpacaServer(uint16_t tcp_port,
                                   const ServerDescription& server_description,
                                   ArrayView<DeviceInterfacePtr> devices)
    : alpaca_devices_(server_description, devices),
      server_connections_(*this, tcp_port),
      discovery_server_(tcp_port),
      server_transaction_id_(0) {}

bool TinyAlpacaServer::Initialize() {
  return alpaca_devices_.Initialize() && server_connections_.Initialize() &&
         discovery_server_.Initialize();
}

void TinyAlpacaServer::PerformIO() {
  alpaca_devices_.MaintainDevices();
  server_connections_.PerformIO();
  discovery_server_.PerformIO();
}

void TinyAlpacaServer::OnStartDecoding(AlpacaRequest& request) {
  request.set_server_transaction_id(++server_transaction_id_);
}

bool TinyAlpacaServer::OnRequestDecoded(AlpacaRequest& request, Print& out) {
  TAS_VLOG(3) << TASLIT("OnRequestDecoded: api=") << request.api;
  switch (request.api) {
    case EAlpacaApi::kUnknown:
      break;

    case EAlpacaApi::kDeviceApi:
      // ABSL_FALLTHROUGH_INTENDED
    case EAlpacaApi::kDeviceSetup:
      return alpaca_devices_.DispatchDeviceRequest(request, out);

    case EAlpacaApi::kManagementApiVersions:
      return HandleManagementApiVersions(request, out);

    case EAlpacaApi::kManagementDescription:
      return HandleManagementDescription(request, out);

    case EAlpacaApi::kManagementConfiguredDevices:
      return HandleManagementConfiguredDevices(request, out);

    case EAlpacaApi::kServerSetup:
      return HandleServerSetup(request, out);
  }

  return WriteResponse::HttpErrorResponse(
      EHttpStatusCode::kHttpInternalServerError, Literals::ApiUnknown(), out);
}

void TinyAlpacaServer::OnRequestDecodingError(AlpacaRequest& request,
                                              EHttpStatusCode status,
                                              Print& out) {
  TAS_VLOG(3) << TASLIT("OnRequestDecodingError: status=") << status;
  WriteResponse::HttpErrorResponse(status, AnyPrintable(), out);
}

bool TinyAlpacaServer::HandleManagementApiVersions(AlpacaRequest& request,
                                                   Print& out) {
  TAS_VLOG(3) << TASLIT("HandleManagementApiVersions");
  return WriteResponse::AscomNotImplementedErrorResponse(request, out);
}

bool TinyAlpacaServer::HandleManagementDescription(AlpacaRequest& request,
                                                   Print& out) {
  TAS_VLOG(3) << TASLIT("HandleManagementDescription");
  return WriteResponse::AscomNotImplementedErrorResponse(request, out);
}

bool TinyAlpacaServer::HandleManagementConfiguredDevices(AlpacaRequest& request,
                                                         Print& out) {
  TAS_VLOG(3) << TASLIT("HandleManagementConfiguredDevices");
  return WriteResponse::AscomNotImplementedErrorResponse(request, out);
}

bool TinyAlpacaServer::HandleServerSetup(AlpacaRequest& request, Print& out) {
  TAS_VLOG(3) << TASLIT("HandleServerSetup");
  return WriteResponse::AscomNotImplementedErrorResponse(request, out);
}

}  // namespace alpaca
