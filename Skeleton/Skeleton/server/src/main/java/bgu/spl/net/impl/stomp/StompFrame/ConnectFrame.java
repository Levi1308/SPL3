package bgu.spl.net.impl.stomp.StompFrame;

import java.util.List;

public class ConnectFrame extends StompFrame{
    String version;
    String host;
    String login;
    String passcode;
    public ConnectFrame(String version,String host,String login,String passcode,String body)
    {
        super("CONNECT",null);
        this.version=version;
        this.host="stomp.cs.bgu.ac.il";
        this.login=login;
        this.passcode=passcode;
    }
}
