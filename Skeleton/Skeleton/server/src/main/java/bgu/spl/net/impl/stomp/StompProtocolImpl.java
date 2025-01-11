package bgu.spl.net.impl.stomp;

import java.io.IOException;
import java.util.HashMap;
import java.util.Map;

import bgu.spl.net.api.StompMessagingProtocol;
import bgu.spl.net.impl.stomp.StompFrame;
import bgu.spl.net.srv.ConnectionHandler;
import bgu.spl.net.srv.Connections;

public class StompProtocolImpl  implements StompMessagingProtocol<String> {
    private boolean shouldTerminate = false;

    private int connectionId;

    private Connections<String> connections;

    int messageID = 0; //Generator of message ID

    int connectionID; //The connection ID of the ConnectionHandler that's currently using this protocol

    

    @Override
    public void start (int connectionId, Connections<String> connections){
        this.connectionId = connectionId;
        this.connections = connections;
    }

    @Override
    public void process(String message) {
        System.out.println(message); //Printing the received frame
        StompFrame frame = new StompFrame(message);
        shouldTerminate = (frame.getCommand().equals("DISCONNECT"));
        String[] lines=message.split("\n");
        String command=lines[0].trim();
        Map<String,String> headers=frame.getHeaders();
            switch (frame.getCommand()) {
                case "CONNECT":
                double connect_version=Integer.parseInt(headers.get("accept-version"));
                String host=headers.get("host");
                String login=headers.get("login");
                String passcode=headers.get("passcode");
                boolean userAlreadyConnected = connections.userConnectedAlready(login);
                if (userAlreadyConnected) { //User is already connected to a different client
                Map<String, String> outputHeaders = new HashMap<>();
                outputHeaders.put("message", "User is already connected");
                StompFrame outputFrame = new StompFrame("ERROR", outputHeaders, "");
                String output = outputFrame.createFrame();
                connections.send(connectionID, output); //Sending back error frame
                try {
                    Thread.sleep(100);
                    }
                    catch(Exception ex) {}
                try {
                    shouldTerminate = true;
                    connections.disconnect(connectionID); //Resetting all client related data (subscriptions & from loggedIn clients list)
                    connections.removeConnection(connectionID);
                    connections.getConnection(connectionID).close();
                }
                catch (IOException exception) {}
            }
            else if (connections.connectUser(login, passcode, connectionID)) { //Trying to connect the user to the client
                Map<String, String> outputHeaders = new HashMap<>();
                outputHeaders.put("version", "1.2");
                StompFrame outputFrame = new StompFrame("CONNECTED", outputHeaders, "");
                String output = outputFrame.createFrame();
                connections.send(connectionID, output);
            }
            else { //User connection failed cause of wrong password
                Map<String, String> outputHeaders = new HashMap<>();
                outputHeaders.put("message", "Invalid password");
                StompFrame outputFrame = new StompFrame("ERROR", outputHeaders, "");
                String output = outputFrame.createFrame();
                connections.send(connectionID, output);
                try {
                    Thread.sleep(100);
                    }
                    catch(Exception ex) {}
                try {
                    shouldTerminate = true;
                    connections.disconnect(connectionID);
                    connections.removeConnection(connectionID);
                    connections.getConnection(connectionID).close();
                }
                catch (IOException exception) {}
            }

                break;
                case "SUBSCRIBE":
                String receiptID = headers.get("receipt");
                String subscriptionID = headers.get("id");
                String channel = headers.get("destination").substring(1); //without the '/' in the beginning of the channel
                boolean done = connections.addChannelSubscription(channel, Integer.parseInt(subscriptionID), connections.getConnection(connectionID));
            if (done) { //Managed to add the subscription
                Map<String, String> outputHeaders = new HashMap<>();
                outputHeaders.put("receipt-id", receiptID);
                StompFrame outputFrame = new StompFrame("RECEIPT", outputHeaders, "");
                String output = outputFrame.createFrame();
                connections.send(connectionID, output);
            }
            else { //Didn't manage to add the subscription because the user already subscribed to it.
                Map<String, String> outputHeaders = new HashMap<>();
                outputHeaders.put("receipt-id", receiptID);
                outputHeaders.put("message", "The user already subscribed to channel: " + channel);
                StompFrame outputFrame = new StompFrame("ERROR", outputHeaders, "");
                String output = outputFrame.createFrame();
                connections.send(connectionID, output);
                try {
                Thread.sleep(100);
                }
                catch(Exception ex) {}
                try {
                    shouldTerminate = true;
                    connections.disconnect(connectionID);
                    connections.removeConnection(connectionID);
                    connections.getConnection(connectionID).close();
                }
                catch (IOException exception) {}
            }
            
                break;
                case "UNSUBSCRIBE":
                String receiptIDun = headers.get("receipt");
                String subscriptionIDun = headers.get("id");
                boolean doneUn = connections.removeChannelSubscription(Integer.parseInt(subscriptionIDun), connections.getConnection(connectionID)); 
                if (doneUn) { //Managed to remove the subscription
                    Map<String, String> outputHeaders = new HashMap<>();
                    outputHeaders.put("receipt-id", receiptIDun);
                    StompFrame outputFrame = new StompFrame("RECEIPT", outputHeaders, "");
                    String output = outputFrame.createFrame();
                    connections.send(connectionID, output);
                }
                else { //Didn't manage to remove the subscription because the user wasn't subscribed to it.
                    Map<String, String> outputHeaders = new HashMap<>();
                    outputHeaders.put("receipt-id", receiptIDun);
                    outputHeaders.put("message", "The user isn't subscribed with that number: " + subscriptionIDun);
                    StompFrame outputFrame = new StompFrame("ERROR", outputHeaders, "");
                    String output = outputFrame.createFrame();
                    connections.send(connectionID, output);
                    try {
                        Thread.sleep(100);
                        }
                        catch(Exception ex) {}
                    try {
                        shouldTerminate = true;
                        connections.disconnect(connectionID);
                        connections.removeConnection(connectionID);
                        connections.getConnection(connectionID).close();
                    }
                    catch (IOException exception) {}
                }
                break;
                case "SEND":
                String channelS = headers.get("destination").substring(1); //without the '/' in the beginning of the channel
                int subscriptionIDS = connections.getUserSubID(channelS, connectionID);
            if (subscriptionIDS == -1) { //If the user isn't subscribed to the sent report's channel
                Map<String, String> outputHeaders = new HashMap<>();
                outputHeaders.put("message", "The user isn't subscribed to channel: " + channelS);
                StompFrame outputFrame = new StompFrame("ERROR", outputHeaders, "");
                String output = outputFrame.createFrame();
                connections.send(connectionID, output);
                try {
                    Thread.sleep(100);
                    }
                    catch(Exception ex) {}
                try {
                    shouldTerminate = true;
                    connections.disconnect(connectionID);
                    connections.removeConnection(connectionID);
                    connections.getConnection(connectionID).close();
                }
                catch (IOException exception) {}
            }
            else { //The user is subscribed to the reported channel
                String bodyMessage = frame.getBody();
                Map<String, String> outputHeaders = new HashMap<>();
                outputHeaders.put("subscription", "" + subscriptionIDS);
                outputHeaders.put("message-id", "" + messageID);
                outputHeaders.put("destination", "/" + channelS);
                StompFrame outputFrame = new StompFrame("MESSAGE", outputHeaders, bodyMessage);
                String output = outputFrame.createFrame();
                messageID++;
                connections.send(channelS, output); //Sending the report to all the users subscribed to the channel
            }
                break;
                
                case "DISCONNECT":
                String receiptIDis = headers.get("receipt");
                Map<String, String> outputHeaders = new HashMap<>();
                outputHeaders.put("receipt-id", receiptIDis);
                StompFrame outputFrame = new StompFrame("RECEIPT", outputHeaders, "");
                String output = outputFrame.createFrame();
                connections.send(connectionID, output); //Sending back receipt for disconnecting
                connections.disconnect(connectionID); //Removing the user from loggedIn & the user's subscription
                break;
                
               
            }
        
    }


    @Override
    public boolean shouldTerminate() {
       return shouldTerminate;
    }

/* 
     public Map<String, String> parseStompMessage(String[] lines) {
         // Initialize a map to store headers
        Map<String, String> headers = new HashMap<>();

        // Loop through lines (skip the first line as it's the command)
        int i = 1;
        while (i < lines.length && !lines[i].isEmpty()) {
            String line = lines[i].trim();
            String[] parts = line.split(":");
            if (parts.length == 2) {
                String headerName = parts[0].trim();
                String headerValue = parts[1].trim();
                headers.put(headerName, headerValue);
            }
            i++;
        }
        /*
        // Print the headers
        System.out.println("Headers:");
        for (Map.Entry<String, String> entry : headers.entrySet()) {
            System.out.println(entry.getKey() + ": " + entry.getValue());
        }

        // The body is after the headers, so we fetch the remaining part of the message
        if (i < lines.length) {
            String body = lines[i].trim();
            System.out.println("Body: " + body);
        }
            */
        //return headers;
   

}
