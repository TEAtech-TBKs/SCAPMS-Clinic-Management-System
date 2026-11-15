#pragma once
#include "model/PatientRecord.h"
#include <vector>
#include <memory>
#include <string>

namespace scapms {
class SearchEngine {
public:
    static int linearSearchById(const std::vector<std::unique_ptr<PatientRecord>>& data,
                                const std::string& id);
    static int binarySearchById(const std::vector<std::unique_ptr<PatientRecord>>& sortedData,
                                const std::string& id);
    static int recursiveSearchByName(const std::vector<std::unique_ptr<PatientRecord>>& data,
                                     const std::string& name, int index = 0);
    static void selectionSort(std::vector<std::unique_ptr<PatientRecord>>& data,
                              const std::string& field);
    static void insertionSort(std::vector<std::unique_ptr<PatientRecord>>& data,
                              const std::string& field);
};
}
