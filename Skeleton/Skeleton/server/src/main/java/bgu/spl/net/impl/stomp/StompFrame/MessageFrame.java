package bgu.spl.net.impl.stomp.StompFrame;

public class MessageFrame extends StompFrame{
    String subscription; //: a client-unique id 
    String destination;
    String messageId; //a server-unique id that for the message
    public MessageFrame(String subscription,String destination,String messageId,String body)
    {
        super("MESSAGE", body);
        this.subscription=subscription;
        this.destination=destination;
        this.messageId=messageId;
    }
}