#include "../include/WareHouse.h"
#include "../include/Volunteer.h"
#include "../include/Action.h"
#include <iostream>
#include <fstream>
#include <sstream>
extern WareHouse* backup;

//Destructor
WareHouse::~WareHouse() {
    clear();
}

//Copy Assignment Operator
WareHouse &WareHouse::operator=(const WareHouse& other) {
    if (this != &other) {
        clear();

        isOpen = other.isOpen;
        copy(other);
        customerCounter = other.customerCounter;
        volunteerCounter = other.volunteerCounter;
        orderCounter = other.orderCounter;
    }
    return *this;
}

//Copy Constructor
WareHouse::WareHouse(const WareHouse& other) : isOpen(other.isOpen), actionsLog(vector<BaseAction*>()), volunteers(vector<Volunteer*>()), pendingOrders(vector<Order*>()), inProcessOrders(vector<Order*>()), completedOrders(vector<Order*>()), customers(vector<Customer*>()), customerCounter(other.customerCounter), volunteerCounter(other.volunteerCounter), orderCounter(other.orderCounter) {
    copy(other);
}

//Move Assignment Operator
WareHouse &WareHouse::operator=(WareHouse&& other){
    if (this != &other) {
        clear();

        isOpen = other.isOpen;
        actionsLog = std::move(other.actionsLog);
        volunteers = std::move(other.volunteers);
        pendingOrders = std::move(other.pendingOrders);
        inProcessOrders = std::move(other.inProcessOrders);
        completedOrders = std::move(other.completedOrders);
        customers = other.customers;
        customerCounter = other.customerCounter;
        volunteerCounter = other.volunteerCounter;
        orderCounter = other.orderCounter;        
    }
    return *this;
}

//Move Constructor
WareHouse::WareHouse(WareHouse&& other) :
    isOpen(other.isOpen),
    actionsLog(std::move(other.actionsLog)),
    volunteers(std::move(other.volunteers)),
    pendingOrders(std::move(other.pendingOrders)),
    inProcessOrders(std::move(other.inProcessOrders)),
    completedOrders(std::move(other.completedOrders)),
    customers(std::move(other.customers)),
    customerCounter(other.customerCounter),
    volunteerCounter(other.volunteerCounter),
    orderCounter(other.orderCounter)
    {
}


void WareHouse::clear(){
    for (BaseAction* action : actionsLog) {
        delete action;
        action = nullptr;
    }
    actionsLog.clear();
    for (Volunteer* volunteer : volunteers) {
        delete volunteer;
        volunteer = nullptr;
    }
    volunteers.clear();
    for (Order* order : pendingOrders) {
        delete order;
        order = nullptr;
    }
    pendingOrders.clear();
    for (Order* order : inProcessOrders) {
        delete order;
        order = nullptr;
    }
    inProcessOrders.clear();
    for (Order* order : completedOrders) {
        delete order;
        order = nullptr;
    }
    completedOrders.clear();
    for (Customer* customer : customers) {
        delete customer;
        customer = nullptr;
    }
    customers.clear();
}

void WareHouse::copy(const WareHouse& other) {
    for (BaseAction* action : other.actionsLog)
        actionsLog.push_back(action->clone());
    for (Volunteer* volunteer : other.volunteers)
        volunteers.push_back(volunteer->clone());
    for (Order* order : other.pendingOrders)
        pendingOrders.push_back(new Order(*order));
    for (Order* order : other.inProcessOrders)
        inProcessOrders.push_back(new Order(*order));
    for (Order* order : other.completedOrders)
        completedOrders.push_back(new Order(*order));
    for (Customer* customer : other.customers)
        customers.push_back(customer->clone());
}


 

WareHouse::WareHouse(const string &configFilePath) : isOpen(true), actionsLog(vector<BaseAction*>()), volunteers(vector<Volunteer*>()), pendingOrders(vector<Order*>()), inProcessOrders(vector<Order*>()), completedOrders(vector<Order*>()), customers(vector<Customer*>()), customerCounter(0), volunteerCounter(0), orderCounter(0) {
    std::ifstream file(configFilePath);
    string line;

    while (getline(file, line)) {
        if (line.empty() || line[0] == '#') continue;
        std::istringstream stringStream(line);
        std::string type;
        stringStream >> type;
        if (type == "customer") {
            std::string name, type;
            int distance, maxOrders;
            stringStream >> name >> type >> distance >> maxOrders;
            if (type == "civilian"){
                CivilianCustomer* newCustomer = new CivilianCustomer(customerCounter, name, distance, maxOrders);
                customers.push_back(newCustomer);
            }
            else {
                SoldierCustomer* newCustomer = new SoldierCustomer(customerCounter, name, distance, maxOrders);
                customers.push_back(newCustomer);
            }
            customerCounter++;
        }
        else if (type == "volunteer") {
            std::string name ,role;
            int coolDown, maxDist, distPerStep, maxOrders;
            stringStream >> name >> role;
            if (role == "collector") {
                stringStream >> coolDown;
                CollectorVolunteer* newVolunteer = new CollectorVolunteer(volunteerCounter, name, coolDown);
                volunteers.push_back(newVolunteer);
            }
            else if (role == "limited_collector") {
                stringStream >> coolDown >> maxOrders;
                LimitedCollectorVolunteer* newVolunteer = new LimitedCollectorVolunteer(volunteerCounter, name, coolDown, maxOrders);
                volunteers.push_back(newVolunteer);
            }
            else if (role == "driver") {
                stringStream >> maxDist >> distPerStep;
                DriverVolunteer* newVolunteer = new DriverVolunteer(volunteerCounter, name, maxDist, distPerStep);
                volunteers.push_back(newVolunteer);
            }
            else {
                stringStream >> maxDist >> distPerStep >> maxOrders;
                LimitedDriverVolunteer* newVolunteer = new LimitedDriverVolunteer(volunteerCounter, name, maxDist, distPerStep, maxOrders);
                volunteers.push_back(newVolunteer);
            }
            volunteerCounter++;
        }
    }
}

void WareHouse::start() {
    open();
    std::string input = "";
    std::string command = "";

    while (command != "close"){
        std::getline(std::cin, input);
        std::istringstream stringStream(input);
        stringStream >> command;

        if (command == "step") {
            int numOfSteps;
            stringStream >> numOfSteps;
            SimulateStep* newAction = new SimulateStep(numOfSteps);
            newAction->act(*this);
            actionsLog.push_back(newAction);
        }
        else if (command == "order"){
            int customerId;
            stringStream >> customerId;
            AddOrder* newAction = new AddOrder(customerId);
            newAction->act(*this);
            actionsLog.push_back(newAction);
        }
        else if (command == "customer"){
            std::string name, type;
            int distance, maxOrders;
            stringStream >> name >> type >> distance >> maxOrders;
            AddCustomer* newAction = new AddCustomer(name, type, distance, maxOrders);
            newAction->act(*this);
            actionsLog.push_back(newAction);
        }
        else if (command == "orderStatus"){
            int orderId;
            stringStream >> orderId;
            PrintOrderStatus* newAction = new PrintOrderStatus(orderId);
            newAction->act(*this);
            actionsLog.push_back(newAction);
        }
        else if (command == "customerStatus"){
            int customerId;
            stringStream >> customerId;
            PrintCustomerStatus* newAction = new PrintCustomerStatus(customerId);
            newAction->act(*this);
            actionsLog.push_back(newAction);
        }
        else if (command == "volunteerStatus"){
            int volunteerId;
            stringStream >> volunteerId;
            PrintVolunteerStatus* newAction = new PrintVolunteerStatus(volunteerId);
            newAction->act(*this);
            actionsLog.push_back(newAction);
        }
        else if (command == "log"){
            PrintActionsLog* newAction = new PrintActionsLog();
            newAction->act(*this);
            actionsLog.push_back(newAction);
        }
        else if (command == "backup"){
            BackupWareHouse* newAction = new BackupWareHouse();
            newAction->act(*this);
            actionsLog.push_back(newAction);
        }
        else if (command == "restore"){
            RestoreWareHouse* newAction = new RestoreWareHouse();
            newAction->act(*this);
            actionsLog.push_back(newAction);
        }
        else if (command == "close"){
            Close newAction = Close();
            newAction.act(*this);
        }
    }

}

void WareHouse::addOrder(Order* order) {
    pendingOrders.push_back(order);
    orderCounter++;
}

void WareHouse::addAction(BaseAction* action) {
    actionsLog.push_back(action);
}

Customer &WareHouse::getCustomer(int customerId) const {
    for (Customer* customer : customers)
        if (customer->getId() == customerId) 
            return *customer;
    throw std::runtime_error("Customer not found.");
}

Volunteer &WareHouse::getVolunteer(int volunteerId) const {
    for (Volunteer* volunteer : volunteers)
        if (volunteer->getId() == volunteerId) 
            return *volunteer;
    throw std::runtime_error("Volunteer not found.");
}

vector<Order*> &WareHouse::getPendingOrders() {
    return pendingOrders;
}

vector<Order*> &WareHouse::getInProcessOrders() {
    return inProcessOrders;
}

vector<Order*> &WareHouse::getCompletedOrders() {
    return completedOrders;
}

bool WareHouse::volunteerExists(int volunteerId) const {
    for (Volunteer* volunteer : volunteers)
        if (volunteer->getId() == volunteerId) 
            return true;
    return false;
}

vector<Volunteer*> &WareHouse::getVolunteers() {
    return volunteers;
}

vector<Customer*> &WareHouse::getCustomers() {
    return customers;
}

int WareHouse::getNumOfCustomers() const {
    return customerCounter;
}

int WareHouse::getOrderCounter() const {
    return orderCounter;
}

void WareHouse::moveOrderToInProcess(int orderId) {
    for (unsigned i = 0; i < pendingOrders.size(); i++) {
        if (pendingOrders[i]->getId() == orderId) {
            inProcessOrders.push_back(pendingOrders[i]);
            pendingOrders.erase(pendingOrders.begin() + i);
            return;
        }
    }
}

void WareHouse::moveOrderToPending(int orderId) {
    for (unsigned i = 0; i < inProcessOrders.size(); i++) {
        if (inProcessOrders[i]->getId() == orderId) {
            pendingOrders.push_back(inProcessOrders[i]);
            inProcessOrders.erase(inProcessOrders.begin() + i);
            return;
        }
    }
}

void WareHouse::moveOrderToCompleted(int orderId) {
    for (unsigned i = 0; i < inProcessOrders.size(); i++) {
        if (inProcessOrders[i]->getId() == orderId) {
            inProcessOrders[i]->setStatus(OrderStatus::COMPLETED);
            completedOrders.push_back(inProcessOrders[i]);
            inProcessOrders.erase(inProcessOrders.begin() + i);
            return;
        }
    }
}

void WareHouse::increaseCustomerCounter() {
    customerCounter++;
}

void WareHouse::removeVolunteer(int volunteerId) {
    for (unsigned i = 0; i < volunteers.size(); i++) {
        if (volunteers[i]->getId() == volunteerId) {
            delete volunteers[i];
            volunteers.erase(volunteers.begin() + i);
            return;
        }
    }
}

Order &WareHouse::getOrder(int orderId) const {
    for (Order* order : pendingOrders)
        if (order->getId() == orderId) 
            return *order;
    for (Order* order : inProcessOrders)
        if (order->getId() == orderId) 
            return *order;
    for (Order* order : completedOrders)
        if (order->getId() == orderId) 
            return *order;
    return *pendingOrders[0];
}

const vector<BaseAction*> &WareHouse::getActions() const {
    return actionsLog;
}

void WareHouse::close() {
    isOpen = false;
    std::cout << "Warehouse is closed!" << std::endl;
}

void WareHouse::open() {
    isOpen = true;
    std::cout << "Warehouse is open!" << std::endl;
}