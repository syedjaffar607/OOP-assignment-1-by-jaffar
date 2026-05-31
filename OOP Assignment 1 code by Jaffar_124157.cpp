#include<iostream>
#include<string>
#include<vector>
#include<map>
#include<fstream>
#include<sstream>
#include<iomanip>
#include<algorithm>
#include<memory>
#include<stdexcept>
#include<ctime>
#include<cmath>
using namespace std;

// Exception classes for handling system errors
class FlightFullException : public exception {
public:
    const char* what() const throw() {
        return "Flight is full!";
    }
};

class InvalidCancellationException : public exception {
public:
    const char* what() const throw() {
        return "No seats booked to cancel.";
    }
};

// Abstract Base Class for Flight
class Flight {
protected:
    string flightNumber;
    string origin;
    string destination;
    string departureDateTime;
    int totalSeats;
    int bookedSeats;
   
public:
    Flight(string fn, string o, string d, string dt, int seats)
        : flightNumber(fn), origin(o), destination(d), departureDateTime(dt), totalSeats(seats), bookedSeats(0) {}
   
    virtual ~Flight() {}

    // Pure virtual functions for polymorphism
    virtual double calculateBaseFare() const = 0;
    virtual void displayDetails() const = 0;
   
    // Getters
    string getFlightNumber() const { return flightNumber; }
    int getTotalSeats() const { return totalSeats; }
    int getbookedSeats() const { return bookedSeats; }
    bool isFull() const { return bookedSeats >= totalSeats; }
   
    // Booking and Cancellation methods
    void bookSeats() {
        if (isFull()) throw FlightFullException();
        bookedSeats++;
    }
    void cancelSeat() {
        if (bookedSeats <= 0) throw InvalidCancellationException();
        bookedSeats--;
    }
    
    // Operator overloading
    bool operator==(const Flight& other) const { 
        return flightNumber == other.flightNumber;
    }
};

// Derived Class 1: Domestic Flights
class DomesticFlight : public Flight {
public:
    DomesticFlight(string fn, string o, string d, string dt, int seats)
        : Flight(fn, o, d, dt, seats) {} 
        
    double calculateBaseFare() const override {
        return 1000.0;
    }
    void displayDetails() const override {
        cout << "[Domestic] " << flightNumber << " | " << origin << " -> " << destination
             << " | Seats: " << bookedSeats << "/" << totalSeats << endl;
    }
};

// Derived Class 2: International Flights
class InternationalFlight : public Flight {
private:
    double visaFee;
public:
    InternationalFlight(string fn, string o, string d, string dt, int seats, double vFee)
        : Flight(fn, o, d, dt, seats), visaFee(vFee) {}
        
    double calculateBaseFare() const override {
        return 2000.0 + visaFee;
    }
    void displayDetails() const override {
        cout << "[International] " << flightNumber << " | " << origin << " -> " << destination
             << " | Seats: " << bookedSeats << "/" << totalSeats << " | Visa: $" << visaFee << endl;
    }
};

// Derived Class 3: Charter Flights
class CharterFlight : public Flight {
private:
    string contractHolder;
public:
    CharterFlight(string fn, string o, string d, string dt, int seats, string holder)
        : Flight(fn, o, d, dt, seats), contractHolder(holder) {}
        
    double calculateBaseFare() const override {
        return 5000.0;
    }
    void displayDetails() const override {
        cout << "[Charter] " << flightNumber << " | " << origin << " -> " << destination
             << " | Seats: " << bookedSeats << "/" << totalSeats << " | Holder: " << contractHolder << endl;
    }
}; 

// Base Class for Passenger
class Passenger {
protected:
    string name;
    int loyaltyPoints;
    string PassengerId;
public:
    Passenger(string n, string id) : name(n), PassengerId(id), loyaltyPoints(0) {}
    virtual ~Passenger() {}
   
    string getId() const { return PassengerId; }
    string getName() const { return name; }
   
    virtual double calculateRefundPercentage() const = 0;
    virtual void displayinfo() const {
        cout << "Passenger: " << name << " (" << PassengerId << ") - Points: " << loyaltyPoints << endl;
    }
    bool operator==(const Passenger& other) const {
        return PassengerId == other.PassengerId;
    }
};

// Passenger Types
class EconomyPassenger : public Passenger {
public:
    EconomyPassenger(string n, string Id) : Passenger(n, Id) {
        loyaltyPoints = 50;
    }
    double calculateRefundPercentage() const override { return 0.60; }
}; 

class BusinessPassenger : public Passenger {
public:
    BusinessPassenger(string n, string Id) : Passenger(n, Id) {
        loyaltyPoints = 200;
    }
    double calculateRefundPercentage() const override { return 0.90; }
}; 

class FirstClassPassenger : public Passenger {
public:
    FirstClassPassenger(string n, string id) : Passenger(n, id) {
        loyaltyPoints = 500;
    }
    double calculateRefundPercentage() const override { return 1.00; }
};
   
// Ticket Class
class Ticket {
private:
    Flight* flight;
    Passenger* passenger;
    int seatNumber;
    double fare;
    bool isActive;
   
public:
    Ticket(Flight* f, Passenger* p, int seat, double price)
        : flight(f), passenger(p), seatNumber(seat), fare(price), isActive(true) {}
        
    double getFare() const { return fare; }
    bool getStatus() const { return isActive; }
    string getPassengerId() const { return passenger->getId(); }
    string getFlightNumber() const { return flight->getFlightNumber(); }
    void cancel() { isActive = false; }
   
    friend ostream& operator<<(ostream& os, const Ticket& t) {
        os << "Ticket [Passenger: " << t.passenger->getName() << " | Flight: "
           << t.flight->getFlightNumber() << " | Seat: " << t.seatNumber
           << " | Fare: $" << t.fare << " | Status: " << (t.isActive ? "Active" : "Cancelled") << "]";
        return os;
    }
};
   
// Main Airline Management System Class
class AirlineSystem {
private:
    vector<unique_ptr<Flight>> flights;
    vector<unique_ptr<Passenger>> passengers;
    vector<unique_ptr<Ticket>> tickets;
   
    int findFlightIndex(string flightNum) {
        for (size_t i = 0; i < flights.size(); ++i) {
            if (flights[i]->getFlightNumber() == flightNum) 
                return i;
        }
        return -1;
    }
    int findPassengerIndex(string id) {
        for (size_t i = 0; i < passengers.size(); ++i) {
            if (passengers[i]->getId() == id)
                return i;
        }
        return -1;
    }
public:
    AirlineSystem() {
        loadFromFile();
    }
    
    void addFlight() {
        string fn, o, d, dt, contract;
        int seats;
        double fee;
       
        cout << "Enter the Flight type (1.Domestic flight, 2.International flight, 3.Charter flight): ";
        int choice; 
        cin >> choice;
        cout << "Enter the Flight Number: ";
        cin >> fn;
        cout << "Origin: ";
        cin >> o;
        cout << "Destination: ";
        cin >> d;
        cout << "Date/Time(YYYY-MM-DD HH): ";
        cin >> ws;
        getline(cin, dt);
        cout << "Total Seats: ";
        cin >> seats;
       
       if (choice == 1) {
    flights.push_back(unique_ptr<DomesticFlight>(
        new DomesticFlight(fn, o, d, dt, seats)));
}
else if (choice == 2) {
    cout << "Visa/Entry Fee: ";
    cin >> fee;

    flights.push_back(unique_ptr<InternationalFlight>(
        new InternationalFlight(fn, o, d, dt, seats, fee)));
}
else if (choice == 3) {
    cout << "Contract Holder Name: ";
    cin >> ws;
    getline(cin, contract);

    flights.push_back(unique_ptr<CharterFlight>(
        new CharterFlight(fn, o, d, dt, seats, contract)));
}
else {
    cout << "Invalid type." << endl;
}
    }
    
    void removeFlight() {
        string fn;
        cout << "Enter Flight Number to remove: ";
        cin >> fn;
        int idx = findFlightIndex(fn);
        if (idx != -1) {
            flights.erase(flights.begin() + idx);
            cout << "Flight removed." << endl;   
        } else { 
            cout << "Flight not found." << endl;
        }
    }
    
    void listFlights() const {
        cout << "\n--- List of Flights ---" << endl;
        for (const auto& f : flights) {
            f->displayDetails();
        }
    }
    
    void registerPassenger() {
        string name, id;
        cout << "Name: ";
        cin >> name;
        cout << "ID: ";
        cin >> id;
        if (findPassengerIndex(id) != -1) {
            cout << "Passenger ID already exists!" << endl;
            return;
        }
        cout << "Passenger type (1.Economy, 2.Business, 3.First Class): ";
        int choice; cin >> choice;
        if (choice == 1) passengers.push_back(unique_ptr<EconomyPassenger>(new EconomyPassenger(name, id)));
        else if (choice == 2) passengers.push_back(unique_ptr<BusinessPassenger>(new BusinessPassenger(name, id)));
        else if (choice == 3) passengers.push_back(unique_ptr<FirstClassPassenger>(new FirstClassPassenger(name, id)));
        else cout << "Invalid Passenger Type." << endl;
    }
    
    void listPassengers() const {
        cout << "\n--- Registered Passengers ---" << endl;
        for (const auto& p : passengers) {
            p->displayinfo();
        }
    }
    
    void bookTicket() {
        string fid, pid;
        cout << "Enter Flight Number: ";
        cin >> fid;
        cout << "Enter Passenger ID: ";
        cin >> pid;
       
        int fIdx = findFlightIndex(fid);
        if (fIdx == -1) { cout << "Flight not found." << endl; return; }
        int pIdx = findPassengerIndex(pid);
        if (pIdx == -1) { cout << "Passenger not found." << endl; return; }
        
        Flight* f = flights[fIdx].get();
        Passenger* p = passengers[pIdx].get();
        try {
            f->bookSeats();
            double fare = f->calculateBaseFare();
            int seatNum = f->getbookedSeats();
            tickets.push_back(unique_ptr<Ticket>(new Ticket(f, p, seatNum, fare)));
            cout << "Ticket booked successfully!" << endl;
        } catch (const exception& e) {
            cout << e.what() << endl;    
        }
    }
    
    void cancelTicket() {
        string pid, fid;
        cout << "Enter Passenger ID: ";
        cin >> pid;
        cout << "Enter Flight Number: ";
        cin >> fid;
        
        auto it = find_if(tickets.begin(), tickets.end(), [&](const unique_ptr<Ticket>& t) { 
            return t->getPassengerId() == pid && t->getFlightNumber() == fid && t->getStatus() == true; 
        });

        if (it != tickets.end()) {
            int pIdx = findPassengerIndex(pid);
            if (pIdx != -1) {
                double refundPct = passengers[pIdx]->calculateRefundPercentage();
                double refundAmt = (*it)->getFare() * refundPct; 
                int fidx = findFlightIndex(fid);
                if (fidx != -1) flights[fidx]->cancelSeat();
                (*it)->cancel();
                cout << "Ticket cancelled. Refund amount: $" << refundAmt << endl;
            }
        } else {
            cout << "Active ticket not found for this passenger on this flight." << endl;
        }
    }
    
    void viewReports() const {
        cout << "\n---- Reports -----" << endl;
        cout << "Top Active Tickets:" << endl;
        for (const auto& t : tickets) {
            cout << *t << endl;
        }
    }
    
    void loadFromFile() {
        flights.push_back(unique_ptr<DomesticFlight>(new DomesticFlight("D101", "NYC", "LAX", "2023-12-01 10:00", 100)));
        flights.push_back(unique_ptr<InternationalFlight>(new InternationalFlight("I202", "JFK", "LHR", "2023-12-02 18:00", 200, 50.0)));
        passengers.push_back(unique_ptr<EconomyPassenger>(new EconomyPassenger("John_Doe", "p001")));
        passengers.push_back(unique_ptr<BusinessPassenger>(new BusinessPassenger("Jane_Smith", "p002")));
        cout <<      "------welcome to JF flight------." << endl;
         cout << "Loaded default Data." << endl;
    }
};

void displayMenu() {
    cout << "\n1. Add Flight route\n2. Remove Flight route\n3. List All Active Flights\n4. Register New Passenger"
         << "\n5. List All  Register Passengers\n6. Flight Booking Ticket\n7. flight Cancel Ticket\n8. View All Reports\n9. Safe Exit\nEnter choice: ";
}

int main() {
    AirlineSystem sys;
    int choice = 0;
   
    while (choice != 9) {
        displayMenu();
        cin >> choice;
       
        switch (choice) {
            case 1: sys.addFlight(); break;
            case 2: sys.removeFlight(); break;
            case 3: sys.listFlights(); break;
            case 4: sys.registerPassenger(); break;
            case 5: sys.listPassengers(); break;
            case 6: sys.bookTicket(); break;
            case 7: sys.cancelTicket(); break;
            case 8: sys.viewReports(); break;
            case 9: cout << "Saving and exiting..." << endl; break;
            default: cout << "Invalid choice" << endl;
        }
    }
    return 0;
}
