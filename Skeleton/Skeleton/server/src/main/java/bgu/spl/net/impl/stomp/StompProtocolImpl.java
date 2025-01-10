package bgu.spl.net.impl.stomp;

import bgu.spl.net.api.StompMessagingProtocol;
import bgu.spl.net.impl.stomp.StompFrame.StompFrame;
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
        
    }

    

    @Override
    public boolean shouldTerminate() {
       return shouldTerminate;
    }

    




}
