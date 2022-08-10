#include "device_types/device_impl_base.h"

#include <McuCore.h>

#include "alpaca_response.h"
#include "constants.h"
#include "device_description.h"
#include "literals.h"

namespace alpaca {
namespace {

class DeviceDescriptionHtml : public Printable {
 public:
  explicit DeviceDescriptionHtml(const DeviceDescription& description)
      : description_(description) {}

  size_t printTo(Print& out) const override {
    mcucore::CountingPrint counter(out);
    mcucore::OPrintStream strm(counter);
    auto status_or_uuid = description_.GetOrCreateUniqueId();
    mcucore::Uuid uuid;
    if (!status_or_uuid.ok()) {
      MCU_DCHECK_OK(status_or_uuid)
          << MCU_PSD("Should have been able to GetOrCreateUniqueId");
      uuid.Zero();
    } else {
      uuid = status_or_uuid.value();
    }

    strm << MCU_PSD(
                "<html><body>"
                "<h1>Tiny Alpaca Server Device Setup</h1>\n"
                "Type: ")
         << description_.device_type << MCU_PSD("<br>")  //
         << MCU_PSD("Number: ") << description_.device_number << MCU_PSD("<br>")
         << MCU_PSD("Name: ") << description_.name << MCU_PSD("<br>")
         << MCU_PSD("Description: ") << description_.description
         << MCU_PSD("<br>") << MCU_PSD("Unique ID: ") << uuid << MCU_PSD("<br>")
         << MCU_PSD("Driver Info: ") << description_.driver_info
         << MCU_PSD("<br>") << MCU_PSD("Driver Version: ")
         << description_.driver_version << MCU_PSD("<br>")
         << MCU_PSD("Interface Version: ") << description_.interface_version()
         << MCU_PSD("<br>") << MCU_PSD("EEPROM Domain: ")
         << description_.domain.value() << MCU_PSD("</body></html>");
    return counter.count();
  }

 private:
  const DeviceDescription& description_;
};

}  // namespace

bool DeviceImplBase::HandleDeviceSetupRequest(const AlpacaRequest& request,
                                              Print& out) {
  // Produce a default response indicating that there is no custom setup for
  // this device.
  DeviceDescriptionHtml html(device_description_);
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
  // kHttpNotImplemented, but using the HttpMethodNotImplemented reason
  // phrase.
  return WriteResponse::HttpErrorResponse(
      EHttpStatusCode::kHttpInternalServerError,
      mcucore::PrintableCat(MCU_FLASHSTR("request.api: "),
                            ToFlashStringHelper(request.http_method)),
      out);
}

void DeviceImplBase::AddToHomePageHtml(const AlpacaRequest& request,
                                       EHtmlPageSection section,
                                       mcucore::OPrintStream& strm) {
  if (section == EHtmlPageSection::kBody) {
    AddStartDeviceSection(strm);
    AddDeviceBanner(strm);
    AddDeviceSummary(strm);
    AddDeviceDetails(strm);
    AddEndDeviceSection(strm);
  }
}

void DeviceImplBase::AddStartDeviceSection(mcucore::OPrintStream& strm) {
  strm << MCU_PSD("<div class='d ") << device_description_.device_type
       << MCU_PSD("' id=") << device_description_.device_type << '_'
       << device_description_.device_number << MCU_PSD(">\n");
}

void DeviceImplBase::AddEndDeviceSection(mcucore::OPrintStream& strm) {
  strm << MCU_PSD("</div>\n");
}

void DeviceImplBase::AddDeviceBanner(mcucore::OPrintStream& strm) {
  strm << MCU_PSD("<div class=db><h3><span class=dn>")
       << device_description_.name << MCU_PSD("</span> <span class=dt>")
       << device_description_.device_type << MCU_PSD(" device #")
       << device_description_.device_number << MCU_PSD("</span></h3></div>\n");
}

void DeviceImplBase::AddDeviceSummary(mcucore::OPrintStream& strm) {
  strm << MCU_PSD(
              "<table class=ds>\n<tr class=dd><td>Description:</td>"
              "<td class=dd>")
       << device_description_.description << MCU_PSD("</td></tr>\n");

  auto status_or_uuid = device_description_.GetOrCreateUniqueId();
  if (status_or_uuid.ok()) {
    strm << MCU_PSD("<tr class=du><td>Unique ID:</td><td class=du>")
         << status_or_uuid.value() << MCU_PSD("</td></tr>\n");
  }

  strm << MCU_PSD("<tr class=ddi><td>Driver:</td><td class=ddi>")
       << device_description_.driver_info << MCU_PSD("</td></tr>\n");
  strm << MCU_PSD("<tr class=ddv><td>Driver Version:</td><td class=ddv>")
       << device_description_.driver_version << MCU_PSD("</td></tr>\n");
  strm << MCU_PSD("<tr class=dom><td>EEPROM Domain:</td><td class=dom>")
       << device_description_.domain.value() << MCU_PSD("</td></tr></table>\n");
}

mcucore::EepromTag DeviceImplBase::MakeTag(uint8_t id) {
  return {.domain = device_description_.domain, .id = id};
}

bool DeviceImplBase::HandleGetRequest(const AlpacaRequest& request,
                                      Print& out) {
  switch (request.device_method) {
    case EDeviceMethod::kConnected:
      return WriteResponse::StatusOrBoolResponse(request, GetConnected(), out);

    case EDeviceMethod::kDescription:
      return WriteResponse::AnyPrintableStringResponse(
          request, device_description_.description, out);

    case EDeviceMethod::kDriverInfo:
      return WriteResponse::AnyPrintableStringResponse(
          request, device_description_.driver_info, out);

    case EDeviceMethod::kDriverVersion:
      return WriteResponse::AnyPrintableStringResponse(
          request, device_description_.driver_version, out);

    case EDeviceMethod::kInterfaceVersion:
      return WriteResponse::IntResponse(
          request, device_description_.interface_version(), out);

    case EDeviceMethod::kName:
      return WriteResponse::AnyPrintableStringResponse(
          request, device_description_.name, out);

    case EDeviceMethod::kSupportedActions:
      return WriteResponse::ProgmemStringArrayResponse(
          request, device_description_.supported_actions, out);

    default:
      return WriteResponse::AscomMethodNotImplementedResponse(request, out);
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
      return WriteResponse::AscomMethodNotImplementedResponse(request, out);
  }
}

bool DeviceImplBase::HandlePutAction(const AlpacaRequest& request, Print& out) {
  // If there are NO supported actions, then we return MethodNotImplemented,
  // rather than ActionNotImplemented, which we return when there are some valid
  // actions, but the specified action name is not supported.
  if (device_description_.supported_actions.size == 0) {
    return WriteResponse::AscomMethodNotImplementedResponse(request, out);
  }
  return WriteResponse::AscomActionNotImplementedResponse(request, out);
}

bool DeviceImplBase::HandlePutCommandBlind(const AlpacaRequest& request,
                                           Print& out) {
  return WriteResponse::AscomMethodNotImplementedResponse(request, out);
}

bool DeviceImplBase::HandlePutCommandBool(const AlpacaRequest& request,
                                          Print& out) {
  return WriteResponse::AscomMethodNotImplementedResponse(request, out);
}

bool DeviceImplBase::HandlePutCommandString(const AlpacaRequest& request,
                                            Print& out) {
  return WriteResponse::AscomMethodNotImplementedResponse(request, out);
}

bool DeviceImplBase::HandlePutConnected(const AlpacaRequest& request,
                                        Print& out) {
  if (!request.have_connected) {
    return WriteResponse::AscomParameterMissingErrorResponse(
        request, ProgmemStringViews::Connected(), out);
  }
  return WriteResponse::StatusResponse(request, SetConnected(request.connected),
                                       out);
}

mcucore::StatusOr<bool> DeviceImplBase::GetConnected() { return true; }

mcucore::Status DeviceImplBase::SetConnected(bool value) {
  return mcucore::OkStatus();
}

}  // namespace alpaca
