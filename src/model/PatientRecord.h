#pragma once
#include <string>
#include <vector>
#include <memory>

namespace scapms {

class Appointment;

class Appointable {
public:
    virtual ~Appointable() = default;
    virtual void scheduleAppointment(const std::string& dateTime) = 0;
    virtual void cancelAppointment(int appointmentId) = 0;
};

class PatientRecord : public Appointable {
protected:
    std::string id_, name_, contact_;
    int age_;
    std::vector<int> appointmentIds_;
public:
    PatientRecord(std::string id, std::string name, int age, std::string contact);
    virtual ~PatientRecord() = default;

    const std::string& id() const { return id_; }
    const std::string& name() const { return name_; }
    const std::string& contact() const { return contact_; }
    int age() const { return age_; }
    const std::vector<int>& appointmentIds() const { return appointmentIds_; }

    void setName(const std::string& value);
    void setAge(int value);
    void setContact(const std::string& value);
    void addAppointmentId(int id);
    void removeAppointmentId(int id);

    virtual std::string category() const = 0;
    virtual std::string careNote() const = 0;
    virtual std::string typeCode() const = 0;
    virtual std::unique_ptr<PatientRecord> clone() const = 0;

    void scheduleAppointment(const std::string& dateTime) override;
    void cancelAppointment(int appointmentId) override;
};

class GeneralPatient final : public PatientRecord {
public:
    using PatientRecord::PatientRecord;
    std::string category() const override { return "General"; }
    std::string careNote() const override { return "Routine consultation"; }
    std::string typeCode() const override { return "G"; }
    std::unique_ptr<PatientRecord> clone() const override;
};

class EmergencyPatient final : public PatientRecord {
public:
    using PatientRecord::PatientRecord;
    std::string category() const override { return "Emergency"; }
    std::string careNote() const override { return "High priority assessment"; }
    std::string typeCode() const override { return "E"; }
    std::unique_ptr<PatientRecord> clone() const override;
};

class FollowUpPatient final : public PatientRecord {
public:
    using PatientRecord::PatientRecord;
    std::string category() const override { return "Follow-Up"; }
    std::string careNote() const override { return "Review previous consultation"; }
    std::string typeCode() const override { return "F"; }
    std::unique_ptr<PatientRecord> clone() const override;
};

struct Appointment {
    int id = 0;
    std::string patientId;
    std::string dateTime;
    std::string reason;
    std::string status = "Pending";
};

}
