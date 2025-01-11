package bgu.spl.net.impl.stomp.Frames;

public class UnSubscribedFrame extends StompFrame{

    String id;


    public UnSubscribedFrame(String id)
    {
        super("UNSUBSCRIBE",  null);
        this.id=id;
    }

    public String getId(){
        return id;
    }
}
