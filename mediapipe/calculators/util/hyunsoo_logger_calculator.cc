// // Copyright 2019 The MediaPipe Authors.
// //
// // Licensed under the Apache License, Version 2.0 (the "License");
// // you may not use this file except in compliance with the License.
// // You may obtain a copy of the License at
// //
// //      http://www.apache.org/licenses/LICENSE-2.0
// //
// // Unless required by applicable law or agreed to in writing, software
// // distributed under the License is distributed on an "AS IS" BASIS,
// // WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// // See the License for the specific language governing permissions and
// // limitations under the License.

// #include "mediapipe/calculators/util/hyunsoo_logger_calculator.pb.h"
// #include "mediapipe/framework/calculator_framework.h"
// #include "mediapipe/framework/port/logging.h"
// #include <iostream>
// #include <chrono>
// #include <string>

// namespace mediapipe {

// constexpr char kLogTag[] = "LOG_TAG";

// class HyunsooLoggerCalculator : public CalculatorBase {
//  public:
//   HyunsooLoggerCalculator() = default;
//   ~HyunsooLoggerCalculator() override = default;

//   static absl::Status GetContract(CalculatorContract* cc);

//   // From Calculator.
//   absl::Status Open(CalculatorContext* cc) override;
//   absl::Status Process(CalculatorContext* cc) override;
//   absl::Status Close(CalculatorContext* cc) override;

//  private:
//   std::string log_tag;
// };
// REGISTER_CALCULATOR(HyunsooLoggerCalculator);

// absl::Status HyunsooLoggerCalculator::GetContract(CalculatorContract* cc) {
//   // Mandatory fields
//   cc->Inputs().Index(0).SetAny();
//   return absl::OkStatus();
// }

// absl::Status HyunsooLoggerCalculator::Open(CalculatorContext* cc) {
//   cc->SetOffset(TimestampDiff(0));
//   const auto calculator_options =
//         cc->Options<HyunsooLoggerCalculatorOptions>();
//   log_tag = calculator_options.log_tag();
//   return absl::OkStatus();
// }

// absl::Status HyunsooLoggerCalculator::Process(CalculatorContext* cc) {
//   auto currentTime = std::chrono::steady_clock::now();
//   auto duration = currentTime.time_since_epoch();
//   auto nanoseconds = std::chrono::duration_cast<std::chrono::nanoseconds>(duration);
//   auto input = cc->Inputs().Index(0).Value();
//   LOG(INFO) << "Hyunsoo - " << log_tag << " " << nanoseconds.count() << " / " << input.Timestamp() << std::endl;

//   return absl::OkStatus();
// }

// absl::Status HyunsooLoggerCalculator::Close(CalculatorContext* cc) {
//   return absl::OkStatus();
// }

// }  // namespace mediapipe


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

#include "mediapipe/framework/calculator_framework.h"
#include "mediapipe/framework/port/canonical_errors.h"
#include "mediapipe/calculators/util/hyunsoo_logger_calculator.pb.h"
#include <iostream>
#include <chrono>
#include <string>

namespace mediapipe {
constexpr char kLogTag[] = "LOG_TAG";
// A Calculator that simply passes its input Packets and header through,
// unchanged.  The inputs may be specified by tag or index.  The outputs
// must match the inputs exactly.  Any number of input side packets may
// also be specified.  If output side packets are specified, they must
// match the input side packets exactly and the Calculator passes its
// input side packets through, unchanged.  Otherwise, the input side
// packets will be ignored (allowing HyunsooLoggerCalculator to be used to
// test internal behavior).  Any options may be specified and will be
// ignored.
class HyunsooLoggerCalculator : public CalculatorBase {
  private:
    std::string log_tag;
 public:
  static absl::Status GetContract(CalculatorContract* cc) {
    // if (!cc->Inputs().TagMap()->SameAs(*cc->Outputs().TagMap())) {
    //   return absl::InvalidArgumentError(
    //       "Input and output streams to HyunsooLoggerCalculator must use "
    //       "matching tags and indexes.");
    // }
    for (CollectionItemId id = cc->Inputs().BeginId();
         id < cc->Inputs().EndId(); ++id) {
      cc->Inputs().Get(id).SetAny();
      cc->Outputs().Get(id).SetSameAs(&cc->Inputs().Get(id));
    }
    for (CollectionItemId id = cc->InputSidePackets().BeginId();
         id < cc->InputSidePackets().EndId(); ++id) {
      cc->InputSidePackets().Get(id).SetAny();
    }
    if (cc->OutputSidePackets().NumEntries() != 0) {
      if (!cc->InputSidePackets().TagMap()->SameAs(
              *cc->OutputSidePackets().TagMap())) {
        return absl::InvalidArgumentError(
            "Input and output side packets to HyunsooLoggerCalculator must use "
            "matching tags and indexes.");
      }
      for (CollectionItemId id = cc->InputSidePackets().BeginId();
           id < cc->InputSidePackets().EndId(); ++id) {
        cc->OutputSidePackets().Get(id).SetSameAs(
            &cc->InputSidePackets().Get(id));
      }
    }
    return absl::OkStatus();
  }

  absl::Status Open(CalculatorContext* cc) final {
    for (CollectionItemId id = cc->Inputs().BeginId();
         id < cc->Inputs().EndId(); ++id) {
      if (!cc->Inputs().Get(id).Header().IsEmpty()) {
        cc->Outputs().Get(id).SetHeader(cc->Inputs().Get(id).Header());
      }
    }
    if (cc->OutputSidePackets().NumEntries() != 0) {
      for (CollectionItemId id = cc->InputSidePackets().BeginId();
           id < cc->InputSidePackets().EndId(); ++id) {
        cc->OutputSidePackets().Get(id).Set(cc->InputSidePackets().Get(id));
      }
    }
    const auto calculator_options =
          cc->Options<HyunsooLoggerCalculatorOptions>();
    log_tag = calculator_options.log_tag();
    cc->SetOffset(TimestampDiff(0));
    return absl::OkStatus();
  }

  absl::Status Process(CalculatorContext* cc) final {
    auto currentTime = std::chrono::steady_clock::now();
    auto duration = currentTime.time_since_epoch();
    auto milliseconds = std::chrono::duration_cast<std::chrono::nanoseconds>(duration);

    cc->GetCounter("PassThrough")->Increment();
    if (cc->Inputs().NumEntries() == 0) {
      return tool::StatusStop();
    }
    for (CollectionItemId id = cc->Inputs().BeginId();
         id < cc->Inputs().EndId(); ++id) {
      if (!cc->Inputs().Get(id).IsEmpty()) {
        VLOG(3) << "Passing " << cc->Inputs().Get(id).Name() << " to "
                << cc->Outputs().Get(id).Name() << " at "
                << cc->InputTimestamp().DebugString();
        LOG(INFO) << "Hyunsoo - " << log_tag << " | " 
                  << milliseconds.count() 
                  << " / " << cc->Inputs().Get(id).Value().Timestamp()
                  << " / " << cc->InputTimestamp().DebugString() << std::endl;
        cc->Outputs().Get(id).AddPacket(cc->Inputs().Get(id).Value());
      }
    }
    return absl::OkStatus();
  }
};
REGISTER_CALCULATOR(HyunsooLoggerCalculator);

}  // namespace mediapipe
