#include "../include/Channel.h"

Channel::Channel(string channel_name):channel_name(channel_name), events(){}

Channel::~Channel(){}

string Channel::get_name(){return channel_name;}

std::vector<Event> Channel::getEvents_ByUser(const std::string& user) {
        std::cout << "Enter Channel: " << channel_name << "\n";
        std::cout << "user: " << user << "\n";
        std::cout << "Events size: " << events.size() << "\n";
        for (const Event& e : events) {
            std::cout << "Event by: " << e.getEventOwnerUser() << "\tdescription: " << e.get_description() << "\n";
        }

        std::vector<Event> reports;
        for (const Event& e : events) {
            if (e.getEventOwnerUser() == user) { 
                reports.push_back(e);
            }
        }
        return reports;
    }

void Channel::addEvent(Event event){
    events.push_back(event);
}
int Channel::getNumberDepartment(){
	return events.size();
}

