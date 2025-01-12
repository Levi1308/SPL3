package bgu.spl.net.impl.stomp.Frames;

public class RecieptFrame extends StompFrame{
    String versionId;
    public RecieptFrame(String version)
    {
        super("RECIEPT",null);
        this.versionId=version;
    }

    public String getVersionId(){
        return versionId;
    }
}
