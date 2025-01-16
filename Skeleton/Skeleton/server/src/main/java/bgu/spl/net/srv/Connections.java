package bgu.spl.net.srv;



public interface Connections<T> {

    boolean send(int connectionId, T msg);

    void send(String channel, T msg);

    void disconnect(int connectionId);

    boolean connectUser(String login, String passcode, int connectionID);

    ConnectionHandler<T> getConnection(int connectionID);

    boolean removeChannelSubscription(Integer subscriptionID, ConnectionHandler<T> connection);

    boolean addChannelSubscription(String channel, Integer subscriptionID, ConnectionHandler<T> connection);

    int getUserSubID(String channel, int connectionID);

    void addConnection(int connectionID, ConnectionHandler<T> connection);
    
    boolean userConnectedAlready(String login);

    void removeConnection(int connectionID);
}
