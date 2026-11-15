#include "controller/SearchEngine.h"
#include <cctype>
#include <utility>

namespace scapms {
namespace {
std::string lower(std::string s) {
    for (char& c : s) c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
    return s;
}
bool lessField(const PatientRecord* a, const PatientRecord* b, const std::string& field) {
    if (field == "ID") return a->id() < b->id();
    if (field == "Age") return a->age() < b->age();
    return lower(a->name()) < lower(b->name());
}
}
int SearchEngine::linearSearchById(const std::vector<std::unique_ptr<PatientRecord>>& d,
                                   const std::string& id) {
    for (std::size_t i = 0; i < d.size(); ++i) if (d[i]->id() == id) return static_cast<int>(i);
    return -1;
}
int SearchEngine::binarySearchById(const std::vector<std::unique_ptr<PatientRecord>>& d,
                                  const std::string& id) {
    int left = 0, right = static_cast<int>(d.size()) - 1;
    while (left <= right) {
        int mid = left + (right - left) / 2;
        if (d[mid]->id() == id) return mid;
        if (d[mid]->id() < id) left = mid + 1;
        else right = mid - 1;
    }
    return -1;
}
int SearchEngine::recursiveSearchByName(const std::vector<std::unique_ptr<PatientRecord>>& d,
                                        const std::string& name, int index) {
    if (index >= static_cast<int>(d.size())) return -1;
    if (lower(d[index]->name()) == lower(name)) return index;
    return recursiveSearchByName(d, name, index + 1);
}
void SearchEngine::selectionSort(std::vector<std::unique_ptr<PatientRecord>>& d,
                                 const std::string& field) {
    for (std::size_t i = 0; i < d.size(); ++i) {
        std::size_t best = i;
        for (std::size_t j = i + 1; j < d.size(); ++j)
            if (lessField(d[j].get(), d[best].get(), field)) best = j;
        if (best != i) std::swap(d[i], d[best]);
    }
}
void SearchEngine::insertionSort(std::vector<std::unique_ptr<PatientRecord>>& d,
                                 const std::string& field) {
    for (std::size_t i = 1; i < d.size(); ++i) {
        auto key = std::move(d[i]);
        std::size_t j = i;
        while (j > 0 && lessField(key.get(), d[j - 1].get(), field)) {
            d[j] = std::move(d[j - 1]);
            --j;
        }
        d[j] = std::move(key);
    }
}
}
