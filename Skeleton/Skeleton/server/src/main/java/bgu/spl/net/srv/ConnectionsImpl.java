package bgu.spl.net.srv;

import java.util.HashMap;
import java.util.Map;
import java.util.Set;
import java.util.concurrent.ConcurrentHashMap;

public class ConnectionsImpl<T> implements Connections<T> {
    private Map<Integer, ConnectionHandler<T>> activeClients;
    private Map<String, Map<ConnectionHandler<T>, Integer>> channelsSubscriptions;
    private Map<String, String> users;
    private Map<String, Integer> loggedInUsers;
    public static int connectionID = 0;

    ConnectionsImpl() {
        activeClients = new HashMap<>();
        channelsSubscriptions = new HashMap<>();
        users = new HashMap<>();
        loggedInUsers = new HashMap<>();
    }


    
    @Override
    public boolean send(int connectionId, T msg) {
        ConnectionHandler<T> handler = activeClients.getOrDefault(connectionId, null);
        if (handler != null) {
            handler.send(msg);
            return true;
        }
        return false;
    }

    @Override
    public void send(String channel, T msg) {
        Map<ConnectionHandler<T>, Integer> subscribers = channelsSubscriptions.get(channel);
        if (subscribers != null) {
            for(ConnectionHandler<T> connection: subscribers.keySet()) {
                connection.send(msg);
            }
        }
    }

    @Override
    public void disconnect(int connectionId) {
        loggedInUsers.values().remove(connectionId);
        for (String channel : channelsSubscriptions.keySet()) { //Going over the channels & removing the user from every channel it subscribed to
            Map<ConnectionHandler<T>, Integer> subs = channelsSubscriptions.get(channel);
            for (ConnectionHandler<T> conn : subs.keySet()) {
                if(conn == activeClients.get(connectionID)) {
                    channelsSubscriptions.get(channel).remove(conn);
                }
                
            }
        }
    }

    public void addConnection(int connectionID, ConnectionHandler<T> connection) { //adding a connection to a list of active connections
        activeClients.put(connectionID, connection);
    }

    public boolean addChannelSubscription(String channel, Integer subscriptionID, ConnectionHandler<T> connection) {
        if (channelsSubscriptions.containsKey(channel)) {
            if(channelsSubscriptions.get(channel).containsKey(connection)) {
                return false;
            }
            channelsSubscriptions.get(channel).put(connection, subscriptionID);
            return true;
        }
        else {
            Map<ConnectionHandler<T>, Integer> newChannel = new HashMap<>();
            newChannel.put(connection, subscriptionID);
            channelsSubscriptions.put(channel, newChannel);
            return true;
        }
    }


    public boolean removeChannelSubscription(Integer subscriptionID, ConnectionHandler<T> connection) {
        for(String channel: channelsSubscriptions.keySet()) {
            if (channelsSubscriptions.get(channel).containsKey(connection)) {
                if (channelsSubscriptions.get(channel).get(connection) == subscriptionID) {
                    channelsSubscriptions.get(channel).remove(connection);
                    return true;
                }
            }
        }
        return false;
    }

    public Map<ConnectionHandler<T>, Integer> getSubscribedConnections(String channel) {
        return channelsSubscriptions.get(channel);
    }

    private boolean hasRegistered(String login, String passcode) {
        return users.containsKey(login);
    }

    private void addUser(String login, String passcode) {
        users.put(login, passcode);
    }

    public boolean userConnectedAlready(String login) {
        return loggedInUsers.containsKey(login);
    }
    public boolean connectUser(String login, String passcode, int connectionID) {
        if (hasRegistered(login, passcode)) {
            if (!users.get(login).equals(passcode)) {
                return false;
            }
            else {
                loggedInUsers.put(login, connectionID);
                return true;
            }
        }
        else {
            addUser(login, passcode);
            loggedInUsers.put(login, connectionID);
            return true;
        }
    }

    public int getUserSubID(String channel, int connectionID) {
        if (!channelsSubscriptions.containsKey(channel)) { //There isn't a channel with that name
            return -1;
        }
        else if (!channelsSubscriptions.get(channel).containsKey(activeClients.get(connectionID))) { //User isn't subscribed to this channel
            return -1;
        }
        else {
            return channelsSubscriptions.get(channel).get(activeClients.get(connectionID));
        }
    }

    public ConnectionHandler<T> getConnection(int connectionID) {
        return activeClients.get(connectionID);
    }

    public void removeConnection(int connectionID) {
        activeClients.remove(connectionID);
    }
    

    
    
}

