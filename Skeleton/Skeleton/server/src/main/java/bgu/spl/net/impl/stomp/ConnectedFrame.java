package bgu.spl.net.impl.stomp;

public class ConnectedFrame extends StompFrame{
    String version;
    

    public ConnectedFrame(String version)
    {
        super("CONNECTED",null);
        this.version=version;
    }
    public String getVersion(){
        return version;
    }
}
