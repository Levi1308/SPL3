package bgu.spl.net.impl.stomp;

public class RecieptFrame extends StompFrame{
    String versionId;
    public RecieptFrame(String version)
    {
        super("RECIEPT");
        this.versionId=version;
    }
    public String getVersionId(){
        return versionId;
    }
}
