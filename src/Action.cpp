#include "../include/Action.h"
#include "../include/WareHouse.h"
#include "../include/Volunteer.h"
#include <typeinfo>
#include <iostream>
#include <string>
#include <vector>
extern WareHouse* backup;

BaseAction::BaseAction() : errorMsg(""), status(ActionStatus::ERROR){}

BaseAction::~BaseAction() {} 

ActionStatus BaseAction::getStatus() const { 
    return status;
}

void BaseAction::complete() {
    status = ActionStatus::COMPLETED;
}

void BaseAction::error(string errorMsg) {
    this->status = ActionStatus::ERROR;
    this->errorMsg = errorMsg;
}

string BaseAction::getErrorMsg() const {
    return "Error: " + errorMsg;
}


//SimulateStep
SimulateStep::SimulateStep(int numOfSteps) : BaseAction(), numOfSteps(numOfSteps){}

void SimulateStep::act(WareHouse &wareHouse) {
    vector<Order*> &whPendingOrders = wareHouse.getPendingOrders();
    vector<Volunteer*> &whVolunteers = wareHouse.getVolunteers();
    std::vector<Volunteer*>::iterator currVol;
    std::vector<int> moveOrderIds;
    bool isTaken;

    for (int i = 0; i < numOfSteps; i++){
        moveOrderIds.clear();
        currVol = whVolunteers.begin();
    for (Order* currOrder : whPendingOrders) {
        isTaken = false;
        switch (currOrder->getStatus()){
            case OrderStatus::PENDING: {
                while (currVol != whVolunteers.end() && !isTaken) {
                    if (dynamic_cast<CollectorVolunteer*>(*currVol) != nullptr && (*currVol)->canTakeOrder(*currOrder)){
                        isTaken = true;
                        currOrder->setCollectorId((*currVol)->getId());
                        currOrder->setStatus(OrderStatus::COLLECTING);
                        (*currVol)->acceptOrder(*currOrder);
                        moveOrderIds.push_back(currOrder->getId());
                    }
                    currVol++;
                }
                break;
            }
            case OrderStatus::COLLECTING: {
                for (unsigned int i = 0; i < whVolunteers.size() && !isTaken; i++) {
                    if (dynamic_cast<DriverVolunteer*>(whVolunteers[i]) != nullptr && (whVolunteers[i])->canTakeOrder(*currOrder)){
                        isTaken = true;
                        currOrder->setDriverId((whVolunteers[i])->getId());
                        currOrder->setStatus(OrderStatus::DELIVERING);
                        (whVolunteers[i])->acceptOrder(*currOrder);
                        moveOrderIds.push_back(currOrder->getId());
                    }
                }
                break;
            }
            default:
                break;
        }
    }

    for (unsigned int i = 0; i < moveOrderIds.size(); i++) {
        wareHouse.moveOrderToInProcess(moveOrderIds[i]);
    }

    for (Volunteer* steppingVol : whVolunteers) {
        steppingVol->step();

        if (steppingVol->getCompletedOrderId() != NO_ORDER) {
            if (dynamic_cast<CollectorVolunteer*>(steppingVol) != nullptr)
                wareHouse.moveOrderToPending(steppingVol->getCompletedOrderId());
            else
                wareHouse.moveOrderToCompleted(steppingVol->getCompletedOrderId());
        
            steppingVol->resetCompletedOrderId();
        }

        if (!steppingVol->hasOrdersLeft() && steppingVol->getActiveOrderId() == NO_ORDER)
            wareHouse.removeVolunteer(steppingVol->getId());
    }
    }
    complete();
}

SimulateStep *SimulateStep::clone() const {
    return new SimulateStep(*this);
}

string SimulateStep::toString() const {
    return "simulateStep " + std::to_string(numOfSteps) + " COMPLETED";
}



//AddOrder
AddOrder::AddOrder(int id) : BaseAction(), customerId(id){}

void AddOrder::act(WareHouse &wareHouse) {
    if (wareHouse.getNumOfCustomers() <= customerId || !wareHouse.getCustomer(customerId).canMakeOrder()) {
        error("Cannot place this order");
        std::cout << getErrorMsg() << std::endl;
    }
    else {
        Order* newOrder = new Order(wareHouse.getOrderCounter(), customerId, wareHouse.getCustomer(customerId).getCustomerDistance());
        wareHouse.getCustomer(customerId).addOrder(newOrder->getId());
        wareHouse.addOrder(newOrder);
        complete();
    }
}

AddOrder *AddOrder::clone() const {
    return new AddOrder(*this);
}

string AddOrder::toString() const {
    switch (getStatus()) {
        case ActionStatus::COMPLETED:
            return "order " + std::to_string(customerId) + " COMPLETED";
            break;
        case ActionStatus::ERROR:
            return "order " + std::to_string(customerId) + " ERROR";
            break;
        default:
            return "";
            break;
    }
}


//AddCustomer
CustomerType AddCustomer::convertCustomerType(const string &customerType) {
    if (customerType == "Civilian" || customerType == "civilian")
        return CustomerType::Civilian;
    else
        return CustomerType::Soldier;
}

AddCustomer::AddCustomer(const string &customerName, const string &customerType, int distance, int maxOrders) : BaseAction(), customerName(customerName), customerType(convertCustomerType(customerType)), distance(distance), maxOrders(maxOrders) {
}

void AddCustomer::act(WareHouse &wareHouse) {
    Customer* newCustomer;
    switch (customerType) {
        case CustomerType::Civilian:
            newCustomer = new CivilianCustomer(wareHouse.getNumOfCustomers(), customerName, distance, maxOrders);
            wareHouse.getCustomers().push_back(newCustomer);
            break;
        case CustomerType::Soldier:
            newCustomer = new SoldierCustomer(wareHouse.getNumOfCustomers(), customerName, distance, maxOrders);
            wareHouse.getCustomers().push_back(newCustomer);
            break;
    }
    wareHouse.increaseCustomerCounter();
    complete();
}

AddCustomer *AddCustomer::clone() const {
    return new AddCustomer(*this);
}

string AddCustomer::toString() const {
    switch (customerType){
        case CustomerType::Civilian:
            return "Customer " + customerName + " Civilian " + std::to_string(distance) + " " + std::to_string(maxOrders) + " COMPLETED";
            break;
        case CustomerType::Soldier:
            return "Customer " + customerName + " Soldier " + std::to_string(distance) + " " + std::to_string(maxOrders) + " COMPLETED";
            break;
        default:
            return "";
            break;
    }
}


//PrintOrderStatus 
PrintOrderStatus::PrintOrderStatus(int id) : BaseAction(), orderId(id){}

void PrintOrderStatus::act(WareHouse &wareHouse) {
    if (wareHouse.getOrderCounter() <= orderId) {
        error("Order doesn't exist");
        std::cout << getErrorMsg() << std::endl;
    }
    else {
        std::cout << wareHouse.getOrder(orderId).toString() << std::endl;
        complete();
    }
}

PrintOrderStatus *PrintOrderStatus::clone() const {
    return new PrintOrderStatus(*this);
}

string PrintOrderStatus::toString() const {
    switch (getStatus()) {
        case ActionStatus::COMPLETED:
            return "orderStatus " + std::to_string(orderId) + " COMPLETED";
            break;
        case ActionStatus::ERROR:
            return "orderStatus " + std::to_string(orderId) + " ERROR";
            break;
        default:
            return "";
            break;
    }
}



//PrintCustomerStatus  
PrintCustomerStatus::PrintCustomerStatus(int id) : BaseAction(), customerId(id){}

void PrintCustomerStatus::act(WareHouse &wareHouse) {
    if (wareHouse.getNumOfCustomers() <= customerId) {
        error("Customer doesn't exist");
        std::cout << getErrorMsg() << std::endl;
    }
    else {
        std::cout << "CustomerID: " + std::to_string(customerId) << std::endl;
        for (int currOrderId : wareHouse.getCustomer(customerId).getOrdersIds()) {
            std::cout << "OrderID: " + std::to_string(currOrderId) << std::endl;
            std::cout << "OrderStatus: " + wareHouse.getOrder(currOrderId).getStatusString() << std::endl;
        }
        int ordersLeft = wareHouse.getCustomer(customerId).getMaxOrders() - wareHouse.getCustomer(customerId).getNumOrders();
        std::cout << "numOrdersLeft: " + std::to_string(ordersLeft) << std::endl;
        complete();
    }
}

PrintCustomerStatus *PrintCustomerStatus::clone() const {
    return new PrintCustomerStatus(*this);
}

string PrintCustomerStatus::toString() const {
    switch (getStatus()) {
        case ActionStatus::COMPLETED:
            return "customerStatus " + std::to_string(customerId) + " COMPLETED";
            break;
        case ActionStatus::ERROR:
            return "customerStatus " + std::to_string(customerId) + " ERROR";
            break;
        default:
            return "";
            break;
    }
}



//PrintVolunteerStatus
PrintVolunteerStatus::PrintVolunteerStatus(int id) : BaseAction(), volunteerId(id){}

void PrintVolunteerStatus::act(WareHouse &wareHouse) {
    if (!wareHouse.volunteerExists(volunteerId)){
        error("Volunteer doesn't exist");
        std::cout << getErrorMsg() << std::endl;
    }
    else {
        std::cout << wareHouse.getVolunteer(volunteerId).toString() << std::endl;
        complete();
    }
}

PrintVolunteerStatus *PrintVolunteerStatus::clone() const {
    return new PrintVolunteerStatus(*this);
}

string PrintVolunteerStatus::toString() const {
    switch (getStatus()) {
        case ActionStatus::COMPLETED:
            return "volunteerStatus " + std::to_string(volunteerId) + " COMPLETED";
            break;
        case ActionStatus::ERROR:
            return "volunteerStatus " + std::to_string(volunteerId) + " ERROR";
            break;
        default:
            return "";
            break;
    }
}



//PrintActionsLog
PrintActionsLog::PrintActionsLog() : BaseAction(){}

void PrintActionsLog::act(WareHouse &wareHouse) {
    for (BaseAction* currAction : wareHouse.getActions()) {
        std::cout << currAction->toString() << std::endl;
    }
    complete();
}

PrintActionsLog *PrintActionsLog::clone() const {
    return new PrintActionsLog(*this);
}

string PrintActionsLog::toString() const {
    return "log COMPLETED";
}



//Close
Close::Close() : BaseAction(){}

void Close::act(WareHouse &wareHouse) {
    for (Order* currOrder : wareHouse.getPendingOrders())
        std::cout << "OrderID: " << std::to_string(currOrder->getId()) << ", CustomerID: " << std::to_string(currOrder->getCustomerId()) << ", OrderStatus: " << currOrder->getStatusString() << std::endl;
    for (Order* currOrder : wareHouse.getInProcessOrders())
        std::cout << "OrderID: " << std::to_string(currOrder->getId()) << ", CustomerID: " << std::to_string(currOrder->getCustomerId()) << ", OrderStatus: " << currOrder->getStatusString() << std::endl;
    for (Order* currOrder : wareHouse.getCompletedOrders())
        std::cout << "OrderID: " << std::to_string(currOrder->getId()) << ", CustomerID: " << std::to_string(currOrder->getCustomerId()) << ", OrderStatus: " << currOrder->getStatusString() << std::endl;
    wareHouse.close();
    complete();
}

Close *Close::clone() const {
    return new Close(*this);
}

string Close::toString() const {
    return "close COMPLETED";
}



//BackupWareHouse
BackupWareHouse::BackupWareHouse() : BaseAction(){}

void BackupWareHouse::act(WareHouse &wareHouse) {
    if (backup == nullptr)
        backup = new WareHouse(wareHouse);
    else
        *backup = wareHouse;
    complete();
}

BackupWareHouse *BackupWareHouse::clone() const {
    return new BackupWareHouse(*this);
}

string BackupWareHouse::toString() const {
    return "backup COMPLETED";
}



//RestoreWareHouse
RestoreWareHouse::RestoreWareHouse() : BaseAction(){}

void RestoreWareHouse::act(WareHouse &wareHouse) {
    if (backup == nullptr){
        error("No backup available");
        std::cout << getErrorMsg() << std::endl;
    }
    else {
        wareHouse = *backup;
        complete();
    }
}

RestoreWareHouse *RestoreWareHouse::clone() const {
    return new RestoreWareHouse(*this);
}

string RestoreWareHouse::toString() const {
    switch (getStatus()) {
        case ActionStatus::COMPLETED:
            return "restore COMPLETED";
            break;
        case ActionStatus::ERROR:
            return "restore ERROR";
            break;
        default:
            return "";
            break;
    }
}
