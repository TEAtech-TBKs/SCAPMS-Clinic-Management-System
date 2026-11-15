#pragma once
#include <QMainWindow>
#include <QTableWidget>
#include <QLabel>
#include <QComboBox>
#include <QLineEdit>
#include <QSpinBox>
#include <QDateTimeEdit>
#include <QTimer>
#include "controller/ClinicManager.h"

namespace scapms {
class MainWindow : public QMainWindow {
    Q_OBJECT
    ClinicManager manager_;
    QTableWidget *patientsTable_, *appointmentsTable_;
    QTableWidget *queueTable_;
    QLabel *statusLabel_, *patientCount_, *pendingCount_, *emergencyCount_;
    QLineEdit *searchEdit_;
    QComboBox *searchMethod_, *sortMethod_, *sortField_;
    QTimer timer_;

    void buildUi();
    void refreshAll();
    void refreshPatients();
    void refreshAppointments();
    void refreshQueue();
    void addPatient();
    void editPatient();
    void deletePatient();
    void scheduleAppointment();
    void cancelAppointment();
    void searchPatients();
    void sortPatients();
    void saveData();
    void loadData();
    void showReports();
    void showError(const QString& message);
public:
    explicit MainWindow(QWidget *parent=nullptr);
};
}
