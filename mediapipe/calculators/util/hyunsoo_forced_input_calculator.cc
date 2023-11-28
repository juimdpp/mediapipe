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

#include "mediapipe/calculators/util/hyunsoo_forced_input_calculator.pb.h"
#include "mediapipe/framework/formats/rect.pb.h"
#include "mediapipe/framework/calculator_framework.h"
#include "mediapipe/framework/port/logging.h"
#include <iostream>
#include <string>

// Takes in an iterable and selects one item of the iterable and duplicates it N times

namespace mediapipe {
namespace {
    using ::mediapipe::NormalizedRect;
}
class HyunsooForcedInputCalculator : public CalculatorBase {
 public:
  HyunsooForcedInputCalculator() = default;
  ~HyunsooForcedInputCalculator() override = default;

  static absl::Status GetContract(CalculatorContract* cc);

  // From Calculator.
  absl::Status Open(CalculatorContext* cc) override;
  absl::Status Process(CalculatorContext* cc) override;
  absl::Status Close(CalculatorContext* cc) override;

 private:
  int number_detections;
};
REGISTER_CALCULATOR(HyunsooForcedInputCalculator);

absl::Status HyunsooForcedInputCalculator::GetContract(CalculatorContract* cc) {
  // Mandatory fields
  RET_CHECK(cc->Inputs().HasTag("INPUT_ITERABLE"));
  cc->Inputs().Tag("INPUT_ITERABLE").Set<std::vector<::mediapipe::NormalizedRect>>();

  RET_CHECK(cc->Outputs().HasTag("OUTPUT_ITERABLE"));
  cc->Outputs().Tag("OUTPUT_ITERABLE").Set<std::vector<::mediapipe::NormalizedRect>>();
  return absl::OkStatus();
}

absl::Status HyunsooForcedInputCalculator::Open(CalculatorContext* cc) {
  cc->SetOffset(TimestampDiff(0));

  const auto calculator_options =
        cc->Options<HyunsooForcedInputCalculatorOptions>();
  number_detections = calculator_options.number_detections();

  return absl::OkStatus();
}

absl::Status HyunsooForcedInputCalculator::Process(CalculatorContext* cc) {
  std::vector<NormalizedRect> input_iterable = 
    cc->Inputs().Tag("INPUT_ITERABLE").Get<std::vector<NormalizedRect>>();
  NormalizedRect input_item = input_iterable.front();
  
  auto output = absl::make_unique<std::vector<NormalizedRect>>();
  for(int i=0; i<number_detections; i++){
    output->push_back(input_item);
  }
  LOG(INFO) << "HyunsooForcedInputCalculator ** " << output->size() << "\n";
  cc->Outputs().Tag("OUTPUT_ITERABLE").Add(output.release(), cc->InputTimestamp());
  return absl::OkStatus();
}

absl::Status HyunsooForcedInputCalculator::Close(CalculatorContext* cc) {
  return absl::OkStatus();
}

}  // namespace mediapipe
