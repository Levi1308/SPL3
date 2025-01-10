package bgu.spl.net.impl.stomp.StompFrame;

public class UnSubscribedFrame extends StompFrame{

    String id;


    public UnSubscribedFrame(String id)
    {
        super("UNSUBSCRIBE",  null);
        this.id=id;
    }

}
