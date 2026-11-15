#include "model/PatientRecord.h"
#include <stdexcept>
#include <utility>

namespace scapms {
PatientRecord::PatientRecord(std::string id, std::string name, int age, std::string contact)
    : id_(std::move(id)), name_(std::move(name)), contact_(std::move(contact)), age_(age) {}

void PatientRecord::setName(const std::string& v) {
    if (v.empty()) throw std::invalid_argument("Patient name cannot be empty.");
    name_ = v;
}
void PatientRecord::setAge(int v) {
    if (v < 0 || v > 120) throw std::invalid_argument("Patient age must be between 0 and 120.");
    age_ = v;
}
void PatientRecord::setContact(const std::string& v) {
    if (v.empty()) throw std::invalid_argument("Contact information cannot be empty.");
    contact_ = v;
}
void PatientRecord::addAppointmentId(int id) { appointmentIds_.push_back(id); }
void PatientRecord::removeAppointmentId(int id) {
    for (auto it = appointmentIds_.begin(); it != appointmentIds_.end(); ++it) {
        if (*it == id) { appointmentIds_.erase(it); return; }
    }
}
void PatientRecord::scheduleAppointment(const std::string& dateTime) {
    if (dateTime.empty()) throw std::invalid_argument("Appointment date/time is required.");
}
void PatientRecord::cancelAppointment(int appointmentId) { removeAppointmentId(appointmentId); }

std::unique_ptr<PatientRecord> GeneralPatient::clone() const {
    auto p = std::make_unique<GeneralPatient>(id_, name_, age_, contact_);
    for (int a : appointmentIds_) p->addAppointmentId(a);
    return p;
}
std::unique_ptr<PatientRecord> EmergencyPatient::clone() const {
    auto p = std::make_unique<EmergencyPatient>(id_, name_, age_, contact_);
    for (int a : appointmentIds_) p->addAppointmentId(a);
    return p;
}
std::unique_ptr<PatientRecord> FollowUpPatient::clone() const {
    auto p = std::make_unique<FollowUpPatient>(id_, name_, age_, contact_);
    for (int a : appointmentIds_) p->addAppointmentId(a);
    return p;
}
}
