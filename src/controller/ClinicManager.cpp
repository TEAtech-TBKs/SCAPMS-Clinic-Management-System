#include "controller/ClinicManager.h"
#include <stdexcept>
#include <algorithm>

namespace scapms {
ClinicManager::ClinicManager() { recent_.fill(nullptr); }

void ClinicManager::rememberState() {
    Snapshot s;
    for (const auto& p : patients_) s.patients.push_back(p->clone());
    s.appointments = appointments_;
    undo_.push(std::move(s));
}
void ClinicManager::refreshCache() {
    recent_.fill(nullptr);
    for (std::size_t i = 0; i < patients_.size() && i < recent_.size(); ++i)
        recent_[i] = patients_[i].get();
}
void ClinicManager::addPatient(const std::string& id, const std::string& name, int age,
                               const std::string& contact, const std::string& type) {
    if (id.empty() || name.empty() || contact.empty()) throw std::invalid_argument("All patient fields are required.");
    if (age < 0 || age > 120) throw std::invalid_argument("Patient age must be between 0 and 120.");
    if (findPatient(id)) throw std::invalid_argument("Patient ID already exists.");
    rememberState();
    if (type == "Emergency") patients_.push_back(std::make_unique<EmergencyPatient>(id,name,age,contact));
    else if (type == "Follow-Up") patients_.push_back(std::make_unique<FollowUpPatient>(id,name,age,contact));
    else patients_.push_back(std::make_unique<GeneralPatient>(id,name,age,contact));
    refreshCache();
}
void ClinicManager::updatePatient(const std::string& id, const std::string& name, int age,
                                  const std::string& contact, const std::string& type) {
    PatientRecord* p = findPatient(id);
    if (!p) throw std::invalid_argument("Patient ID was not found.");
    if (name.empty() || contact.empty() || age < 0 || age > 120)
        throw std::invalid_argument("Enter a name, contact and an age from 0 to 120.");
    rememberState();
    // Changing category replaces the dynamic type while preserving its common data.
    std::unique_ptr<PatientRecord> replacement;
    if (type == "Emergency") replacement = std::make_unique<EmergencyPatient>(id,name,age,contact);
    else if (type == "Follow-Up") replacement = std::make_unique<FollowUpPatient>(id,name,age,contact);
    else replacement = std::make_unique<GeneralPatient>(id,name,age,contact);
    for (int a : p->appointmentIds()) replacement->addAppointmentId(a);
    for (auto& item : patients_) if (item->id() == id) { item = std::move(replacement); break; }
    refreshCache();
}
void ClinicManager::deletePatient(const std::string& id) {
    int index = -1;
    for (std::size_t i=0;i<patients_.size();++i) if (patients_[i]->id()==id) index=static_cast<int>(i);
    if (index < 0) throw std::invalid_argument("Patient ID was not found.");
    rememberState();
    for (auto& a : appointments_) if (a.patientId == id && a.status == "Pending") a.status = "Cancelled";
    patients_.erase(patients_.begin()+index);
    refreshCache();
}
PatientRecord* ClinicManager::findPatient(const std::string& id) {
    for (auto& p : patients_) if (p->id()==id) return p.get();
    return nullptr;
}
const PatientRecord* ClinicManager::findPatient(const std::string& id) const {
    for (const auto& p : patients_) if (p->id()==id) return p.get();
    return nullptr;
}
void ClinicManager::schedule(const std::string& patientId, const std::string& dateTime,
                             const std::string& reason) {
    PatientRecord* p = findPatient(patientId);
    if (!p) throw std::invalid_argument("Register the patient before scheduling an appointment.");
    if (dateTime.empty() || reason.empty()) throw std::invalid_argument("Appointment date/time and reason are required.");
    rememberState();
    p->scheduleAppointment(dateTime);
    Appointment a; a.id=nextAppointmentId_++; a.patientId=patientId; a.dateTime=dateTime; a.reason=reason;
    appointments_.push_back(a); p->addAppointmentId(a.id);
}
void ClinicManager::cancelAppointment(int appointmentId) {
    for (auto& a : appointments_) if (a.id==appointmentId && a.status=="Pending") {
        rememberState(); a.status="Cancelled";
        if (auto* p=findPatient(a.patientId)) p->cancelAppointment(appointmentId);
        return;
    }
    throw std::invalid_argument("Pending appointment ID was not found.");
}
void ClinicManager::addToQueue(const std::string& id) {
    if (!findPatient(id)) throw std::invalid_argument("Patient ID was not found.");
    waiting_.push(id);
}
std::string ClinicManager::callNext() {
    if (waiting_.empty()) throw std::invalid_argument("The waiting queue is empty.");
    std::string id=waiting_.front(); waiting_.pop(); return id;
}
bool ClinicManager::undoLast() {
    if (undo_.empty()) return false;
    Snapshot s=std::move(undo_.top()); undo_.pop();
    patients_=std::move(s.patients); appointments_=std::move(s.appointments);
    refreshCache(); return true;
}
void ClinicManager::clear() {
    patients_.clear(); appointments_.clear(); waiting_={}; undo_={}; nextAppointmentId_=1; refreshCache();
}
void ClinicManager::replaceData(PatientList p, std::vector<Appointment> a) {
    patients_=std::move(p); appointments_=std::move(a); waiting_={}; undo_={};
    nextAppointmentId_=1;
    for (const auto& x:appointments_) if (x.id>=nextAppointmentId_) nextAppointmentId_=x.id+1;
    refreshCache();
}
int ClinicManager::completedCount(const std::string& id) const {
    int n=0; for (const auto& a:appointments_) if(a.patientId==id && a.status=="Completed") ++n; return n;
}
int ClinicManager::pendingCount() const {
    int n=0; for (const auto& a:appointments_) if(a.status=="Pending") ++n; return n;
}
int ClinicManager::categoryCount(const std::string& c) const {
    int n=0; for (const auto& p:patients_) if(p->category()==c) ++n; return n;
}
}
