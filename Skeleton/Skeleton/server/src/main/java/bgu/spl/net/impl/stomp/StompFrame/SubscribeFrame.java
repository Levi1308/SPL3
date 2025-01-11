package bgu.spl.net.impl.stomp.StompFrame;


public class SubscribeFrame extends StompFrame{

    String id;
    String destination;

    public SubscribeFrame(String destination,String id)
    {
        super("SUBSCRIBE",  null);
        this.destination=destination;
        this.id=id;
    }

    public String getId(){
        return id;
    }
    public String getDestination(){
        return destination;
    }
}
