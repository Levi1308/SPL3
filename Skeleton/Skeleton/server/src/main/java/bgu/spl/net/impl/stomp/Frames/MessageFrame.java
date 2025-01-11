package bgu.spl.net.impl.stomp.Frames;
public class MessageFrame extends StompFrame {
    private String subscription; // A client-unique ID
    private String destination;
    private String messageId; // A server-unique ID for the message

    public MessageFrame(String subscription, String destination, String messageId, String body) {
        super("MESSAGE", body);
        this.subscription = subscription;
        this.destination = destination;
        this.messageId = messageId;
    }

    // Getter for subscription
    public String getSubscription() {
        return subscription;
    }

    // Getter for destination
    public String getDestination() {
        return destination;
    }

    // Getter for messageId
    public String getMessageId() {
        return messageId;
    }
}