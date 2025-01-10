package bgu.spl.net.impl.stomp.StompFrame;

import java.util.List;
import java.util.concurrent.ConcurrentLinkedQueue;

public class StompFrame {
    String commandName;
    String bodyframe;
    StompFrame stompFrame;
    public StompFrame(String command,String bodyframe)
    {
        this.commandName=command;
        this.bodyframe=bodyframe;
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