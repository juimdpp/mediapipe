// Copyright 2019 The MediaPipe Authors.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "mediapipe/calculators/util/hyunsoo_logger_calculator.pb.h"
#include "mediapipe/framework/calculator_framework.h"
#include "mediapipe/framework/port/logging.h"
#include <iostream>
#include <chrono>
#include <string>

namespace mediapipe {

constexpr char kLogTag[] = "LOG_TAG";

class HyunsooLoggerCalculator : public CalculatorBase {
 public:
  HyunsooLoggerCalculator() = default;
  ~HyunsooLoggerCalculator() override = default;

  static absl::Status GetContract(CalculatorContract* cc);

  // From Calculator.
  absl::Status Open(CalculatorContext* cc) override;
  absl::Status Process(CalculatorContext* cc) override;
  absl::Status Close(CalculatorContext* cc) override;

 private:
  std::string log_tag;
};
REGISTER_CALCULATOR(HyunsooLoggerCalculator);

absl::Status HyunsooLoggerCalculator::GetContract(CalculatorContract* cc) {
  // Mandatory fields
  cc->Inputs().Index(0).SetAny();
  return absl::OkStatus();
}

absl::Status HyunsooLoggerCalculator::Open(CalculatorContext* cc) {
  cc->SetOffset(TimestampDiff(0));
  const auto calculator_options =
        cc->Options<HyunsooLoggerCalculatorOptions>();
  log_tag = calculator_options.log_tag();
  return absl::OkStatus();
}

absl::Status HyunsooLoggerCalculator::Process(CalculatorContext* cc) {
  auto currentTime = std::chrono::steady_clock::now();
  auto duration = currentTime.time_since_epoch();
  auto nanoseconds = std::chrono::duration_cast<std::chrono::nanoseconds>(duration);
  auto input = cc->Inputs().Index(0).Value();
  LOG(INFO) << "Hyunsoo - " << log_tag << " " << nanoseconds.count() << " / " << input.Timestamp() << std::endl;

  return absl::OkStatus();
}

absl::Status HyunsooLoggerCalculator::Close(CalculatorContext* cc) {
  return absl::OkStatus();
}

}  // namespace mediapipe
