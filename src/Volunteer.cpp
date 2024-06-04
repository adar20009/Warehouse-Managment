#include "../include/Volunteer.h"
#include <iostream>
#include <string>
#include <vector>


Volunteer::Volunteer(int id, const string &name) : completedOrderId(NO_ORDER), activeOrderId(NO_ORDER), id(id), name(name) {
}

Volunteer::~Volunteer() {}

int Volunteer::getId() const {
    return id;
}

const string &Volunteer::getName() const {
    return name;
}

int Volunteer::getActiveOrderId() const {
    return activeOrderId;
}

void Volunteer::resetCompletedOrderId() {
    completedOrderId = NO_ORDER;
}

int Volunteer::getCompletedOrderId() const {
    return completedOrderId;
}

bool Volunteer::isBusy() const {
    return activeOrderId != NO_ORDER;
}



//CollectorVolunteer
CollectorVolunteer::CollectorVolunteer(int id, const string &name, int coolDown) : Volunteer(id, name), coolDown(coolDown), timeLeft(0){}

CollectorVolunteer *CollectorVolunteer::clone() const {
    return new CollectorVolunteer(*this);
}

void CollectorVolunteer::step() {
    if (decreaseCoolDown()) {
        completedOrderId = activeOrderId;
        activeOrderId = NO_ORDER;
    }
}

int CollectorVolunteer::getCoolDown() const {
    return coolDown;
}

int CollectorVolunteer::getTimeLeft() const {
    return timeLeft;
}

bool CollectorVolunteer::decreaseCoolDown() {
    if(activeOrderId != NO_ORDER){
        timeLeft--;
        return timeLeft == 0;
    }
    return false;
}

bool CollectorVolunteer::hasOrdersLeft() const {
    return true;
}

bool CollectorVolunteer::canTakeOrder(const Order &order) const {
    return !isBusy();
}

void CollectorVolunteer::acceptOrder(const Order &order) {
    activeOrderId = order.getId();
    timeLeft = coolDown;
}

string CollectorVolunteer::toString() const {
    string orderIdString;
    string timeLeftString;
    string isBusyString;
    if (activeOrderId == NO_ORDER){
        orderIdString = "None";
        timeLeftString = "None";
        isBusyString = "False";
    }
    else {
        orderIdString = std::to_string(activeOrderId);
        timeLeftString = std::to_string(timeLeft);
        isBusyString = "True";
    }
    
    return "VolunteerID: " + std::to_string(getId()) + "\nisBusy: " + isBusyString + "\nOrderID: " + orderIdString + "\nTimeLeft: " + timeLeftString + "\nOrdersLeft: No Limit";
}


//LimitedCollectorVolunteer
LimitedCollectorVolunteer::LimitedCollectorVolunteer(int id, const string &name, int coolDown, int maxOrders) : CollectorVolunteer(id, name, coolDown), maxOrders(maxOrders), ordersLeft(maxOrders) {}

LimitedCollectorVolunteer *LimitedCollectorVolunteer::clone() const {
    return new LimitedCollectorVolunteer(*this);
}

bool LimitedCollectorVolunteer::hasOrdersLeft() const {
    return ordersLeft > 0;
}

bool LimitedCollectorVolunteer::canTakeOrder(const Order &order) const {
    return !isBusy() && hasOrdersLeft();
}

void LimitedCollectorVolunteer::acceptOrder(const Order &order) {
    CollectorVolunteer::acceptOrder(order);
    ordersLeft--;
}

int LimitedCollectorVolunteer::getMaxOrders() const {
    return maxOrders;
}

int LimitedCollectorVolunteer::getNumOrdersLeft() const {
    return ordersLeft;
}

string LimitedCollectorVolunteer::toString() const {
    string orderIdString;
    string timeLeftString;
    string isBusyString;
    if (getActiveOrderId() == NO_ORDER){
        orderIdString = "None";
        timeLeftString = "None";
        isBusyString = "False";
    }
    else {
        orderIdString = std::to_string(getActiveOrderId());
        timeLeftString = std::to_string(getTimeLeft());
        isBusyString = "True";
    }
    
    return "VolunteerID: " + std::to_string(getId()) + "\nisBusy: " + isBusyString + "\nOrderID: " + orderIdString + "\nTimeLeft: " + timeLeftString + "\nOrdersLeft: " + std::to_string(getNumOrdersLeft());
}


//DriverVolunteer
DriverVolunteer::DriverVolunteer(int id, const string &name, int maxDistance, int distancePerStep) : Volunteer(id, name), maxDistance(maxDistance), distancePerStep(distancePerStep), distanceLeft(0) {}

DriverVolunteer *DriverVolunteer::clone() const {
    return new DriverVolunteer(*this);
}

int DriverVolunteer::getDistanceLeft() const {
    return distanceLeft;
}

int DriverVolunteer::getMaxDistance() const {
    return maxDistance;
}

int DriverVolunteer::getDistancePerStep() const {
    return distancePerStep;
}

bool DriverVolunteer::decreaseDistanceLeft() {
    if(isBusy()){
        distanceLeft -= distancePerStep;
        return distanceLeft <= 0;
    }
    return false;
}

bool DriverVolunteer::hasOrdersLeft() const {
    return true;
}

bool DriverVolunteer::canTakeOrder(const Order &order) const {
    return !isBusy() && order.getDistance() <= maxDistance;
}

void DriverVolunteer::acceptOrder(const Order &order) {
    activeOrderId = order.getId();
    distanceLeft = order.getDistance();
}

void DriverVolunteer::step() {
    if (decreaseDistanceLeft()) {
        completedOrderId = activeOrderId;
        activeOrderId = NO_ORDER;
    }
}

string DriverVolunteer::toString() const {
    string orderIdString;
    string distanceLeftString;
    string isBusyString;
    if (getActiveOrderId() == NO_ORDER){
        orderIdString = "None";
        distanceLeftString = "None";
        isBusyString = "False";
    }
    else {
        orderIdString = std::to_string(getActiveOrderId());
        distanceLeftString = std::to_string(getDistanceLeft());
        isBusyString = "True";
    }
    
    return "VolunteerID: " + std::to_string(getId()) + "\nisBusy: " + isBusyString + "\nOrderID: " + orderIdString + "\nDistanceLeft: " + distanceLeftString + "\nOrdersLeft: NoLimit";
}


//LimitedDriverVolunteer
LimitedDriverVolunteer::LimitedDriverVolunteer(int id, const string &name, int maxDistance, int distancePerStep,int maxOrders) : DriverVolunteer(id, name, maxDistance, distancePerStep), maxOrders(maxOrders), ordersLeft(maxOrders) {}

LimitedDriverVolunteer *LimitedDriverVolunteer::clone() const {
    return new LimitedDriverVolunteer(*this);
}

int LimitedDriverVolunteer::getMaxOrders() const {
    return maxOrders;
}

int LimitedDriverVolunteer::getNumOrdersLeft() const {
    return ordersLeft;
}

bool LimitedDriverVolunteer::hasOrdersLeft() const {
    return ordersLeft > 0;
}

bool LimitedDriverVolunteer::canTakeOrder(const Order &order) const {
    return !isBusy() && hasOrdersLeft() && order.getDistance() <= getMaxDistance();
}

void LimitedDriverVolunteer::acceptOrder(const Order &order) {
    DriverVolunteer::acceptOrder(order);
    ordersLeft--;
}

string LimitedDriverVolunteer::toString() const {
    string orderIdString;
    string distanceLeftString;
    string isBusyString;
    if (getActiveOrderId() == NO_ORDER){
        orderIdString = "None";
        distanceLeftString = "None";
        isBusyString = "False";
    }
    else {
        orderIdString = std::to_string(getActiveOrderId());
        distanceLeftString = std::to_string(getDistanceLeft());
        isBusyString = "True";
    }
    
    return "VolunteerID: " + std::to_string(getId()) + "\nisBusy: " + isBusyString + "\nOrderID: " + orderIdString + "\nDistanceLeft: " + distanceLeftString + "\nOrdersLeft: " + std::to_string(getNumOrdersLeft());
}

