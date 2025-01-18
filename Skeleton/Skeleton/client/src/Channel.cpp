#include "../include/Channel.h"

Channel::Channel(string channel_name):channel_name(channel_name), events(){}

Channel::~Channel(){}

string Channel::get_name(){return channel_name;}

vector<Event> Channel::getEvents_ByUser(string user)
{
	if (events.find(user) == events.end()) {
		return vector<Event>();
	}
	else {
		return events.find(user)->second;
	}
}

void Channel::addEvent(string user, Event event){
    if (events.find(user) == events.end()) {
		vector<Event> eventsList;
		eventsList.push_back(event);
		events.insert({ user, eventsList });
	}
	else {
		events.find(user)->second.push_back(event);
	}
}
int Channel::getNumberDepartment(){
	return events.size();
}

