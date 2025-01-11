package bgu.spl.net.impl.stomp.StompFrame;

import java.util.List;
import java.util.concurrent.ConcurrentLinkedQueue;
public class StompFrame {
    private String commandName;
    private String bodyframe;

    public StompFrame(String commandName, String bodyframe) {
        this.commandName = commandName;
        this.bodyframe = bodyframe;
    }

    // Getter for commandName
    public String getCommandName() {
        return commandName;
    }

    // Getter for bodyframe
    public String getBodyframe() {
        return bodyframe;
    }
}





/*
    public StompFrame(String commnd){
        switch (commnd) {
            case "ERROR":
                stompFrame=new ErrorFrame();
                break;
            case "CONNECT":
            stompFrame=new ConnectFrame();
            break;
            case "SUBSCRIBE":
            stompFrame=new SubscribeFrame();
            break;
            case "UNSUBSCRIBE":

            break;
            case "SEND":

            break;
            case "CONNECTED":

            break;
            case "DISCONNECT":

            break;
            case "RECIEPT":

            break;
            case "MESSAGE":

            break;
            default:
                break;
        }
    }
        */