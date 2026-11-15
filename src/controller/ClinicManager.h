#pragma once
#include "model/PatientRecord.h"
#include <vector>
#include <queue>
#include <stack>
#include <array>
#include <memory>
#include <string>

namespace scapms {
class ClinicManager {
    using PatientList = std::vector<std::unique_ptr<PatientRecord>>;
    struct Snapshot {
        PatientList patients;
        std::vector<Appointment> appointments;
    };
    PatientList patients_;
    std::vector<Appointment> appointments_;
    std::queue<std::string> waiting_;
    std::stack<Snapshot> undo_;
    std::array<PatientRecord*, 10> recent_{};
    int nextAppointmentId_ = 1;

    void rememberState();
    void refreshCache();
public:
    ClinicManager();
    PatientList& patients() { return patients_; }
    const PatientList& patients() const { return patients_; }
    const std::vector<Appointment>& appointments() const { return appointments_; }
    std::queue<std::string> waitingQueue() const { return waiting_; }

    void addPatient(const std::string& id, const std::string& name, int age,
                    const std::string& contact, const std::string& type);
    void updatePatient(const std::string& id, const std::string& name, int age,
                       const std::string& contact, const std::string& type);
    void deletePatient(const std::string& id);
    PatientRecord* findPatient(const std::string& id);
    const PatientRecord* findPatient(const std::string& id) const;
    void schedule(const std::string& patientId, const std::string& dateTime,
                  const std::string& reason);
    void cancelAppointment(int appointmentId);
    void addToQueue(const std::string& patientId);
    std::string callNext();
    bool undoLast();
    void clear();
    void replaceData(PatientList patients, std::vector<Appointment> appointments);
    int completedCount(const std::string& patientId) const;
    int pendingCount() const;
    int categoryCount(const std::string& category) const;
};
}
