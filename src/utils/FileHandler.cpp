#include "utils/FileHandler.h"
#include <fstream>
#include <sstream>
#include <stdexcept>

namespace scapms {
void FileHandler::save(const std::string& path,
                       const std::vector<std::unique_ptr<PatientRecord>>& patients,
                       const std::vector<Appointment>& appointments) {
    std::ofstream out(path);
    if (!out) throw std::runtime_error("Could not open the selected file for writing.");
    out << "PATIENTS\n";
    for (const auto& p:patients)
        out << p->typeCode() << '|' << p->id() << '|' << p->name() << '|'
            << p->age() << '|' << p->contact() << '\n';
    out << "APPOINTMENTS\n";
    for (const auto& a:appointments)
        out << a.id << '|' << a.patientId << '|' << a.dateTime << '|'
            << a.reason << '|' << a.status << '\n';
    if (!out) throw std::runtime_error("An error occurred while saving the file.");
}
void FileHandler::load(const std::string& path,
                       std::vector<std::unique_ptr<PatientRecord>>& patients,
                       std::vector<Appointment>& appointments) {
    std::ifstream in(path);
    if (!in) throw std::runtime_error("Could not open the selected file.");
    std::vector<std::unique_ptr<PatientRecord>> newPatients;
    std::vector<Appointment> newAppointments;
    std::string line, section;
    while (std::getline(in,line)) {
        if (line=="PATIENTS" || line=="APPOINTMENTS") { section=line; continue; }
        if (line.empty()) continue;
        std::stringstream ss(line); std::vector<std::string> f; std::string part;
        while(std::getline(ss,part,'|')) f.push_back(part);
        if (section=="PATIENTS" && f.size()==5) {
            int age=std::stoi(f[3]);
            if (age<0 || age>120) throw std::runtime_error("Invalid age in data file.");
            if (f[0]=="E") newPatients.push_back(std::make_unique<EmergencyPatient>(f[1],f[2],age,f[4]));
            else if (f[0]=="F") newPatients.push_back(std::make_unique<FollowUpPatient>(f[1],f[2],age,f[4]));
            else newPatients.push_back(std::make_unique<GeneralPatient>(f[1],f[2],age,f[4]));
        } else if (section=="APPOINTMENTS" && f.size()==5) {
            Appointment a; a.id=std::stoi(f[0]); a.patientId=f[1]; a.dateTime=f[2]; a.reason=f[3]; a.status=f[4];
            newAppointments.push_back(a);
            for (auto& p:newPatients) if(p->id()==a.patientId && a.status!="Cancelled") p->addAppointmentId(a.id);
        } else throw std::runtime_error("The selected file contains an invalid record.");
    }
    patients=std::move(newPatients); appointments=std::move(newAppointments);
}
}
