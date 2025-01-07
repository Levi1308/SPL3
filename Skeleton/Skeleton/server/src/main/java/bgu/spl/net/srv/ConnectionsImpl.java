package bgu.spl.net.srv;

import java.util.Map;
import java.util.Set;
import java.util.concurrent.ConcurrentHashMap;

public class ConnectionsImpl<T> implements Connections<T> {
    private ConcurrentHashMap<Integer, ConnectionHandler<T>> clients = new ConcurrentHashMap<>();
    private ConcurrentHashMap<String, Set<Integer>> channelSubscriptions = new ConcurrentHashMap<>();

    
    @Override
    public boolean send(int connectionId, T msg) {
        ConnectionHandler<T> handler = clients.getOrDefault(connectionId, null);
        if (handler != null) {
            handler.send(msg);
            return true;
        }
        return false;
    }

    @Override
    public void send(String channel, T msg) {
        Set<Integer> subscribers = channelSubscriptions.getOrDefault(channel,null);
        if (subscribers != null) {
            for (Integer connectionId : subscribers) {
                ConnectionHandler<T> handler = clients.getOrDefault(connectionId,null);
                if (handler != null) {
                    handler.send(msg);
                }
            }
        }
    }

    @Override
    public void disconnect(int connectionId) {
        clients.remove(connectionId);
    }

    public void subscribeClientToChannel(String channel, int connectionId) {
        channelSubscriptions.putIfAbsent(channel, ConcurrentHashMap.newKeySet());
        channelSubscriptions.get(channel).add(connectionId);
    }

    public void unsubscribeClientFromChannel(String channel, int connectionId) {
        Set<Integer> subscribers = channelSubscriptions.getOrDefault(channel,null);
        if (subscribers != null) {
            subscribers.remove(connectionId);
        }
    }
    

    
    
}

