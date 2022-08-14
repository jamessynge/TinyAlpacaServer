#include "tiny_alpaca_server.h"

#include <McuCore.h>

#include "alpaca_response.h"
#include "constants.h"
#include "http_response_header.h"
#include "literals.h"

namespace alpaca {

TinyAlpacaDeviceServer::TinyAlpacaDeviceServer(
    ServerContext& server_context, const ServerDescription& server_description,
    mcucore::ArrayView<DeviceInterface*> devices)
    : server_context_(server_context),
      alpaca_devices_(server_context, devices),
      server_description_(server_description),
      server_transaction_id_(0) {}

void TinyAlpacaDeviceServer::ValidateAndReset() {
  MCU_CHECK_OK(server_context_.Initialize());
  alpaca_devices_.ValidateDevices();
  alpaca_devices_.ResetHardware();
}

void TinyAlpacaDeviceServer::InitializeForServing() {
  alpaca_devices_.InitializeDevices();
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

    case EAlpacaApi::kServerStatus:
      return HandleServerStatus(request, out);
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
  MCU_VLOG(3) << MCU_PSD("OnRequestDecodingError ") << MCU_NAME_VAL(status);
  WriteResponse::HttpErrorResponse(status, mcucore::AnyPrintable(), out);
}

bool TinyAlpacaDeviceServer::HandleManagementApiVersions(AlpacaRequest& request,
                                                         Print& out) {
  MCU_VLOG(3) << MCU_PSD("HandleManagementApiVersions");
  uint32_t versions[] = {1};
  return WriteResponse::ArrayResponse(
      request,
      MakeArrayViewSource(versions, &mcucore::JsonArrayEncoder::AddUIntElement),
      out);
}

bool TinyAlpacaDeviceServer::HandleManagementDescription(AlpacaRequest& request,
                                                         Print& out) {
  MCU_VLOG(3) << MCU_PSD("TinyAlpacaDeviceServer::HandleManagementDescription");
  mcucore::JsonPropertySourceAdapter<ServerDescription> description(
      server_description_);
  return WriteResponse::ObjectResponse(request, description, out);
}

bool TinyAlpacaDeviceServer::HandleServerSetup(AlpacaRequest& request,
                                               Print& out) {
  MCU_VLOG(3) << MCU_PSD("HandleServerSetup");

  auto body = MCU_FLASHSTR(
      "<html><body>"
      "<h1>Tiny Alpaca Server</h1>\n"
      "No setup\n"
      "</body></html>");

  return WriteResponse::OkResponse(request, EContentType::kTextHtml,
                                   mcucore::AnyPrintable(body), out,
                                   /*append_http_newline=*/true);
}

bool TinyAlpacaDeviceServer::HandleServerStatus(AlpacaRequest& request,
                                                Print& out) {
  MCU_VLOG(3) << MCU_PSD("HandleServerStatus");
  if (!(request.http_method == EHttpMethod::GET ||
        request.http_method == EHttpMethod::HEAD)) {
    WriteResponse::HttpErrorResponse(EHttpStatusCode::kHttpMethodNotAllowed,
                                     mcucore::AnyPrintable(), out);
    return false;
  }

  HttpResponseHeader hrh;
  hrh.status_code = EHttpStatusCode::kHttpOk;
  hrh.reason_phrase = ProgmemStrings::OK();
  hrh.content_type = EContentType::kTextHtml;
  hrh.do_close = true;
  hrh.printTo(out);

  if (request.http_method == EHttpMethod::GET) {
    // Start html, start head, then give each device a chance to add to head.
    mcucore::OPrintStream strm(out);
    strm << MCU_PSD("<html><head><title>") << server_description_.server_name
         << MCU_PSD(" (Tiny Alpaca Server)</title>\n");
    alpaca_devices_.AddToHomePageHtml(request, EHtmlPageSection::kHead, strm);
    strm << MCU_PSD("</head><body>\n<div class=s><h1 id=sn>")
         << server_description_.server_name << MCU_PSD("</h1>\n<table>\n")
         << MCU_PSD("<tr id=ss><td>Server Software:</td><td class=ss>")
         << MCU_PSD("<a href='")
         << MCU_PSD("https://github/jamessynge/TinyAlpacaServer")
         << MCU_PSD("'>") << MCU_PSD("Tiny Alpaca Server") << MCU_PSD("</a>")
         << MCU_PSD("</td></tr>\n")
         << MCU_PSD("<tr id=sl><td>Location:</td><td class=sl>")
         << server_description_.location << MCU_PSD("</td></tr>\n")
         << MCU_PSD("<tr id=sm><td>Manufacturer:</td><td class=sm>")
         << server_description_.manufacturer << MCU_PSD("</td></tr>\n")
         << MCU_PSD("<tr id=smv><td>Version:</td><td class=smv>")
         << server_description_.manufacturer_version << MCU_PSD("</td></tr>\n")
         << MCU_PSD(
                "</table>\n</div>\n<div class=d>\n<h2 id=dsl>Configured "
                "Devices<h2>\n");
    alpaca_devices_.AddToHomePageHtml(request, EHtmlPageSection::kBody, strm);
    strm << MCU_PSD("\n</div>");
    alpaca_devices_.AddToHomePageHtml(request, EHtmlPageSection::kTrailer,
                                      strm);
    strm << MCU_PSD("\n</body></html>");
  }

  return false;  // There is no Content-Length in the header, so we can't
                 // continue the connection after this.
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
