package bgu.spl.net.impl.stomp;

import java.util.HashMap;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;

public class StompFrame {
    private String STOMPCommand; // MESSAGE / SEND / CONNECT / DISCONNECT / SUBSCRIBE / UNSUBSCRIBE
    private Map<String, String> headers;
    private String body;


    StompFrame(String frame) { //Creating a frame object using a frame string
        headers = new HashMap<>();
        List<String> frameLines = split(frame, '\n');
        STOMPCommand = frameLines.get(0);
        boolean hasBody = false;
        int bodyIndex = 0;
        for (int i = 1; i < frameLines.size(); i++) {
            if (i < (frameLines.size() - 2)) {
                if (frameLines.get(i) == "\n" && frameLines.get(i+1) == "\n" && frameLines.get(i+2) != "\0") {
                    hasBody = true;
                    bodyIndex = i + 2;
                    break;
                }
            }
            if (frameLines.get(i) != "\n" && frameLines.get(i) != "\0") {
                String line = frameLines.get(i);
                int index = line.indexOf(':');
                String header = line.substring(0, index);
                String value = line.substring(index + 1);
                headers.put(header, value);
            }
        }
        if (hasBody) {
            String bodyString = "";
            for (int i = bodyIndex; i < frameLines.size(); i++) {
                if (frameLines.get(i) != "\0") {
                    bodyString = bodyString + frameLines.get(i);
                }
            }
            body = bodyString;
        }
    }
    StompFrame(String command, Map<String, String> headers, String body) {
        this.STOMPCommand = command;
        this.headers = headers;
        this.body = body;
    }

    private List<String> split(String line, char delimiter) {
        List<String> parts = new LinkedList<>();
        while (line.length() > 0)
        {
            String part = "";
            int i = 0;
            for (i = 0; i < line.length(); i++)
            {
                if (delimiter == line.charAt(i))
                {
                    if (part.length() > 0) {
                        parts.add(part);
                        parts.add("\n");
                        if((i != line.length() - 1) && delimiter == line.charAt(i+1)) {
                            parts.add("\n");
                        }
                    }
                    break;
                }
                else
                    part += line.charAt(i);
            }
            if (i == line.length())
            {
                parts.add(part);
                line = "";
            }
            else
                line = line.substring(i + 1);
        }
        return parts;
    }

    public String createFrame() { //Creating a string of frame from the fields
        String frame = STOMPCommand + "\n";
        for (String header: headers.keySet()) {
            frame = frame + header + ":" + headers.get(header) + "\n";
        }
        frame = frame + "\n";
        if (body.length() > 0) {
            frame = frame + body + "\n";
        }
        return frame;
    }
    public String getCommand() {
        return STOMPCommand;
    }
    public Map<String, String> getHeaders() {
        return headers;
    }
    public String getBody() {
        return body;
    }
}
