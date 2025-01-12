package bgu.spl.net.impl.stomp;

public class UnSubscribedFrame extends StompFrame{

    String id;


    public UnSubscribedFrame(String id)
    {
        super("UNSUBSCRIBE");
        this.id=id;
    }
    public String getId(){
        return id;
    }

}
