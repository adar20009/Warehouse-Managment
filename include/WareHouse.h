#pragma once
#include <string>
#include <vector>

#include "Order.h"
#include "Customer.h"

class BaseAction;
class Volunteer;

// Warehouse responsible for Volunteers, Customers Actions, and Orders.


class WareHouse {

    public:
        WareHouse(const string &configFilePath);

        ~WareHouse(); //Destructor
        WareHouse &operator=(const WareHouse& other); //Copy assignment operator
        WareHouse(const WareHouse& other); //Copy constructor
        WareHouse &operator=(WareHouse&& other); //Move assignment operator
        WareHouse(WareHouse&& other); //Move constructor

        void clear(); //Clear all the vectors
        void copy(const WareHouse& other); //Copy all the vectors
        void start();
        void addOrder(Order* order);
        void addAction(BaseAction* action);
        Customer &getCustomer(int customerId) const;
        Volunteer &getVolunteer(int volunteerId) const;
        vector<Order*> &getPendingOrders(); //Return a reference to the pendingOrders vector
        vector<Order*> &getInProcessOrders(); //Return a reference to the inProcessOrders orders vector
        bool volunteerExists(int volunteerId) const; //Return true if volunteerId exists in volunteers vector, false otherwise
        vector<Order*> &getCompletedOrders(); //Return a reference to the completedOrders orders vector
        vector<Volunteer*> &getVolunteers(); //Return a reference to the volunteers vector
        vector<Customer*> &getCustomers(); //Return a reference to the customers vector
        int getNumOfCustomers() const; //Return the number of customers
        int getOrderCounter () const; //Return the orderCounter
        void moveOrderToInProcess(int orderId); //Move order from pending to inProcess
        void moveOrderToPending(int orderId); //Move order from inProcess to pending
        void moveOrderToCompleted(int orderId); //Move order from inProcess to completed
        void increaseCustomerCounter(); //Increase customerCounter by 1
        void removeVolunteer(int volunteerId); //Remove volunteer from volunteers vector
        Order &getOrder(int orderId) const;
        const vector<BaseAction*> &getActions() const;
        void close();
        void open();

    private:
        bool isOpen;
        vector<BaseAction*> actionsLog;
        vector<Volunteer*> volunteers;
        vector<Order*> pendingOrders;
        vector<Order*> inProcessOrders;
        vector<Order*> completedOrders;
        vector<Customer*> customers;
        int customerCounter; //For assigning unique customer IDs
        int volunteerCounter; //For assigning unique volunteer IDs
        int orderCounter; //For assigning unique order IDs
};