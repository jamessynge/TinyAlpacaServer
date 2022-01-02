#include "device_types/cover_calibrator/cover_calibrator_adapter.h"

#include "alpaca_response.h"
#include "ascom_error_codes.h"
#include "constants.h"
#include "literals.h"

namespace alpaca {

CoverCalibratorAdapter::CoverCalibratorAdapter(const DeviceInfo& device_info)
    : DeviceImplBase(device_info) {
  MCU_DCHECK_EQ(device_info.device_type, EDeviceType::kCoverCalibrator);
}

bool CoverCalibratorAdapter::HandleGetRequest(const AlpacaRequest& request,
                                              Print& out) {
  MCU_DCHECK_EQ(request.api, EAlpacaApi::kDeviceApi);
  MCU_DCHECK_EQ(request.device_type, EDeviceType::kCoverCalibrator);
  MCU_DCHECK_EQ(request.device_number, device_number());

  switch (request.device_method) {
    case EDeviceMethod::kBrightness:
      return WriteResponse::StatusOrIntResponse(request, GetBrightness(), out);

    case EDeviceMethod::kCalibratorState:
      return WriteResponse::StatusOrIntEnumResponse<ECalibratorStatus>(
          request, GetCalibratorState(), out);

    case EDeviceMethod::kCoverState:
      return WriteResponse::StatusOrIntEnumResponse<ECoverStatus>(
          request, GetCoverState(), out);

    case EDeviceMethod::kMaxBrightness:
      return WriteResponse::StatusOrIntResponse(request, GetMaxBrightness(),
                                                out);

    default:
      break;
  }
  return DeviceImplBase::HandleGetRequest(request, out);
}

mcucore::StatusOr<int32_t> CoverCalibratorAdapter::GetBrightness() {
  return ErrorCodes::ActionNotImplemented();
}

mcucore::StatusOr<ECalibratorStatus>
CoverCalibratorAdapter::GetCalibratorState() {
  return ECalibratorStatus::kNotPresent;
}

mcucore::StatusOr<ECoverStatus> CoverCalibratorAdapter::GetCoverState() {
  return ECoverStatus::kNotPresent;
}

mcucore::StatusOr<int32_t> CoverCalibratorAdapter::GetMaxBrightness() {
  // NOTE: This type of fixed value could be a good candidate for recording in a
  // Device-Type specific subclass of DeviceInfo.
  return ErrorCodes::ActionNotImplemented();
}

//////////////////////////////////////////////////////////////////////////////

// Handle a PUT 'request', write the HTTP response message to out.
bool CoverCalibratorAdapter::HandlePutRequest(const AlpacaRequest& request,
                                              Print& out) {
  MCU_DCHECK_EQ(request.api, EAlpacaApi::kDeviceApi);
  MCU_DCHECK_EQ(request.device_type, EDeviceType::kCoverCalibrator);
  MCU_DCHECK_EQ(request.device_number, device_number());

  switch (request.device_method) {
    case EDeviceMethod::kCalibratorOff:
      return HandlePutCalibratorOff(request, out);

    case EDeviceMethod::kCalibratorOn:
      return HandlePutCalibratorOn(request, out);

    case EDeviceMethod::kCloseCover:
      return HandlePutCloseCover(request, out);

    case EDeviceMethod::kHaltCover:
      return HandlePutHaltCover(request, out);

    case EDeviceMethod::kOpenCover:
      return HandlePutOpenCover(request, out);

    default:
      return DeviceImplBase::HandlePutRequest(request, out);
  }
}

bool CoverCalibratorAdapter::HandlePutCalibratorOff(
    const AlpacaRequest& request, Print& out) {
  return WriteResponse::StatusResponse(request, SetCalibratorOff(), out);
}

bool CoverCalibratorAdapter::HandlePutCalibratorOn(const AlpacaRequest& request,
                                                   Print& out) {
  if (!request.have_brightness) {
    return WriteResponse::AscomParameterMissingErrorResponse(
        request, ProgmemStringViews::brightness(), out);
  }
  // We decode the value as a uint32_t ()
  if (request.brightness > INT32_MAX) {
    return WriteResponse::AscomParameterInvalidErrorResponse(
        request, ProgmemStringViews::brightness(), out);
  }
  auto status_or_max = GetMaxBrightness();
  if (!status_or_max.ok()) {
    return WriteResponse::AscomErrorResponse(request, status_or_max.status(),
                                             out);
  }
  if (request.brightness > status_or_max.value()) {
    return WriteResponse::AscomParameterInvalidErrorResponse(
        request, ProgmemStringViews::brightness(), out);
  }
  return WriteResponse::StatusResponse(
      request, SetCalibratorBrightness(request.brightness), out);
}

bool CoverCalibratorAdapter::HandlePutCloseCover(const AlpacaRequest& request,
                                                 Print& out) {
  return WriteResponse::StatusResponse(request, MoveCover(/*open=*/false), out);
}

bool CoverCalibratorAdapter::HandlePutHaltCover(const AlpacaRequest& request,
                                                Print& out) {
  return WriteResponse::StatusResponse(request, HaltCoverMotion(), out);
}

bool CoverCalibratorAdapter::HandlePutOpenCover(const AlpacaRequest& request,
                                                Print& out) {
  return WriteResponse::StatusResponse(request, MoveCover(/*open=*/true), out);
}

mcucore::Status CoverCalibratorAdapter::SetCalibratorBrightness(
    uint32_t brightness) {
  return ErrorCodes::NotImplemented();
}

mcucore::Status CoverCalibratorAdapter::SetCalibratorOff() {
  return ErrorCodes::NotImplemented();
}

mcucore::Status CoverCalibratorAdapter::MoveCover(bool open) {
  return ErrorCodes::NotImplemented();
}

mcucore::Status CoverCalibratorAdapter::HaltCoverMotion() {
  return ErrorCodes::NotImplemented();
}
}  // namespace alpaca
