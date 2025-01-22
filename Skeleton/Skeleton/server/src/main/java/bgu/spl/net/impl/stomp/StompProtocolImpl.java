package bgu.spl.net.impl.stomp;

import bgu.spl.net.api.StompMessagingProtocol;
import bgu.spl.net.srv.*;

import java.io.IOException;
import java.util.HashMap;
import java.util.Map;

public class StompProtocolImpl implements StompMessagingProtocol<String> {

    private boolean shouldTerminate = false;

    private Connections<String> activeConnections;

    int messageID = 0; //Generator of message ID

    int connectionID; //The connection ID of the ConnectionHandler that's currently using this protocol

    public void start(int connectionId, Connections<String> connections) { //Updating the connection ID & Connections object
        activeConnections = connections;
        this.connectionID = connectionId;
    }

    public void process(String message) { //Receiving a frame & processing it
        StompFrame frame = new StompFrame(message); //Creating a StompFrame object using the constructor we built
        shouldTerminate = (frame.getCommand().equals("DISCONNECT"));
        if(frame.getCommand().equals("CONNECT")) { //Client trying to connect to a user
            Map<String, String> frameHeaders = frame.getHeaders();
            String login = frameHeaders.get("login");
            String passcode = frameHeaders.get("passcode");
            boolean userAlreadyConnected = activeConnections.userConnectedAlready(login); //Checking if the user is already connected to different client
            if (userAlreadyConnected) { //User is already connected to a different client
                Map<String, String> outputHeaders = new HashMap<>();
                outputHeaders.put("message", "User is already connected");
                StompFrame outputFrame = new StompFrame("ERROR", outputHeaders, "");
                String output = outputFrame.createFrame();
                activeConnections.send(connectionID, output); //Sending back error frame
                try {
                    Thread.sleep(100);
                    }
                    catch(Exception ex) {}
                try {
                    shouldTerminate = true;
                    activeConnections.disconnect(connectionID); //Resetting all client related data (subscriptions & from loggedIn clients list)
                    activeConnections.getConnection(connectionID).close();
                    activeConnections.removeConnection(connectionID);
                }
                catch (IOException exception) {}
            }
            else if (activeConnections.connectUser(login, passcode, connectionID)) { //Trying to connect the user to the client
                Map<String, String> outputHeaders = new HashMap<>();
                outputHeaders.put("version", "1.2");
                StompFrame outputFrame = new StompFrame("CONNECTED", outputHeaders, "");
                String output = outputFrame.createFrame();
                activeConnections.send(connectionID, output);
            }
            else { //User connection failed cause of wrong password
                Map<String, String> outputHeaders = new HashMap<>();
                outputHeaders.put("message", "Wrong password");
                StompFrame outputFrame = new StompFrame("ERROR", outputHeaders, "");
                String output = outputFrame.createFrame();
                activeConnections.send(connectionID, output);
                try {
                    Thread.sleep(100);
                    }
                    catch(Exception ex) {}
                try {
                    shouldTerminate = true;
                    activeConnections.disconnect(connectionID);
                    activeConnections.getConnection(connectionID).close();
                    activeConnections.removeConnection(connectionID);
                }
                catch (IOException exception) {}
            }
        }
        else if (frame.getCommand().equals("DISCONNECT")) {
            Map<String, String> frameHeaders = frame.getHeaders();
            String receiptID = frameHeaders.get("receipt");
            Map<String, String> outputHeaders = new HashMap<>();
            outputHeaders.put("receipt-id", receiptID);
            StompFrame outputFrame = new StompFrame("RECEIPT", outputHeaders, "");
            String output = outputFrame.createFrame();
            activeConnections.send(connectionID, output); //Sending back receipt for disconnecting
            activeConnections.disconnect(connectionID); //Removing the user from loggedIn & the user's subscription
        }
        else if (frame.getCommand().equals("UNSUBSCRIBE")) {
            Map<String, String> frameHeaders = frame.getHeaders();
            String receiptID = frameHeaders.get("receipt");
            String subscriptionID = frameHeaders.get("id");
            boolean done = activeConnections.removeChannelSubscription(Integer.parseInt(subscriptionID), activeConnections.getConnection(connectionID)); //Trying to remove the user from the requested subscription
            if (done) { //Managed to remove the subscription
                System.out.println(activeConnections.toStringChannels());
                Map<String, String> outputHeaders = new HashMap<>();
                outputHeaders.put("receipt-id", receiptID);
                StompFrame outputFrame = new StompFrame("RECEIPT", outputHeaders, "");
                String output = outputFrame.createFrame();
                activeConnections.send(connectionID, output);
            }
            else { //Didn't manage to remove the subscription because the user wasn't subscribed to it.
                Map<String, String> outputHeaders = new HashMap<>();
                outputHeaders.put("receipt-id", receiptID);
                outputHeaders.put("message", "The user isn't subscribed with that number: " + subscriptionID);
                StompFrame outputFrame = new StompFrame("ERROR", outputHeaders, "");
                String output = outputFrame.createFrame();
                activeConnections.send(connectionID, output);
                try {
                    Thread.sleep(100);
                    }
                    catch(Exception ex) {}
                try {
                    shouldTerminate = true;
                    activeConnections.disconnect(connectionID);
                    activeConnections.getConnection(connectionID).close();
                    activeConnections.removeConnection(connectionID);
                }
                catch (IOException exception) {}
            }
            
        }
        else if (frame.getCommand().equals("SUBSCRIBE")) { //Trying to add the user to the requested subscription
            System.out.println(activeConnections.toStringChannels());
            Map<String, String> frameHeaders = frame.getHeaders();
            String receiptID = frameHeaders.get("receipt");
            String subscriptionID = frameHeaders.get("id");
            String channel = frameHeaders.get("destination");
            boolean done = activeConnections.addChannelSubscription(channel, Integer.parseInt(subscriptionID), activeConnections.getConnection(connectionID));
            if (done) { //Managed to add the subscription
                Map<String, String> outputHeaders = new HashMap<>();
                outputHeaders.put("receipt-id", receiptID);
                StompFrame outputFrame = new StompFrame("RECEIPT", outputHeaders, "");
                String output = outputFrame.createFrame();
                activeConnections.send(connectionID, output);
            }
            else { //Didn't manage to add the subscription because the user already subscribed to it.
                Map<String, String> outputHeaders = new HashMap<>();
                outputHeaders.put("receipt-id", receiptID);
                outputHeaders.put("message", "The user already subscribed to channel: " + channel);
                StompFrame outputFrame = new StompFrame("ERROR", outputHeaders, "");
                String output = outputFrame.createFrame();
                activeConnections.send(connectionID, output);
                try {
                Thread.sleep(100);
                }
                catch(Exception ex) {}
                try {
                    shouldTerminate = true;
                    activeConnections.disconnect(connectionID);
                    activeConnections.getConnection(connectionID).close();
                    activeConnections.removeConnection(connectionID);
                }
                catch (IOException exception) {}
            }
            
        }
        else if (frame.getCommand().equals("SEND")) { //Got a report from a user to a specific channel
            Map<String, String> frameHeaders = frame.getHeaders();
            String channel = frameHeaders.get("destination"); 
            int subscriptionID = activeConnections.getUserSubID(channel, connectionID);
            if (subscriptionID == -1) { //If the user isn't subscribed to the sent report's channel
                Map<String, String> outputHeaders = new HashMap<>();
                outputHeaders.put("message", "The user isn't subscribed to channel: " + channel);
                StompFrame outputFrame = new StompFrame("ERROR", outputHeaders, "");
                String output = outputFrame.createFrame();
                activeConnections.send(connectionID, output);
                try {
                    Thread.sleep(100);
                    }
                    catch(Exception ex) {}
                try {
                    shouldTerminate = true;
                    activeConnections.disconnect(connectionID);
                    activeConnections.getConnection(connectionID).close();
                    activeConnections.removeConnection(connectionID);
                }
                catch (IOException exception) {}
            }
            else { //The user is subscribed to the reported channel
                
                String bodyMessage = frame.getBody();
                System.out.println("body-"+bodyMessage);
                Map<String, String> outputHeaders = new HashMap<>();
                outputHeaders.put("subscription", "" + subscriptionID);
                outputHeaders.put("message-id", "" + messageID);
                outputHeaders.put("destination",channel);
                StompFrame outputFrame = new StompFrame("MESSAGE", outputHeaders, bodyMessage);
                String output = outputFrame.createFrame();
                messageID++;
                System.out.println(output);
                activeConnections.send(channel, output); //Sending the report to all the users subscribed to the channel
            }

        }
    }

    /**
     * @return true if the connection should be terminated
     */
    public boolean shouldTerminate() {
        return shouldTerminate;
    }
}
