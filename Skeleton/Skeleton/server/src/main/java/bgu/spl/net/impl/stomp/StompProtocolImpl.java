package bgu.spl.net.impl.stomp;

import bgu.spl.net.api.StompMessagingProtocol;
import bgu.spl.net.srv.Connections;

public class StompProtocolImpl  implements StompMessagingProtocol<String> {
    


    @Override
    public void start (int connectionId, Connections<String> connections){

    }

    @Override
    public void process(String message) {

    }

    @Override
    public boolean shouldTerminate() {
       return false;
    }




}
