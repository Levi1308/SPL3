package bgu.spl.net.impl.stomp;

public class ErrorFrame extends StompFrame{
    String message;
    String receiptId;

    public ErrorFrame(String message,String receiptId,String body){
        super("ERROR");
            this.message=message;
            this.receiptId=receiptId;
    }
    public String getMessage(){
        return message;
    }
    public String getRecieptId(){
        return receiptId;
    }
}