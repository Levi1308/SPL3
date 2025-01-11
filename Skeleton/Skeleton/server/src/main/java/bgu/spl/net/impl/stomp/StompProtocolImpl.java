package bgu.spl.net.impl.stomp;

import java.util.HashMap;
import java.util.Map;

import bgu.spl.net.api.StompMessagingProtocol;
import bgu.spl.net.impl.stomp.StompFrame.ConnectFrame;
import bgu.spl.net.impl.stomp.StompFrame.*;
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
        String[] lines=message.split("\n");
        String command=lines[0].trim();
        Map<String,String> headers=parseStompMessage(lines);
            switch (command) {
                case "ERROR":
                //ErrorFrame errorFrame=new ErrorFrame(frame[1],frame[2],frame[3]);
                String error_message=headers.get("message"); // Description of the error
                int receiptId=Integer.parseInt(headers.get("reciept-id")); // Optional receipt ID for the related frame 
                String error_body=headers.get("body");//should be body description headline

                    break;
                case "CONNECT":
                int connect_version=Integer.parseInt(headers.get("accept-version"));
                String host=headers.get("host");
                String login=headers.get("login");
                String passcode=headers.get("passcode");
                //ConnectFrame connectFrame=new ConnectFrame(frame[1],frame[2],frame[3],frame[4]);
                //connections.addConnection(connectFrame.getLogin(), new ConnectionHandler<T>());
                break;
                case "SUBSCRIBE":
                //SubscribeFrame subFrame=new SubscribeFrame(frame[1], frame[2]);
                int sub_id=Integer.parseInt(headers.get("id"));
                String sub_destination=headers.get("destination");
                break;
                case "UNSUBSCRIBE":
                //UnSubscribedFrame unsubFrame=new UnSubscribedFrame(frame[1]);
                int unsub_id=Integer.parseInt(headers.get("id"));
                break;
                case "SEND":
                //SendFrame sendFrame=new SendFrame(frame[1], frame[2]);
                String send_destination=headers.get("destination");
                String send_body=headers.get("body");//will fail
                break;
                case "CONNECTED":
                //ConnectedFrame connectedFrame=new ConnectedFrame(frame[1]);
                int connected_version=Integer.parseInt(headers.get("version"));
                break;
                case "DISCONNECT":
                //Disconnect disconnect=new Disconnect(frame[1]);
                int disconnect_reciept=Integer.parseInt(headers.get("reciept"));;
                //connections.disconnect(disconnect_id);
                break;
                case "RECIEPT":
                //RecieptFrame recieptFrame=new RecieptFrame(frame[1]);
                int reciept_id=Integer.parseInt(headers.get("reciept-id"));
                break;
                case "MESSAGE":
                //MessageFrame messageFrame=new MessageFrame(frame[1],frame[2],frame[3],frame[4]);
                int subscription=Integer.parseInt(headers.get("subscription")); // A client-unique ID
                String message_destination=headers.get("destination");
                int messageId=Integer.parseInt(headers.get("message-id")); // A server-unique ID for the message
                String message_body=headers.get("body");
                break;
                default:
                    break;
            }
        
    }


    @Override
    public boolean shouldTerminate() {
       return shouldTerminate;
    }

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
        return headers;
     }
 
}
