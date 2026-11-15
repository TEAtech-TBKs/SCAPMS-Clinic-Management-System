#include "gui/MainWindow.h"
#include "controller/SearchEngine.h"
#include "utils/FileHandler.h"
#include <QWidget>
#include <QTabWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QPushButton>
#include <QHeaderView>
#include <QMessageBox>
#include <QInputDialog>
#include <QFileDialog>
#include <QMenuBar>
#include <QStatusBar>
#include <QShortcut>
#include <QKeySequence>
#include <QDateTime>
#include <QTimer>
#include <QTextEdit>
#include <QDialog>
#include <QDialogButtonBox>

namespace scapms {
MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    buildUi();
    connect(&timer_, &QTimer::timeout, this, [this] { refreshAll(); });
    timer_.start(15000);
    refreshAll();
}
void MainWindow::buildUi() {
    setWindowTitle("SCAPMS - Smart Clinic Appointment & Patient Management System");
    resize(1100,700);
    auto *fileMenu=menuBar()->addMenu("&File");
    auto *newAction=fileMenu->addAction("&New");
    auto *saveAction=fileMenu->addAction("&Save");
    auto *loadAction=fileMenu->addAction("&Load");
    fileMenu->addSeparator();
    auto *exitAction=fileMenu->addAction("E&xit");
    connect(newAction,&QAction::triggered,this,[this]{
        if(QMessageBox::question(this,"New","Clear current records?")==QMessageBox::Yes){manager_.clear();refreshAll();}
    });
    connect(saveAction,&QAction::triggered,this,&MainWindow::saveData);
    connect(loadAction,&QAction::triggered,this,&MainWindow::loadData);
    connect(exitAction,&QAction::triggered,this,&QWidget::close);
    auto *editMenu=menuBar()->addMenu("&Admin");
    auto *undoAction=editMenu->addAction("&Undo last operation");
    connect(undoAction,&QAction::triggered,this,[this]{
        if(!manager_.undoLast()) QMessageBox::information(this,"Undo","There is no operation to undo.");
        refreshAll();
    });
    auto *saveShortcut = new QShortcut(QKeySequence("Ctrl+S"), this);
    auto *findShortcut = new QShortcut(QKeySequence("Ctrl+F"), this);
    auto *quitShortcut = new QShortcut(QKeySequence("Ctrl+Q"), this);
    connect(saveShortcut, &QShortcut::activated, this, [this] { saveData(); });
    connect(findShortcut, &QShortcut::activated, this, [this] { searchPatients(); });
    connect(quitShortcut, &QShortcut::activated, this, [this] { close(); });

    auto *central=new QWidget; auto *outer=new QVBoxLayout(central);
    auto *dashboard=new QHBoxLayout;
    patientCount_=new QLabel; pendingCount_=new QLabel; emergencyCount_=new QLabel;
    for(auto *label:{patientCount_,pendingCount_,emergencyCount_}) {
        label->setMinimumHeight(55); label->setAlignment(Qt::AlignCenter);
        label->setStyleSheet("QLabel{background:#e8f0f8;border:1px solid #b7c9dc;border-radius:8px;font-size:16px;font-weight:bold;padding:8px;}");
        dashboard->addWidget(label);
    }
    outer->addLayout(dashboard);
    auto *tabs=new QTabWidget; outer->addWidget(tabs);
    // Patient records tab
    auto *patientTab=new QWidget; auto *pv=new QVBoxLayout(patientTab);
    patientsTable_=new QTableWidget(0,5); patientsTable_->setHorizontalHeaderLabels({"Patient ID","Name","Age","Contact","Category"});
    patientsTable_->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    patientsTable_->setSelectionBehavior(QAbstractItemView::SelectRows); patientsTable_->setEditTriggers(QAbstractItemView::NoEditTriggers);
    pv->addWidget(patientsTable_);
    auto *pb=new QHBoxLayout;
    auto *add=new QPushButton("Register patient"); auto *edit=new QPushButton("Edit selected");
    auto *del=new QPushButton("Delete selected"); auto *undo=new QPushButton("Undo");
    add->setToolTip("Register a new clinic patient"); edit->setToolTip("Edit selected patient information");
    del->setToolTip("Delete selected patient record"); undo->setToolTip("Reverse the most recent administrative change");
    pb->addWidget(add);pb->addWidget(edit);pb->addWidget(del);pb->addWidget(undo);pv->addLayout(pb);
    connect(add,&QPushButton::clicked,this,&MainWindow::addPatient);
    connect(edit,&QPushButton::clicked,this,&MainWindow::editPatient);
    connect(del,&QPushButton::clicked,this,&MainWindow::deletePatient);
    connect(undo,&QPushButton::clicked,this,[this]{if(!manager_.undoLast())QMessageBox::information(this,"Undo","Nothing to undo.");refreshAll();});
    tabs->addTab(patientTab,"Patient Records");

    // Appointment tab
    auto *apptTab=new QWidget; auto *av=new QVBoxLayout(apptTab);
    appointmentsTable_=new QTableWidget(0,5); appointmentsTable_->setHorizontalHeaderLabels({"Appointment ID","Patient ID","Date / Time","Reason","Status"});
    appointmentsTable_->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    appointmentsTable_->setEditTriggers(QAbstractItemView::NoEditTriggers); appointmentsTable_->setSelectionBehavior(QAbstractItemView::SelectRows);
    av->addWidget(appointmentsTable_);
    auto *ab=new QHBoxLayout; auto *schedule=new QPushButton("Schedule appointment"); auto *cancel=new QPushButton("Cancel selected");
    ab->addWidget(schedule);ab->addWidget(cancel);av->addLayout(ab);
    connect(schedule,&QPushButton::clicked,this,&MainWindow::scheduleAppointment);
    connect(cancel,&QPushButton::clicked,this,&MainWindow::cancelAppointment);
    tabs->addTab(apptTab,"Appointments");

    // Search and sort tab
    auto *searchTab=new QWidget; auto *sv=new QVBoxLayout(searchTab);
    auto *searchRow=new QHBoxLayout; searchEdit_=new QLineEdit; searchEdit_->setPlaceholderText("Enter patient ID, name or category");
    searchMethod_=new QComboBox; searchMethod_->addItems({"Linear search (ID)","Binary search (ID; sorts a copy)","Recursive search (name)","Category search"});
    auto *searchBtn=new QPushButton("Search"); searchRow->addWidget(searchEdit_);searchRow->addWidget(searchMethod_);searchRow->addWidget(searchBtn);sv->addLayout(searchRow);
    auto *sortRow=new QHBoxLayout; sortMethod_=new QComboBox;sortMethod_->addItems({"Selection Sort","Insertion Sort"});
    sortField_=new QComboBox;sortField_->addItems({"Name","ID","Age"});auto *sortBtn=new QPushButton("Sort patient list");
    sortRow->addWidget(new QLabel("Algorithm:"));sortRow->addWidget(sortMethod_);sortRow->addWidget(new QLabel("Sort by:"));sortRow->addWidget(sortField_);sortRow->addWidget(sortBtn);sv->addLayout(sortRow);
    auto *hint=new QLabel("Binary search requires ID order; the program sorts a temporary copy by ID before searching.");
    hint->setWordWrap(true);sv->addWidget(hint);sv->addStretch();
    connect(searchBtn,&QPushButton::clicked,this,&MainWindow::searchPatients);
    connect(sortBtn,&QPushButton::clicked,this,&MainWindow::sortPatients);
    tabs->addTab(searchTab,"Search & Sort");

    // Waiting queue tab
    auto *queueTab=new QWidget;auto *qv=new QVBoxLayout(queueTab);
    queueTable_=new QTableWidget(0,2);queueTable_->setHorizontalHeaderLabels({"Queue position","Patient ID"});
    queueTable_->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);qv->addWidget(queueTable_);
    auto *qb=new QHBoxLayout;auto *enqueue=new QPushButton("Add patient to queue");auto *next=new QPushButton("Call next patient");
    qb->addWidget(enqueue);qb->addWidget(next);qv->addLayout(qb);
    connect(enqueue,&QPushButton::clicked,this,[this]{
        bool ok=false;QString id=QInputDialog::getText(this,"Waiting queue","Patient ID:",QLineEdit::Normal,"",&ok);
        if(ok)try{manager_.addToQueue(id.toStdString());refreshQueue();}catch(const std::exception&e){showError(e.what());}
    });
    connect(next,&QPushButton::clicked,this,[this]{
        try{QString id=QString::fromStdString(manager_.callNext());QMessageBox::information(this,"Next patient","Call patient "+id);refreshQueue();}
        catch(const std::exception&e){showError(e.what());}
    });
    tabs->addTab(queueTab,"Waiting Queue");

    auto *reportTab=new QWidget;auto *rv=new QVBoxLayout(reportTab);
    auto *reportBtn=new QPushButton("Generate clinic reports");rv->addWidget(reportBtn);rv->addStretch();
    connect(reportBtn,&QPushButton::clicked,this,&MainWindow::showReports);
    tabs->addTab(reportTab,"Reports");
    setCentralWidget(central);
    statusLabel_=new QLabel("Ready");statusBar()->addWidget(statusLabel_);
}
void MainWindow::showError(const QString& m){QMessageBox::warning(this,"SCAPMS - Input / Operation Error",m);}
void MainWindow::refreshAll(){refreshPatients();refreshAppointments();refreshQueue();
    patientCount_->setText("Registered patients: "+QString::number(manager_.patients().size()));
    pendingCount_->setText("Pending appointments: "+QString::number(manager_.pendingCount()));
    emergencyCount_->setText("Emergency patients: "+QString::number(manager_.categoryCount("Emergency")));
}
void MainWindow::refreshPatients(){
    const auto& ps=manager_.patients();patientsTable_->setRowCount(static_cast<int>(ps.size()));
    for(int i=0;i<static_cast<int>(ps.size());++i){
        const auto&p=ps[static_cast<std::size_t>(i)];
        QStringList vals={QString::fromStdString(p->id()),QString::fromStdString(p->name()),QString::number(p->age()),QString::fromStdString(p->contact()),QString::fromStdString(p->category())};
        for(int c=0;c<5;++c){auto *item=new QTableWidgetItem(vals[c]);patientsTable_->setItem(i,c,item);}
        if(p->category()=="Emergency")for(int c=0;c<5;++c)patientsTable_->item(i,c)->setBackground(QColor("#ffe0df"));
    }
}
void MainWindow::refreshAppointments(){
    const auto& as=manager_.appointments();appointmentsTable_->setRowCount(static_cast<int>(as.size()));
    for(int i=0;i<static_cast<int>(as.size());++i){const auto&a=as[static_cast<std::size_t>(i)];
        QStringList v={QString::number(a.id),QString::fromStdString(a.patientId),QString::fromStdString(a.dateTime),QString::fromStdString(a.reason),QString::fromStdString(a.status)};
        for(int c=0;c<5;++c)appointmentsTable_->setItem(i,c,new QTableWidgetItem(v[c]));
    }
}
void MainWindow::refreshQueue(){
    auto q=manager_.waitingQueue();queueTable_->setRowCount(static_cast<int>(q.size()));int n=1;
    while(!q.empty()){queueTable_->setItem(n-1,0,new QTableWidgetItem(QString::number(n++)));queueTable_->setItem(n-2+1,1,new QTableWidgetItem(QString::fromStdString(q.front())));q.pop();}
}
void MainWindow::addPatient(){
    QDialog d(this);d.setWindowTitle("Register patient");QFormLayout form(&d);
    QLineEdit id,name,contact;QSpinBox age;age.setRange(0,120);QComboBox type;type.addItems({"General","Emergency","Follow-Up"});
    form.addRow("Patient ID:",&id);form.addRow("Full name:",&name);form.addRow("Age:",&age);form.addRow("Contact:",&contact);form.addRow("Category:",&type);
    QDialogButtonBox buttons(QDialogButtonBox::Ok|QDialogButtonBox::Cancel);form.addRow(&buttons);
    connect(&buttons,&QDialogButtonBox::accepted,&d,&QDialog::accept);connect(&buttons,&QDialogButtonBox::rejected,&d,&QDialog::reject);
    if(d.exec()==QDialog::Accepted)try{manager_.addPatient(id.text().toStdString(),name.text().toStdString(),age.value(),contact.text().toStdString(),type.currentText().toStdString());refreshAll();}
    catch(const std::exception&e){showError(e.what());}
}
void MainWindow::editPatient(){
    int row=patientsTable_->currentRow();if(row<0){showError("Select a patient first.");return;}
    QString id=patientsTable_->item(row,0)->text();auto*p=manager_.findPatient(id.toStdString());if(!p)return;
    QDialog d(this);d.setWindowTitle("Edit patient");QFormLayout f(&d);QLineEdit name(QString::fromStdString(p->name())),contact(QString::fromStdString(p->contact()));
    QSpinBox age;age.setRange(0,120);age.setValue(p->age());QComboBox type;type.addItems({"General","Emergency","Follow-Up"});type.setCurrentText(QString::fromStdString(p->category()));
    f.addRow("Name:",&name);f.addRow("Age:",&age);f.addRow("Contact:",&contact);f.addRow("Category:",&type);
    QDialogButtonBox b(QDialogButtonBox::Ok|QDialogButtonBox::Cancel);f.addRow(&b);connect(&b,&QDialogButtonBox::accepted,&d,&QDialog::accept);connect(&b,&QDialogButtonBox::rejected,&d,&QDialog::reject);
    if(d.exec()==QDialog::Accepted)try{manager_.updatePatient(id.toStdString(),name.text().toStdString(),age.value(),contact.text().toStdString(),type.currentText().toStdString());refreshAll();}catch(const std::exception&e){showError(e.what());}
}
void MainWindow::deletePatient(){
    int row=patientsTable_->currentRow();if(row<0){showError("Select a patient first.");return;}
    QString id=patientsTable_->item(row,0)->text();
    if(QMessageBox::question(this,"Delete patient","Delete patient "+id+"?")==QMessageBox::Yes)
        try{manager_.deletePatient(id.toStdString());refreshAll();}catch(const std::exception&e){showError(e.what());}
}
void MainWindow::scheduleAppointment(){
    QDialog d(this);d.setWindowTitle("Schedule appointment");QFormLayout f(&d);
    QLineEdit pid,reason;QDateTimeEdit dt(QDateTime::currentDateTime().addDays(1));dt.setCalendarPopup(true);
    f.addRow("Patient ID:",&pid);f.addRow("Date and time:",&dt);f.addRow("Reason:",&reason);
    QDialogButtonBox b(QDialogButtonBox::Ok|QDialogButtonBox::Cancel);f.addRow(&b);connect(&b,&QDialogButtonBox::accepted,&d,&QDialog::accept);connect(&b,&QDialogButtonBox::rejected,&d,&QDialog::reject);
    if(d.exec()==QDialog::Accepted)try{manager_.schedule(pid.text().toStdString(),dt.dateTime().toString("yyyy-MM-dd HH:mm").toStdString(),reason.text().toStdString());refreshAll();}catch(const std::exception&e){showError(e.what());}
}
void MainWindow::cancelAppointment(){
    int row=appointmentsTable_->currentRow();if(row<0){showError("Select an appointment first.");return;}
    bool ok=false;int id=appointmentsTable_->item(row,0)->text().toInt(&ok);if(ok)try{manager_.cancelAppointment(id);refreshAll();}catch(const std::exception&e){showError(e.what());}
}
void MainWindow::searchPatients(){
    QString key=searchEdit_->text().trimmed();if(key.isEmpty()){showError("Enter a search value.");return;}
    const auto& ps=manager_.patients();int found=-1;QString method=searchMethod_->currentText();
    if(method.startsWith("Linear"))found=SearchEngine::linearSearchById(ps,key.toStdString());
    else if(method.startsWith("Recursive"))found=SearchEngine::recursiveSearchByName(ps,key.toStdString());
    else if(method.startsWith("Binary")){
        std::vector<std::unique_ptr<PatientRecord>> copy;for(const auto&p:ps)copy.push_back(p->clone());
        SearchEngine::insertionSort(copy,"ID");found=SearchEngine::binarySearchById(copy,key.toStdString());
        if(found>=0)QMessageBox::information(this,"Search result","Found: "+QString::fromStdString(copy[static_cast<std::size_t>(found)]->name())+" (binary search on a temporary ID-sorted copy).");
        else QMessageBox::information(this,"Search result","No matching patient ID was found.");
        return;
    } else {
        for(std::size_t i=0;i<ps.size();++i)if(ps[i]->category()==key.toStdString()){found=static_cast<int>(i);break;}
    }
    if(found>=0)QMessageBox::information(this,"Search result","Found: "+QString::fromStdString(ps[static_cast<std::size_t>(found)]->name())+" | "+QString::fromStdString(ps[static_cast<std::size_t>(found)]->id()));
    else QMessageBox::information(this,"Search result","No matching patient was found.");
}
void MainWindow::sortPatients(){
    QString field=sortField_->currentText(),algorithm=sortMethod_->currentText();
    // Sort the actual manager collection through the required manual algorithms.
    auto& data=manager_.patients();
    if(algorithm.startsWith("Selection"))SearchEngine::selectionSort(data,field.toStdString());
    else SearchEngine::insertionSort(data,field.toStdString());
    refreshPatients();statusLabel_->setText(algorithm+" completed by "+field+".");
}
void MainWindow::saveData(){
    QString path=QFileDialog::getSaveFileName(this,"Save clinic data","","SCAPMS data (*.txt *.csv);;All files (*)");
    if(path.isEmpty())return;
    try{FileHandler::save(path.toStdString(),manager_.patients(),manager_.appointments());statusLabel_->setText("Saved: "+path);}
    catch(const std::exception&e){showError(e.what());}
}
void MainWindow::loadData(){
    QString path=QFileDialog::getOpenFileName(this,"Load clinic data","","SCAPMS data (*.txt *.csv);;All files (*)");
    if(path.isEmpty())return;
    try{std::vector<std::unique_ptr<PatientRecord>> p;std::vector<Appointment>a;FileHandler::load(path.toStdString(),p,a);manager_.replaceData(std::move(p),std::move(a));refreshAll();statusLabel_->setText("Loaded: "+path);}
    catch(const std::exception&e){showError(e.what());}
}
void MainWindow::showReports(){
    QString text;
    text += "PATIENT CATEGORY DISTRIBUTION\n";
    text += "General: "+QString::number(manager_.categoryCount("General"))+"\n";
    text += "Emergency: "+QString::number(manager_.categoryCount("Emergency"))+"\n";
    text += "Follow-Up: "+QString::number(manager_.categoryCount("Follow-Up"))+"\n\n";
    text += "PENDING APPOINTMENTS\n";
    for(const auto&a:manager_.appointments())if(a.status=="Pending")
        text += "Appointment "+QString::number(a.id)+" | Patient "+QString::fromStdString(a.patientId)+" | "+QString::fromStdString(a.dateTime)+"\n";
    text += "\nAPPOINTMENT SUMMARY\nTotal: "+QString::number(manager_.appointments().size());
    text += "\nPending: "+QString::number(manager_.pendingCount());
    int cancelled=0,completed=0;
    for(const auto&a:manager_.appointments()){if(a.status=="Cancelled")++cancelled;if(a.status=="Completed")++completed;}
    text += "\nCompleted: "+QString::number(completed)+"\nCancelled: "+QString::number(cancelled);
    QDialog d(this);d.setWindowTitle("Clinic reports");d.resize(600,500);QVBoxLayout l(&d);QTextEdit view;view.setReadOnly(true);view.setPlainText(text);l.addWidget(&view);
    QDialogButtonBox b(QDialogButtonBox::Close);l.addWidget(&b);connect(&b,&QDialogButtonBox::rejected,&d,&QDialog::reject);connect(&b,&QDialogButtonBox::accepted,&d,&QDialog::accept);d.exec();
}
}
