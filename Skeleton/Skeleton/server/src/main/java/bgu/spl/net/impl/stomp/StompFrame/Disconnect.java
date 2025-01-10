package bgu.spl.net.impl.stomp.StompFrame;

public class Disconnect extends StompFrame{
    
    String id;


    public Disconnect(String id)
    {
        super("DISCONNECT",  null);
        this.id=id;
    }
}
