package bgu.spl.net.impl.stomp.StompFrame;
import java.util.HashMap;
import java.util.Map;

public class ErrorFrame extends StompFrame {
    private String message; // Description of the error
    private String receiptId; // Optional receipt ID for the related frame

    public ErrorFrame(String receiptId,String message, String body) {
        super("ERROR", body);
        this.message = message;
        this.receiptId = receiptId;
    }

    // Getters
    public String getMessage() {
        return message;
    }

    public String getReceiptId() {
        return receiptId;
    }
}