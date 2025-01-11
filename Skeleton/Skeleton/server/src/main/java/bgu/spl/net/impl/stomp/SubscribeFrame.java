package bgu.spl.net.impl.stomp;

public class SubscribeFrame extends StompFrame{
    String destination;
    String id;


    public SubscribeFrame(String destination,String id)
    {
        super("SUBSCRIBE",  null);
        this.destination=destination;
        this.id=id;
    }
    public String getDestination(){
        return destination;
    }
    public String getId(){
        return id;
    }

}
