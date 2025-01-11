package bgu.spl.net.impl.stomp;

public class Disconnect extends StompFrame{
    
    String id;


    public Disconnect(String id)
    {
        super("DISCONNECT",  null);
        this.id=id;
    }
    public String getId(){
        return id;
    }
}
