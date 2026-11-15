#pragma once
#include "model/PatientRecord.h"
#include <string>
#include <vector>
#include <memory>

namespace scapms {
class FileHandler {
public:
    static void save(const std::string& path,
                     const std::vector<std::unique_ptr<PatientRecord>>& patients,
                     const std::vector<Appointment>& appointments);
    static void load(const std::string& path,
                     std::vector<std::unique_ptr<PatientRecord>>& patients,
                     std::vector<Appointment>& appointments);
};
}
