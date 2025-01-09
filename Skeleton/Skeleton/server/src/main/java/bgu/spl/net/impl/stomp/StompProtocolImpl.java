package bgu.spl.net.impl.stomp;

import bgu.spl.net.api.StompMessagingProtocol;
import bgu.spl.net.srv.Connections;

public class StompProtocolImpl  implements StompMessagingProtocol<String> {
    private boolean shouldTerminate;
    private int connectionId;
    private Connections<String> connections;

    public StompProtocolImpl(){
        this.connectionId = -1;
        this.connections = null;
        this.shouldTerminate = false;
    }
    


    @Override
    public void start (int connectionId, Connections<String> connections){
        this.connectionId = connectionId;
        this.connections = connections;
    }

    @Override
    public void process(String message) {
        StompFrame frame = StompFrame.parse(message);
        switch (frame.getCommand()) {
            case "CONNECT":
                handleConnect(frame);
                break;
            case "SUBSCRIBE":
                handleSubscribe(frame);
                break;
            case "UNSUBSCRIBE":
                handleUnsubscribe(frame);
                break;
            case "SEND":
                handleSend(frame);
                break;
            case "DISCONNECT":
                handleDisconnect(frame);
                break;
            default:
                sendError("Unknown command: " + frame.getCommand());
                break;
        }

    }

    @Override
    public boolean shouldTerminate() {
       return shouldTerminate;
    }

    private void handleConnect(StompFrame frame) {
        connections.send(connectionId, "CONNECTED\nversion:1.2\n\n^@");
    }

    private void handleSubscribe(StompFrame frame) {
        String destination = frame.getHeader("destination");
        if (destination != null) {
            connections.subscribeClientToChannel(destination, connectionId);
            connections.send(connectionId, "RECEIPT\nreceipt-id:" + frame.getHeader("id") + "\n\n^@");
        } else {
            sendError("SUBSCRIBE frame missing destination header.");
        }
    }

    private void handleUnsubscribe(StompFrame frame) {
        String subscriptionId = frame.getHeader("id");
    
        if (subscriptionId != null) {
            connections.unsubscribeClientFromChannel(subscriptionId, connectionId);
    
            
            String receiptId = frame.getHeader("receipt");
            if (receiptId != null) {
                connections.send(connectionId, "RECEIPT\nreceipt-id:" + receiptId + "\n\n^@");
            }
        } else {
            sendError("Missing 'id' header in UNSUBSCRIBE frame.");
        }
    }
    

    private void handleSend(StompFrame frame) {
        String destination = frame.getHeader("destination");
        if (destination != null) {
            connections.send(destination, frame.getBody());
        } else {
            sendError("SEND frame missing destination header.");
        }
    }

    private void handleDisconnect(StompFrame frame) {
        connections.disconnect(connectionId);
        shouldTerminate = true;
        connections.send(connectionId, "RECEIPT\nreceipt-id:" + frame.getHeader("receipt") + "\n\n^@");
    }

    private void sendError(String message) {
        connections.send(connectionId, "ERROR\nmessage:" + message + "\n\n^@");
    }




}
