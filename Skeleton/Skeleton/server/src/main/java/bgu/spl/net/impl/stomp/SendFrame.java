package bgu.spl.net.impl.stomp;

public class SendFrame extends StompFrame{
    String destination;

    public SendFrame(String destination,String body)
    {
        super("SEND", body);
        this.destination=destination;
    }
    public String getDestination(){
        return destination;
    }
}
