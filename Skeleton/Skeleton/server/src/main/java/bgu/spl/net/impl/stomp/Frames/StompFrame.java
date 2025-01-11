package bgu.spl.net.impl.stomp.Frames;

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

